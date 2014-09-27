/*
 Copyright (C) 2011 James Coliz, Jr. <maniacbug@ymail.com>
 Copyright (C) 2014 Rei <devel@reixd.net> (Initial development portion of fragmentation code only)

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 TMRh20 2014 - Optimization of RF24 and RF24Network Libs
 */

#ifndef __RF24NETWORK_H__
#define __RF24NETWORK_H__

/**
 * @file RF24Network.h
 *
 * Class declaration for RF24Network
 */


#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>
#include <stddef.h>
#include <assert.h>
#include <map>
#include <utility>      // std::pair
#include <queue>
#include "RF24Network_config.h"

#define MAX_FRAME_SIZE 32
#define MAX_FRAME_BUFFER_SIZE 255
#define MAX_PAYLOAD_SIZE 1500
#define MAX_LRU_CACHE_SIZE 32

/**
 * Network Management message types for management of network frames and messages
 * System discard types (128 to 147) Contain no user data, just additional system sub-types sent for informational purposes. (Initially within NETWORK_ACK responses)
 * System retained types (148-167) Contain user data
 *
 * System types can also contain sub-types, included as information, TBD
 *
 */

/* Header types range */
#define MIN_USER_DEFINED_HEADER_TYPE 0
#define MAX_USER_DEFINED_HEADER_TYPE 127

/* System Discard Types */
#define NETWORK_ACK_REQUEST 128
#define NETWORK_ACK 129
#define NETWORK_POLL 130

/*System-Sub Types (0-255)*/
#define NETWORK_REQ_STREAM 11;

/* System retained types */
#define NETWORK_FIRST_FRAGMENT 148
#define NETWORK_MORE_FRAGMENTS 149
#define NETWORK_LAST_FRAGMENT 150
/* System retained - response messages */
#define NETWORK_REQ_ADDRESS 151
#define NETWORK_ADDR_RESPONSE 152

/** Defines for handling written payloads */
#define TX_NORMAL 0
#define TX_ROUTED 1
#define USER_TX_TO_PHYSICAL_ADDRESS 2
#define USER_TX_TO_LOGICAL_ADDRESS 3
#define USER_TX_MULTICAST 4


class RF24;

/**
 * Header which is sent with each message
 *
 * The frame put over the air consists of this header and a message
 */
struct RF24NetworkHeader {
  uint16_t from_node; /**< Logical address where the message was generated */
  uint16_t to_node; /**< Logical address where the message is going */
  uint16_t id; /**< Sequential message ID, incremented every message */
  unsigned char type; /**< Type of the packet.  0-127 are user-defined types, 128-255 are reserved for system */
  unsigned char fragment_id; /**< Used to count the number of fragments of the payload. Zero (0) means no more fragments left. */
  static uint16_t next_id; /**< The message ID of the next message to be sent */

  /**
   * Default constructor
   *
   * Simply constructs a blank header
   */
  RF24NetworkHeader() {}

  /**
   * Send constructor
   *
   * Use this constructor to create a header and then send a message
   *
   * @code
   *  RF24NetworkHeader header(recipient_address,'t');
   *  network.write(header,&message,sizeof(message));
   * @endcode
   *
   * @param _to The logical node address where the message is going
   * @param _type The type of message which follows.  Only 0-127 are allowed for
   * user messages.
   */
  RF24NetworkHeader(uint16_t _to, unsigned char _type = 0): to_node(_to), id(next_id++), type(_type&0x7f) {}

  /**
   * Create debugging string
   *
   * Useful for debugging.  Dumps all members into a single string, using
   * internal static memory.  This memory will get overridden next time
   * you call the method.
   *
   * @return String representation of this object
   */
  const char* toString(void) const;

};

/**
 * Frame structure for each message
 *
 * The frame put over the air consists of a header and a message payload
 */
struct RF24NetworkFrame {

  RF24NetworkHeader header; /**< Header which is sent with each message */
  size_t message_size; /**< The size in bytes of the payload length */
  uint8_t message_buffer[MAX_PAYLOAD_SIZE]; /**< Vector to put the frame payload that will be sent/received over the air */
  uint8_t total_fragments; /**<Total number of expected fragments*/

  /**
   * Default constructor
   *
   * Simply constructs a blank frame
   */
  RF24NetworkFrame() {}

