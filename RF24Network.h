/*
 Copyright (C) 2011 James Coliz, Jr. <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#ifndef __RF24NETWORK_H__
#define __RF24NETWORK_H__

#include <stdint.h>

class RF24;

/**
 * Node configuration for a specific node
 *
 * Each logical node address needs one of these
 */
struct RF24NodeLine
{
  uint64_t talking_pipe; /**< Pipe address for talking to parent node */
  uint64_t listening_pipe; /**< Pipe address for listening to parent node */
  uint16_t parent_node; /**< Logical address of our parent node */
};

/**
 * Header which is sent with each message
 *
 * The frame put over the air consists of this header and a message
 */
struct RF24NetworkHeader
{
  uint16_t from_node;
  uint16_t to_node;
  RF24NetworkHeader() {}
  RF24NetworkHeader(uint16_t _to): to_node(_to) {}
  const char* toString(void) const;
};

/**
 * Enumeration for directionality of the network
 *
 * Uni-directional networks can only talk to the base.  This is what you would set up in
 * a mesh of sensor nodes who were sending readings to the base.  Bi-directional mode
 * allows any node to address any other node, up or down any part of the tree.
 */

typedef enum { RF24_NET_UNIDIRECTIONAL = 0, RF24_NET_BIDIRECTIONAL } rf24_direction_e;

/**
 * Network Layer for RF24 Radios
 *
 * This class implements an OSI Network Layer using nRF24L01(+) radios driven
 * by RF24 library.
 *
 * @warning It is nowhere near done.  I just started coding it!!
 *
 * The layer provides:
 * @li Host Addressing.  Each node has a logical address on the local network.
 * @li Message Forwarding.  Messages can be sent from one node to any other, and
 * this layer will get them there no matter how many hops it takes.
 * @li Someday, fragmentation/reassembly. 
 */

class RF24Network
{
public:
  RF24Network( RF24& _radio, const RF24NodeLine* _topology);

  /**
   * Bring up the network
   *
   * Uni-directional networks can only talk to the base.  This is what you would set up in
   * a mesh of sensor nodes who were sending readings to the base.  Bi-directional mode
   * allows any node to address any other node, up or down any part of the tree.
   *
   * @warning Be sure to 'begin' the radio first.
   *
   * @param _channel The RF channel to operate on
   * @param _node_address The logical address of this node
   * @param _direction Whether this is a bi- or uni-directional network.
   */
  void begin(uint8_t _channel, uint16_t _node_address, rf24_direction_e _direction );
  
  /**
   * Main layer loop
   *
   * This function must be called regularly to keep the layer going.  This is where all
   * the action happens!
   */
  void update(void);

  /**
   * Test whether there is a message available for this node
   * 
   * @return Whether there is a message available for this node
   */
  bool available(void);

  /**
   * Read a message
   *
   * @param[out] header The header (envelope) of this message
   * @param[out] buf Pointer to memory where the message should be placed
   * @param maxlen The largest message size which can be held in @p buf
   * @return The total number of bytes copied into @p buf
   */
  size_t read(RF24NetworkHeader& header, void* buf, size_t maxlen);
  
  /**
   * Send a message
   *
   * @param[in,out] header The header (envelope) of this message.  The critical
   * thing to fill in is the @p to_node field so we know where to send the
   * message.  It is then updated with the details of the actual header sent.
   * @param buf Pointer to memory where the message is located 
   * @param len The size of the message 
   * @return Whether the message was successfully received 
   */
  bool write(RF24NetworkHeader& header,const void* buf, size_t len);

protected:
  void open_pipes(void);
  uint16_t find_node( uint16_t current_node, uint16_t target_node );
  bool write(uint16_t);
  bool enqueue(void);

private:
  RF24& radio; /**< Underlying radio driver, provides link/physical layers */ 
  uint16_t node_address; /**< Logical node address of this unit, 1 .. UINT_MAX */
  const RF24NodeLine* topology; /**< Mapping table of logical node addresses to physical RF pipes */
  uint16_t num_nodes; /**< Number of nodes in the topology table */
  const static short frame_size = 32;
  uint8_t frame_buffer[frame_size];
  uint8_t frame_queue[5*frame_size];
  uint8_t* next_frame;

};
#endif // __RF24_H__
// vim:ai:cin:sts=2 sw=2 ft=cpp