  /**
   * Send constructor
   *
   * Use this constructor to create a frame with header and payload and then send a message
   */
  RF24NetworkFrame(uint16_t _to, unsigned char _type = 0, const void* _message = NULL, size_t _len = 0) :
                  header(RF24NetworkHeader(_to,_type)), message_size(_len), total_fragments(0) {
    if (_message && _len) {
      memcpy(message_buffer,_message,_len);
    }
  }

  /**
   * Send constructor
   *
   * Use this constructor to create a frame with header and payload and then send a message
   */
  RF24NetworkFrame(RF24NetworkHeader& _header, const void* _message = NULL, size_t _len = 0) :
                  header(_header), message_size(_len), total_fragments(0) {
    if (_message && _len) {
      memcpy(message_buffer,_message,_len);
    }
  }

  /**
   * Create debugging string
   *
   * Useful for debugging.  Dumps all members into a single string, using
   * internal static memory.  This memory will get overridden next time
   * you call the method.
   *
   * @return String representation of this object
   */
  const char* toString(void) const;

};

/**
 * TMRh20 2014 - Optimized Network Layer for RF24 Radios
 *
 * This class implements an OSI Network Layer using nRF24L01(+) radios driven
 * by RF24 library.
 */

class RF24Network {

public:
  /**
   * Construct the network
   *
   * @param _radio The underlying radio driver instance
   *
   */
  RF24Network( RF24& _radio );

  /**
   * Bring up the network
   *
   * @warning Be sure to 'begin' the radio first.
   *
   * @param _channel The RF channel to operate on
   * @param _node_address The logical address of this node
   */
  void begin(uint8_t _channel, uint16_t _node_address );

  void failures(uint32_t *_fails, uint32_t *_ok);

  /**
   * Main layer loop
   *
   * This function must be called regularly to keep the layer going.  This is where all
   * the action happens!
   */
  uint8_t update(void);

  /**
   * Test whether there is a message available for this node
   *
   * @return Whether there is a message available for this node
   */
  bool available(void);

  /**
   * Read the next available header
   *
   * Reads the next available header without advancing to the next
   * incoming message.  Useful for doing a switch on the message type
   *
   * If there is no message available, the header is not touched
   *
   * @param[out] header The header (envelope) of the next message
   */
  void peek(RF24NetworkHeader& header);

  /**
   * Read a message
   *
   * @param[out] header The header (envelope) of this message
   * @param[out] message Pointer to memory where the message should be placed
   * @param maxlen The largest message size which can be held in @p message
   * @return The total number of bytes copied into @p message
   */
  size_t read(RF24NetworkHeader& header, void* message, size_t maxlen);

  /**
   * Send a message
   *
   * @note Optimization: Extended timeouts/retries enabled. See txTimeout for more info.
   * @param[in,out] header The header (envelope) of this message.  The critical
   * thing to fill in is the @p to_node field so we know where to send the
   * message.  It is then updated with the details of the actual header sent.
   * @param message Pointer to memory where the message is located
   * @param len The size of the message
   * @return Whether the message was successfully received
   */
  bool write(RF24NetworkHeader& header,const void* message, size_t len);
  bool write(RF24NetworkHeader& header,const void* message, size_t len, uint16_t writeDirect);

  /**
   * This node's parent address
   *
   * @return This node's parent address, or -1 if this is the base
   */
  uint16_t parent() const;

   /**
   * @note: Optimization:This value is automatically assigned based on the node address
   * to reduce errors and increase throughput of the network.
   *
   * Sets the timeout period for individual payloads in milliseconds at staggered intervals.
   * Payloads will be retried automatically until success or timeout
   * Set to 0 to use the normal auto retry period defined by radio.setRetries()
   *
   */

  unsigned long txTimeout;

   /**
   * @note: Optimization: This new value defaults to 200 milliseconds.
   * This only affects payloads that are routed by one or more nodes.
   * This specifies how long to wait for an ack from across the network.
   * Radios routing directly to their parent or children nodes do not
   * utilize this value.
   */

   uint16_t routeTimeout;

  /**@}*/
  /**
   * @name Advanced Operation
   *
   *  Methods you can use to drive the network in more advanced ways
   */
  /**@{*/
#if defined RF24NetworkMulticast
   /**
   * Send a multicast message to multiple nodes at once
   * Allows messages to be rapidly broadcast through the network
   *
   * Multicasting is arranged in levels, with all nodes on the same level listening to the same address
   * Levels are assigned by network level ie: nodes 01-05: Level 1, nodes 011-055: Level 2
   * @see multicastLevel
   * @param message Pointer to memory where the message is located
   * @param len The size of the message
   * @param level Multicast level to broadcast to
   * @return Whether the message was successfully received
   */

  bool multicast(RF24NetworkHeader& header,const void* message, size_t len, uint8_t level);

  /**
  * By default, multicast addresses are divided into levels. Nodes 1-5 share a multicast address,
  * nodes n1-n5 share a multicast address, and nodes n11-n55 share a multicast address. This option
  * is used to override the defaults, and create custom multicast groups that all share a single
  * address.
  * The level should be specified in decimal format 1-6
  * Nodes can be configured to automatically forward multicast payloads to the next multicast level
  * @see multicastRelay
  *
  * @param level Levels 1 to 6 are available. All nodes at the same level will receive the same
  * messages if in range.
  */

  void multicastLevel(uint8_t level);

  /**
   * Set individual nodes to relay received multicast payloads onto the next multicast level.
   * Relay nodes will still receive the payloads, but they will also forward them on.
   * Relay nodes can have a maximum of 4 direct child nodes, but can multicast to any number
   * of nodes.
   * Multicast nodes and relays are configured to filter out duplicate payloads, so having multiple
   * relays in an area should not be a problem.
   */

  bool multicastRelay;

#endif
uint16_t addressOfPipe( uint16_t node,uint8_t pipeNo );

protected:
  //void open_pipes(void);
  //uint16_t find_node( uint16_t current_node, uint16_t target_node );
  bool write(uint16_t, uint8_t directTo);
  bool write_to_pipe( uint16_t node, uint8_t pipe, bool multicast );
  bool enqueue(RF24NetworkFrame frame);

  bool is_direct_child( uint16_t node );
  bool is_descendant( uint16_t node );
  uint16_t direct_child_route_to( uint16_t node );
  //uint8_t pipe_to_descendant( uint16_t node );
  void setup_address(void);
  bool _write(RF24NetworkHeader& header,const void* message, size_t len, uint16_t writeDirect);
  void appendFragmentToFrame(RF24NetworkFrame frame);

  struct logicalToPhysicalStruct{
	uint16_t send_node; 
	uint8_t send_pipe;
	bool multicast;
  }conversion;
  
  bool logicalToPhysicalAddress(logicalToPhysicalStruct *conversionInfo);
  
private:
#if defined (RF24NetworkMulticast)
  uint16_t lastMultiMessageID;
  uint8_t multicast_level;
#endif
  RF24& radio; /**< Underlying radio driver, provides link/physical layers */
  uint16_t node_address; /**< Logical node address of this unit, 1 .. UINT_MAX */
  uint8_t frame_size; /**< How large is each frame over the air */
  const static unsigned int max_frame_payload_size = MAX_FRAME_SIZE-sizeof(RF24NetworkHeader);
  uint8_t frame_buffer[MAX_FRAME_SIZE]; /**< Space to put the frame that will be sent/received over the air */
  std::queue<RF24NetworkFrame> frame_queue;
  std::map<std::pair<uint16_t, uint16_t>, RF24NetworkFrame> frameFragmentsCache;

  uint16_t parent_node; /**< Our parent's node address */
  uint8_t parent_pipe; /**< The pipe our parent uses to listen to us */
  uint16_t node_mask; /**< The bits which contain signfificant node address information */

  bool noListen; //FIXME
  uint32_t lastWriteTime; //FIXME

};

/**
 * @example helloworld_tx.cpp
 *
 * Simplest possible example of using RF24Network.  Put this sketch
 * on one node, and helloworld_rx.pde on the other.  Tx will send
 * Rx a nice message every 2 seconds which rx will print out for us.
 *
 */

/**
 * @example helloworld_rx.cpp
 *
 * Simplest possible example of using RF24Network.  Put this sketch
 * on one node, and helloworld_tx.pde on the other.  Tx will send
 * Rx a nice message every 2 seconds which rx will print out for us.
 */

/**
 * @example rx-test.cpp
 *
 * Example of receiving temperature reading via the network.
 * Transmitter should send a network 'T' message to read a single float representing the temperature
 */

/**
 * @mainpage Network Layer for RF24 Radios
 *
 * This class implements an <a href="http://en.wikipedia.org/wiki/Network_layer">OSI Network Layer</a> using nRF24L01(+) radios driven
 * by the optimized <a href="http://tmrh20.github.com/RF24/">RF24</a> library.
 * See http://tmrh20.github.io for documentation and downloads.
 */

#endif // __RF24NETWORK_H__
// vim:ai:cin:sts=2 sw=2 ft=cpp
