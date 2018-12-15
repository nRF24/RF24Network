/*
 Copyright (C) 2011 James Coliz, Jr. <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#ifndef __RF24NETWORK_H__
#define __RF24NETWORK_H__

/**
 * @file RF24Network.h
 *
 * Class declaration for RF24Network
 */

#include <stddef.h>
#include <stdint.h>
#include "RF24Network_config.h"

#if (defined (__linux) || defined (linux)) && !defined (__ARDUINO_X86__)
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
  
//ATXMega
#elif defined(XMEGA_D3)
  #include "../../rf24lib/rf24lib/RF24.h"
#endif


/**

 */  

/* Header types range */
#define MIN_USER_DEFINED_HEADER_TYPE 0
#define MAX_USER_DEFINED_HEADER_TYPE 127

/** 

 */
 
// ACK Response Types
/**
 * **Reserved network message types**
 *
 * The network will determine whether to automatically acknowledge payloads based on their general type <br>
 * 
 * **User types** (1-127) 1-64 will NOT be acknowledged <br>
 * **System types** (128-255) 192 through 255 will NOT be acknowledged<br>
 *
 * @defgroup DEFINED_TYPES Reserved System Message Types
 *
 * System types can also contain message data.
 *
 * @{
 */

/**
* A NETWORK_ADDR_RESPONSE type is utilized to manually route custom messages containing a single RF24Network address
* 
* Used by RF24Mesh
* 
* If a node receives a message of this type that is directly addressed to it, it will read the included message, and forward the payload
* on to the proper recipient. <br>
* This allows nodes to forward multicast messages to the master node, receive a response, and forward it back to the requester.
*/
#define NETWORK_ADDR_RESPONSE 128 
//#define NETWORK_ADDR_CONFIRM 129

/**
* Messages of type NETWORK_PING will be dropped automatically by the recipient. A NETWORK_ACK or automatic radio-ack will indicate to the sender whether the 
* payload was successful. The time it takes to successfully send a NETWORK_PING is the round-trip-time.
*/
#define NETWORK_PING 130

/**
 * External data types are used to define messages that will be passed to an external data system. This allows RF24Network to route and pass any type of data, such 
 * as TCP/IP frames, while still being able to utilize standard RF24Network messages etc.
 *
 * **Linux**
 * Linux devices (defined RF24_LINUX) will buffer all data types in the user cache. 
 *
 * **Arduino/AVR/Etc:** Data transmitted with the type set to EXTERNAL_DATA_TYPE will not be loaded into the user cache. <br>
 * External systems can extract external data using the following process, while internal data types are cached in the user buffer, and accessed using network.read() :
 * @code
 * uint8_t return_type = network.update();
 * if(return_type == EXTERNAL_DATA_TYPE){
 *     uint16_t size = network.frag_ptr->message_size;	
 *     memcpy(&myDataBuffer,network.frag_ptr->message_buffer,network.frag_ptr->message_size);
 * }		
 * @endcode
 */
#define EXTERNAL_DATA_TYPE 131

/**
 * Messages of this type designate the first of two or more message fragments, and will be re-assembled automatically.
 */
#define NETWORK_FIRST_FRAGMENT 148

/**
 * Messages of this type indicate a fragmented payload with two or more message fragments.
 */
#define NETWORK_MORE_FRAGMENTS 149

/**
 * Messages of this type indicate the last fragment in a sequence of message fragments.
 * Messages of this type do not receive a NETWORK_ACK
 */
#define NETWORK_LAST_FRAGMENT 150 
//#define NETWORK_LAST_FRAGMENT 201

// NO ACK Response Types
//#define NETWORK_ACK_REQUEST 192

/**
 * Messages of this type are used internally, to signal the sender that a transmission has been completed.
 * RF24Network does not directly have a built-in transport layer protocol, so message delivery is not 100% guaranteed.<br>
 * Messages can be lost via corrupted dynamic payloads, or a NETWORK_ACK can fail, while the message was actually successful.
 * 
 * NETWORK_ACK messages can be utilized as a traffic/flow control mechanism, since transmitting nodes will be forced to wait until
 * the payload is transmitted across the network and acknowledged, before sending additional data. 
 *
 * In the event that the transmitting device will be waiting for a direct response, manually sent by the recipient, a NETWORK_ACK is not required. <br>
 * User messages utilizing a 'type' with a decimal value of 64 or less will not be acknowledged across the network via NETWORK_ACK messages.
 */
#define NETWORK_ACK 193

/**
 * Used by RF24Mesh
 *
 * Messages of this type are used with multi-casting , to find active/available nodes.
 * Any node receiving a NETWORK_POLL sent to a multicast address will respond directly to the sender with a blank message, indicating the
 * address of the available node via the header.
 */ 
#define NETWORK_POLL 194

/**
 * Used by RF24Mesh
 *
 * Messages of this type are used to request information from the master node, generally via a unicast (direct) write.
 * Any (non-master) node receiving a message of this type will manually forward it to the master node using an normal network write.
 */
#define NETWORK_REQ_ADDRESS 195
//#define NETWORK_ADDR_LOOKUP 196
//#define NETWORK_ADDR_RELEASE 197
/** @} */

#define NETWORK_MORE_FRAGMENTS_NACK 200


/** Internal defines for handling written payloads */
#define TX_NORMAL 0
#define TX_ROUTED 1
#define USER_TX_TO_PHYSICAL_ADDRESS 2  //no network ACK
#define USER_TX_TO_LOGICAL_ADDRESS 3   // network ACK
#define USER_TX_MULTICAST 4

#define MAX_FRAME_SIZE 32   //Size of individual radio frames
#define FRAME_HEADER_SIZE 10 //Size of RF24Network frames - data

#define USE_CURRENT_CHANNEL 255 // Use current radio channel when setting up the network

/** Internal defines for handling internal payloads - prevents reading additional data from the radio
 * when buffers are full */
 #define FLAG_HOLD_INCOMING 1
 /** FLAG_BYPASS_HOLDS is mainly for use with RF24Mesh as follows:
  * a: Ensure no data in radio buffers, else exit
  * b: Address is changed to multicast address for renewal
  * c: Holds Cleared (bypass flag is set)
  * d: Address renewal takes place and is set
  * e: Holds Enabled (bypass flag off)
  */
 #define FLAG_BYPASS_HOLDS 2
 
 #define FLAG_FAST_FRAG 4
 
 #define FLAG_NO_POLL 8

class RF24;

/**
 * Header which is sent with each message
 *
 * The frame put over the air consists of this header and a message
 *
 * Headers are addressed to the appropriate node, and the network forwards them on to their final destination.
 */
struct RF24NetworkHeader
{
  uint16_t from_node; /**< Logical address where the message was generated */
  uint16_t to_node; /**< Logical address where the message is going */
  uint16_t id; /**< Sequential message ID, incremented every time a new frame is constructed */
  /**
   * Message Types:
   * User message types 1 through 64 will NOT be acknowledged by the network, while message types 65 through 127 will receive a network ACK.  
   * System message types 192 through 255 will NOT be acknowledged by the network. Message types 128 through 192 will receive a network ACK. <br>
   * <br><br>
   */
  unsigned char type; /**< <b>Type of the packet. </b> 0-127 are user-defined types, 128-255 are reserved for system */
  
  /**
  * During fragmentation, it carries the fragment_id, and on the last fragment
  * it carries the header_type.<br>
  */
  unsigned char reserved; /**< *Reserved for system use* */

  static uint16_t next_id; /**< The message ID of the next message to be sent (unused)*/

  /**
   * Default constructor
   *

   * Simply constructs a blank header
   */
  RF24NetworkHeader() {}

  /**
   * Send constructor  
   *  
   * @note Now supports automatic fragmentation for very long messages, which can be sent as usual if fragmentation is enabled. 
   *
   * Fragmentation is enabled by default for all devices except ATTiny <br>
   * Configure fragmentation and max payload size in RF24Network_config.h
   *  
   * Use this constructor to create a header and then send a message  
   *   
   * @code
   *  uint16_t recipient_address = 011;
   *  
   *  RF24NetworkHeader header(recipient_address,'t');
   *  
   *  network.write(header,&message,sizeof(message));
   * @endcode
   *
   * @param _to The Octal format, logical node address where the message is going
   * @param _type The type of message which follows.  Only 0-127 are allowed for
   * user messages. Types 1-64 will not receive a network acknowledgement.
   */

  RF24NetworkHeader(uint16_t _to, unsigned char _type = 0): to_node(_to), id(next_id++), type(_type) {}
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
 * Frame structure for internal message handling, and for use by external applications
 *
 * The actual frame put over the air consists of a header (8-bytes) and a message payload (Up to 24-bytes)<br>
 * When data is received, it is stored using the RF24NetworkFrame structure, which includes:
 * 1. The header 
 * 2. The size of the included message 
 * 3. The 'message' or data being received
 * 
 *
 */


 struct RF24NetworkFrame
{
  RF24NetworkHeader header; /**< Header which is sent with each message */
  uint16_t message_size; /**< The size in bytes of the payload length */
  
  /**
  * On Arduino, the message buffer is just a pointer, and can be pointed to any memory location.
  * On Linux the message buffer is a standard byte array, equal in size to the defined MAX_PAYLOAD_SIZE
  */
  #if defined (RF24_LINUX)
    uint8_t message_buffer[MAX_PAYLOAD_SIZE]; //< Array to store the message
  #else    
    uint8_t *message_buffer; //< Pointer to the buffer storing the actual message 
  #endif
  /**
   * Default constructor
   *
   * Simply constructs a blank frame. Frames are generally used internally. See RF24NetworkHeader.
   */
  //RF24NetworkFrame() {}
  
  RF24NetworkFrame() {}
  /**
   * Constructor - create a network frame with data
   * Frames are constructed and handled differently on Arduino/AVR and Linux devices (defined RF24_LINUX)
   *
   * <br>
   * **Linux:**
   * @param _header The RF24Network header to be stored in the frame
   * @param _message The 'message' or data.
   * @param _len The size of the 'message' or data.
   *
   * <br>
   * **Arduino/AVR/Etc.**
   * @see RF24Network.frag_ptr
   * @param _header The RF24Network header to be stored in the frame
   * @param _message_size The size of the 'message' or data
   * 
   *
   * Frames are used internally and by external systems. See RF24NetworkHeader.
   */
#if defined (RF24_LINUX)   
  RF24NetworkFrame(RF24NetworkHeader& _header, const void* _message = NULL, uint16_t _len = 0) :
                  header(_header), message_size(_len) {
    if (_message && _len) {
      memcpy(message_buffer,_message,_len);
    }
  }
#else  
  RF24NetworkFrame(RF24NetworkHeader &_header, uint16_t _message_size):
                  header(_header), message_size(_message_size){		  
  }
#endif  


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
 * 2014-2015 - Optimized Network Layer for RF24 Radios
 *
 * This class implements an OSI Network Layer using nRF24L01(+) radios driven
 * by RF24 library.
 */

class RF24Network
{

  /**@}*/
  /**
   * @name Primary Interface
   *
   *  These are the main methods you need to operate the network
   */
  /**@{*/
  
public:
  /**
   * Construct the network
   *
   * @param _radio The underlying radio driver instance
   *
   */

  RF24Network( RF24& _radio );

  /**
   * Bring up the network using the current radio frequency/channel.
   * Calling begin brings up the network, and configures the address, which designates the location of the node within RF24Network topology.
   * @note Node addresses are specified in Octal format, see <a href=Addressing.html>RF24Network Addressing</a> for more information.
   * @warning Be sure to 'begin' the radio first.
   *
   * **Example 1:** Begin on current radio channel with address 0 (master node)
   * @code
   * network.begin(00);
   * @endcode
   * **Example 2:** Begin with address 01 (child of master)
   * @code
   * network.begin(01);
   * @endcode
   * **Example 3:** Begin with address 011 (child of 01, grandchild of master)
   * @code
   * network.begin(011);
   * @endcode   
   *
   * @see begin(uint8_t _channel, uint16_t _node_address )
   * @param _node_address The logical address of this node
   *
   */
   
  inline void begin(uint16_t _node_address){
	  begin(USE_CURRENT_CHANNEL,_node_address);
  }

  /**
   * Main layer loop
   *
   * This function must be called regularly to keep the layer going.  This is where payloads are 
   * re-routed, received, and all the action happens.
   *
   * @see
   * 
   * @return Returns the type of the last received payload.
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
  uint16_t peek(RF24NetworkHeader& header);

  /**
   * Read the next available payload
   *
   * Reads the next available payload without advancing to the next
   * incoming message.  Useful for doing a transparent packet 
   * manipulation layer on top of RF24Network.
   *
   * @param[out] header The header (envelope) of this message
   * @param[out] message Pointer to memory where the message should be placed
   * @param maxlen Amount of bytes to copy to message.
   */
  void peek(RF24NetworkHeader& header, void* message, uint16_t maxlen);

  /**
   * Read a message
   *
   * @code
   * while ( network.available() )  {
   *   RF24NetworkHeader header;
   *   uint32_t time;
   *   network.peek(header);
   *   if(header.type == 'T'){
   *     network.read(header,&time,sizeof(time));
   *     Serial.print("Got time: ");
   *     Serial.println(time);
   *   }
   * }
   * @endcode
   * @param[out] header The header (envelope) of this message
   * @param[out] message Pointer to memory where the message should be placed
   * @param maxlen The largest message size which can be held in @p message
   * @return The total number of bytes copied into @p message
   */
  uint16_t read(RF24NetworkHeader& header, void* message, uint16_t maxlen);

  /**
   * Send a message
   *
   * @note RF24Network now supports fragmentation for very long messages, send as normal. Fragmentation
   * may need to be enabled or configured by editing the RF24Network_config.h file. Default max payload size is 120 bytes.
   * 
   * @code
   * uint32_t time = millis();
   * uint16_t to = 00; // Send to master
   * RF24NetworkHeader header(to, 'T'); // Send header type 'T'
   * network.write(header,&time,sizeof(time));
   * @endcode
   * @param[in,out] header The header (envelope) of this message.  The critical
   * thing to fill in is the @p to_node field so we know where to send the
   * message.  It is then updated with the details of the actual header sent.
   * @param message Pointer to memory where the message is located
   * @param len The size of the message
   * @return Whether the message was successfully received
   */
  bool write(RF24NetworkHeader& header,const void* message, uint16_t len);

  /**@}*/
  /**
   * @name Advanced Configuration
   *
   *  For advanced configuration of the network
   */
  /**@{*/
  

   /**
   * Construct the network in dual head mode using two radio modules.
   * @note Not working on RPi. Radios will share MISO, MOSI and SCK pins, but require separate CE,CS pins.
   * @code
   * 	RF24 radio(7,8);
   * 	RF24 radio1(4,5);
   * 	RF24Network(radio.radio1);
   * @endcode
   * @param _radio The underlying radio driver instance
   * @param _radio1 The second underlying radio driver instance
   */
   
  RF24Network( RF24& _radio, RF24& _radio1); 
  
	/**
	* By default, multicast addresses are divided into levels. 
	*
	* Nodes 1-5 share a multicast address, nodes n1-n5 share a multicast address, and nodes n11-n55 share a multicast address.<br>
	*
	* This option is used to override the defaults, and create custom multicast groups that all share a single
	* address. <br> 
	* The level should be specified in decimal format 1-6 <br>
	* @see multicastRelay
	* @param level Levels 1 to 6 are available. All nodes at the same level will receive the same
	* messages if in range. Messages will be routed in order of level, low to high by default, with the
	* master node (00) at multicast Level 0
	*/
	
	void multicastLevel(uint8_t level);

	/**
	* Enabling this will allow this node to automatically forward received multicast frames to the next highest
	* multicast level. Duplicate frames are filtered out, so multiple forwarding nodes at the same level should
	* not interfere. Forwarded payloads will also be received.
	* @see multicastLevel
	*/
	
	bool multicastRelay;

 /**
   * Set up the watchdog timer for sleep mode using the number 0 through 10 to represent the following time periods:<br>
   * wdt_16ms = 0, wdt_32ms, wdt_64ms, wdt_128ms, wdt_250ms, wdt_500ms, wdt_1s, wdt_2s, wdt_4s, wdt_8s
   * @code
   * 	setup_watchdog(7);   // Sets the WDT to trigger every second
   * @endcode
   * @param prescalar The WDT prescaler to define how often the node will wake up. When defining sleep mode cycles, this time period is 1 cycle.
   */
 void setup_watchdog(uint8_t prescalar);

    /**
   * @note: This value is automatically assigned based on the node address
   * to reduce errors and increase throughput of the network.
   *
   * Sets the timeout period for individual payloads in milliseconds at staggered intervals.
   * Payloads will be retried automatically until success or timeout
   * Set to 0 to use the normal auto retry period defined by radio.setRetries()
   *
   */

  uint32_t txTimeout; /**< Network timeout value */
  
  /**
   * This only affects payloads that are routed by one or more nodes.
   * This specifies how long to wait for an ack from across the network.
   * Radios sending directly to their parent or children nodes do not
   * utilize this value.
   */
  
   uint16_t routeTimeout; /**< Timeout for routed payloads */  
  
 
  /**@}*/
  /**
   * @name Advanced Operation
   *
   *  For advanced operation of the network
   */
  /**@{*/

  /**
   * Return the number of failures and successes for all transmitted payloads, routed or sent directly  
   * @note This needs to be enabled via #define ENABLE_NETWORK_STATS in RF24Network_config.h
   *
   *   @code  
   * bool fails, success;  
   * network.failures(&fails,&success);  
   * @endcode  
   *
   */
  void failures(uint32_t *_fails, uint32_t *_ok);
  
   #if defined (RF24NetworkMulticast)
  
  /**
   * Send a multicast message to multiple nodes at once
   * Allows messages to be rapidly broadcast through the network  
   *   
   * Multicasting is arranged in levels, with all nodes on the same level listening to the same address  
   * Levels are assigned by network level ie: nodes 01-05: Level 1, nodes 011-055: Level 2
   * @see multicastLevel
   * @see multicastRelay
   * @param message Pointer to memory where the message is located
   * @param len The size of the message
   * @param level Multicast level to broadcast to
   * @return Whether the message was successfully sent
   */
   
   bool multicast(RF24NetworkHeader& header,const void* message, uint16_t len, uint8_t level);
   
	
   #endif
   
   /**
   * Writes a direct (unicast) payload. This allows routing or sending messages outside of the usual routing paths.
   * The same as write, but a physical address is specified as the last option.
   * The payload will be written to the physical address, and routed as necessary by the recipient
   */
   bool write(RF24NetworkHeader& header,const void* message, uint16_t len, uint16_t writeDirect);

   /**
   * Sleep this node - For AVR devices only
   * @note NEW - Nodes can now be slept while the radio is not actively transmitting. This must be manually enabled by uncommenting
   * the #define ENABLE_SLEEP_MODE in RF24Network_config.h
   * @note Setting the interruptPin to 255 will disable interrupt wake-ups
   * @note The watchdog timer should be configured in setup() if using sleep mode.
   * This function will sleep the node, with the radio still active in receive mode.
   *
   * The node can be awoken in two ways, both of which can be enabled simultaneously:
   * 1. An interrupt - usually triggered by the radio receiving a payload. Must use pin 2 (interrupt 0) or 3 (interrupt 1) on Uno, Nano, etc.
   * 2. The watchdog timer waking the MCU after a designated period of time, can also be used instead of delays to control transmission intervals.
   * @code
   * if(!network.available()){ network.sleepNode(1,0); }  //Sleeps the node for 1 second or a payload is received
   *
   * Other options:
   * network.sleepNode(0,0);         // Sleep this node for the designated time period, or a payload is received.
   * network.sleepNode(1,255);       // Sleep this node for 1 cycle. Do not wake up until then, even if a payload is received ( no interrupt )
   * @endcode
   * @see setup_watchdog()
   * @param cycles: The node will sleep in cycles of 1s. Using 2 will sleep 2 WDT cycles, 3 sleeps 3WDT cycles...
   * @param interruptPin: The interrupt number to use (0,1) for pins two and three on Uno,Nano. More available on Mega etc.
   * @return True if sleepNode completed normally, after the specified number of cycles. False if sleep was interrupted
   */
 bool sleepNode( unsigned int cycles, int interruptPin, uint8_t INTERRUPT_MODE=0); //added interrupt mode support (default 0=LOW)


  /**
   * This node's parent address
   *
   * @return This node's parent address, or -1 if this is the base
   */
  uint16_t parent() const;
  
   /**
   * Provided a node address and a pipe number, will return the RF24Network address of that child pipe for that node
   */
   uint16_t addressOfPipe( uint16_t node,uint8_t pipeNo );
   
   /**
    * @note Addresses are specified in octal: 011, 034
    * @return True if a supplied address is valid
	*/
   bool is_valid_address( uint16_t node );

 /**@}*/
  /**
   * @name Deprecated
   *
   *  Maintained for backwards compatibility
   */
  /**@{*/  
  
  /**
   * Bring up the network on a specific radio frequency/channel.
   * @note Use radio.setChannel() to configure the radio channel
   *
   * **Example 1:** Begin on channel 90 with address 0 (master node)
   * @code
   * network.begin(90,0);
   * @endcode
   * **Example 2:** Begin on channel 90 with address 01 (child of master)
   * @code
   * network.begin(90,01);
   * @endcode
   * **Example 3:** Begin on channel 90 with address 011 (child of 01, grandchild of master)
   * @code
   * network.begin(90,011);
   * @endcode   
   *
   * @param _channel The RF channel to operate on
   * @param _node_address The logical address of this node
   *
   */
  void begin(uint8_t _channel, uint16_t _node_address );  
  
  /**@}*/
  /**
   * @name External Applications/Systems
   *
   *  Interface for External Applications and Systems ( RF24Mesh, RF24Ethernet )
   */
  /**@{*/
  
  /** The raw system frame buffer of received data. */
  
  uint8_t frame_buffer[MAX_FRAME_SIZE];   

  /** 
   * **Linux** <br>
   * Data with a header type of EXTERNAL_DATA_TYPE will be loaded into a separate queue.
   * The data can be accessed as follows:
   * @code
   * RF24NetworkFrame f;
   * while(network.external_queue.size() > 0){
   *   f = network.external_queue.front();
   *   uint16_t dataSize = f.message_size;
   *   //read the frame message buffer
   *   memcpy(&myBuffer,&f.message_buffer,dataSize);
   *   network.external_queue.pop();
   * }
   * @endcode
   */
  #if defined (RF24_LINUX)
    std::queue<RF24NetworkFrame> external_queue;
  #endif
  
  #if !defined ( DISABLE_FRAGMENTATION ) &&  !defined (RF24_LINUX)
  /**
  * **ARDUINO** <br>
  * The frag_ptr is only used with Arduino (not RPi/Linux) and is mainly used for external data systems like RF24Ethernet. When
  * an EXTERNAL_DATA payload type is received, and returned from network.update(), the frag_ptr will always point to the starting
  * memory location of the received frame. <br>This is used by external data systems (RF24Ethernet) to immediately copy the received
  * data to a buffer, without using the user-cache.
  * 
  * @see RF24NetworkFrame
  * 
 * @code
 * uint8_t return_type = network.update();
 * if(return_type == EXTERNAL_DATA_TYPE){
 *     uint16_t size = network.frag_ptr->message_size;	
 *     memcpy(&myDataBuffer,network.frag_ptr->message_buffer,network.frag_ptr->message_size);
 * }		
 * @endcode  
  * Linux devices (defined as RF24_LINUX) currently cache all payload types, and do not utilize frag_ptr. 
  */
  RF24NetworkFrame* frag_ptr;
  #endif

  /**
  * Variable to determine whether update() will return after the radio buffers have been emptied (DEFAULT), or
  * whether to return immediately when (most) system types are received. 
  * 
  * As an example, this is used with RF24Mesh to catch and handle system messages without loading them into the user cache.
  *
  * The following reserved/system message types are handled automatically, and not returned.
  *
  * | System Message Types <br> (Not Returned) |
  * |-----------------------|
  * | NETWORK_ADDR_RESPONSE |
  * | NETWORK_ACK           |
  * | NETWORK_PING          |
  * | NETWORK_POLL <br>(With multicast enabled) |
  * | NETWORK_REQ_ADDRESS   |  
  *
  */  
  bool returnSysMsgs;

  /**
  * Network Flags allow control of data flow
  *
  * Incoming Blocking: If the network user-cache is full, lets radio cache fill up. Radio ACKs are not sent when radio internal cache is full.<br>
  * This behaviour may seem to result in more failed sends, but the payloads would have otherwise been dropped due to the cache being full.<br>
  * 
  * | FLAGS | Value | Description |
  * |-------|-------|-------------|
  * |FLAG_HOLD_INCOMING| 1(bit_1) | INTERNAL: Set automatically when a fragmented payload will exceed the available cache |
  * |FLAG_BYPASS_HOLDS| 2(bit_2) | EXTERNAL: Can be used to prevent holds from blocking. Note: Holds are disabled & re-enabled by RF24Mesh when renewing addresses. This will cause data loss if incoming data exceeds the available cache space|
  * |FLAG_FAST_FRAG| 4(bit_3) | INTERNAL: Replaces the fastFragTransfer variable, and allows for faster transfers between directly connected nodes. |
  * |FLAG_NO_POLL| 8(bit_4) | EXTERNAL/USER: Disables NETWORK_POLL responses on a node-by-node basis. |  
  * 
  */
  uint8_t networkFlags;
    
  private:

  uint32_t txTime;

  bool write(uint16_t, uint8_t directTo);
  bool write_to_pipe( uint16_t node, uint8_t pipe, bool multicast );
  uint8_t enqueue(RF24NetworkHeader *header);

  bool is_direct_child( uint16_t node );
  bool is_descendant( uint16_t node );
  
  uint16_t direct_child_route_to( uint16_t node );
  //uint8_t pipe_to_descendant( uint16_t node );
  void setup_address(void);
  bool _write(RF24NetworkHeader& header,const void* message, uint16_t len, uint16_t writeDirect);
    
  struct logicalToPhysicalStruct{
	uint16_t send_node; 
	uint8_t send_pipe;
	bool multicast;
  };
  
  bool logicalToPhysicalAddress(logicalToPhysicalStruct *conversionInfo);
  
  
  RF24& radio; /**< Underlying radio driver, provides link/physical layers */
#if defined (DUAL_HEAD_RADIO)
  RF24& radio1;
#endif
#if defined (RF24NetworkMulticast)  
  uint8_t multicast_level;  
#endif
  uint16_t node_address; /**< Logical node address of this unit, 1 .. UINT_MAX */
  //const static int frame_size = 32; /**< How large is each frame over the air */
  uint8_t frame_size;
  const static unsigned int max_frame_payload_size = MAX_FRAME_SIZE-sizeof(RF24NetworkHeader);

  #if defined (RF24_LINUX)
    std::queue<RF24NetworkFrame> frame_queue;
	std::map< uint16_t, RF24NetworkFrame> frameFragmentsCache;
    bool appendFragmentToFrame(RF24NetworkFrame frame);
  
  #else
    #if  defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__) || defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
	  #if !defined (NUM_USER_PAYLOADS)
		#define NUM_USER_PAYLOADS 3
	  #endif
    #endif
	#if !defined (NUM_USER_PAYLOADS)
	  #define NUM_USER_PAYLOADS 5
	#endif
	  
	#if defined (DISABLE_USER_PAYLOADS)
    uint8_t frame_queue[1]; /**< Space for a small set of frames that need to be delivered to the app layer */
	#else
	uint8_t frame_queue[MAIN_BUFFER_SIZE]; /**< Space for a small set of frames that need to be delivered to the app layer */
	#endif
	
	uint8_t* next_frame; /**< Pointer into the @p frame_queue where we should place the next received frame */
	
	#if !defined ( DISABLE_FRAGMENTATION )
      RF24NetworkFrame frag_queue;
      uint8_t frag_queue_message_buffer[MAX_PAYLOAD_SIZE]; //frame size + 1 
    #endif
  
  #endif
  
  //uint8_t frag_queue[MAX_PAYLOAD_SIZE + 11];
  //RF24NetworkFrame frag_queue;
  
  uint16_t parent_node; /**< Our parent's node address */
  uint8_t parent_pipe; /**< The pipe our parent uses to listen to us */
  uint16_t node_mask; /**< The bits which contain signfificant node address information */
  
  #if defined ENABLE_NETWORK_STATS
  static uint32_t nFails;
  static uint32_t nOK;
  #endif  
  
public:

   

};

/**
 * @example helloworld_tx.ino
 *
 * Simplest possible example of using RF24Network.  Put this sketch
 * on one node, and helloworld_rx.pde on the other.  Tx will send
 * Rx a nice message every 2 seconds which rx will print out for us.
 */

/**
 * @example helloworld_rx.ino
 *
 * Simplest possible example of using RF24Network.  Put this sketch
 * on one node, and helloworld_tx.pde on the other.  Tx will send
 * Rx a nice message every 2 seconds which rx will print out for us.
 */

/**
 * @example Network_Ping.ino
 *
 * Example to give users an understanding of addressing and topology in the mesh network
 * Using this sketch, each node will send a ping to the base every
 * few seconds.  The RF24Network library will route the message across
 * the mesh to the correct node.
 *
 */

/**
 * @example Network_Ping_Sleep.ino
 *
 * Example: This is almost exactly the same as the Network_Ping example, but with use
 * of the integrated sleep mode.
 *
 * This example demonstrates how nodes on the network utilize sleep mode to conserve power. For example,
 * the radio itself will draw about 13.5mA in receive mode. In sleep mode, it will use as little as 22ua (.000022mA)
 * of power when not actively transmitting or receiving data. In addition, the Arduino is powered down as well,
 * dropping network power consumption dramatically compared to previous capabilities. <br>
 * Note: Sleeping nodes generate traffic that will wake other nodes up. This may be mitigated with further modifications. Sleep
 * payloads are currently always routed to the master node, which will wake up intermediary nodes. Routing nodes can be configured
 * to go back to sleep immediately.
 * The displayed millis() count will give an indication of how much a node has been sleeping compared to the others, as millis() will
 * not increment while a node sleeps.
 *<br>
 * - Using this sketch, each node will send a ping to every other node in the network every few seconds.<br>
 * - The RF24Network library will route the message across the mesh to the correct node.<br>
 *
 */

/**
 * @example sensornet.pde
 *
 * Example of a sensor network.
 * This sketch demonstrates how to use the RF24Network library to
 * manage a set of low-power sensor nodes which mostly sleep but
 * awake regularly to send readings to the base.
 */
/**
 * @mainpage Network Layer for RF24 Radios
 *
 * This class implements an <a href="http://en.wikipedia.org/wiki/Network_layer">OSI Network Layer</a> using nRF24L01(+) radios driven
 * by the newly optimized <a href="http://tmrh20.github.com/RF24/">RF24</a> library fork.
 *
 * @section Purpose Purpose/Goal
 *
 * Original: Create an alternative to ZigBee radios for Arduino communication.
 * 
 * New: Enhance the current functionality for maximum efficiency, reliability, and speed
 *
 * Xbees are excellent little radios, backed up by a mature and robust standard
 * protocol stack.  They are also expensive.
 *
 * For many Arduino uses, they seem like overkill.  So I am working to improve the current
 * standard for nRF24L01 radios.  The best RF24 modules are available for less than
 * $6 from many sources.  With the RF24Network layer, I hope to cover many
 * common communication scenarios.
 *
 * Please see the @ref Zigbee page for a comparison against the ZigBee protocols
 *
 * @section Features Features
 *
 * <b>Whats new? </b><br> 
 *  @li New: (Dec 8) Merge of RPi and Arduino code. Finally moving closer to a stable release.  Report issues at https://github.com/TMRh20/RF24Network/issues
 *  @li New functionality: (Dec 8) Support for fragmented multicast payloads on both RPi and Arduino
 *  @li New functionality: (Nov 24) Fragmentation & reassembly supported on both RPi and Arduino  
 *  @li Note: structure of network frames is changed, these are only used by external applications like RF24Ethernet and RF24toTUN, and for fragmentation
 *  @li New functionality: User message types 1 through 64 will not receive a network ack
 *
 * The layer provides:
 * @li <b>New</b> (2014): Network ACKs: Efficient acknowledgement of network-wide transmissions, via dynamic radio acks and network protocol acks.
 * @li <b>New</b> (2014): Updated addressing standard for optimal radio transmission.
 * @li <b>New</b> (2014): Extended timeouts and staggered timeout intervals. The new txTimeout variable allows fully automated extended timeout periods via auto-retry/auto-reUse of payloads.
 * @li <b>New</b> (2014): Optimization to the core library provides improvements to reliability, speed and efficiency. See https://tmrh20.github.io/RF24 for more info.
 * @li <b>New</b> (2014): Built in sleep mode using interrupts. (Still under development. (Enable via RF24Network_config.h))
 * @li Host Addressing.  Each node has a logical address on the local network.
 * @li Message Forwarding.  Messages can be sent from one node to any other, and
 * this layer will get them there no matter how many hops it takes.
 * @li Ad-hoc Joining.  A node can join a network without any changes to any
 * existing nodes.
 *
 * The layer does not provide:
 * @li Dynamic address assignment. (See RF24Mesh)
 * @li Layer 4 protocols (TCP/IP - See RF24Ethernet and RF24toTUN)
 *
 * @section More How to learn more
 *
 * @li <a href="classRF24Network.html">RF24Network Class Documentation</a>
 * @li <a href="AdvancedConfig.html"> Advanced Configuration Options</a>
 * @li <a href="Addressing.html"> Addressing format</a>
 * @li <a href="Tuning.html"> Topology and Overview</a>
 * @li <a href="https://github.com/TMRh20/RF24Network/archive/Development.zip">Download Current Development Package</a>
 * @li <a href="examples.html">Examples Page</a>.  Start with <a href="helloworld_rx_8ino-example.html">helloworld_rx</a> and <a href="helloworld_tx_8ino-example.html">helloworld_tx</a>.
 *
 * <b> Additional Information & Add-ons </b>  
 * @li <a href="https://github.com/TMRh20/RF24Mesh">RF24Mesh: Dynamic Mesh Layer for RF24Network Dev</a>
 * @li <a href="https://github.com/TMRh20/RF24Ethernet">RF24Ethernet: TCP/IP over RF24Network</a>
 * @li <a href="http://tmrh20.blogspot.com/2014/03/high-speed-data-transfers-and-wireless.html">My Blog: RF24 Optimization Overview</a>
 * @li <a href="http://tmrh20.blogspot.com/2014/03/arduino-radiointercomwireless-audio.html">My Blog: RF24 Wireless Audio</a>
 * @li <a href="http://maniacbug.github.com/RF24/">RF24: Original Author</a>
 * @section Topology Topology for Mesh Networks using nRF24L01(+)
 *
 * This network layer takes advantage of the fundamental capability of the nRF24L01(+) radio to
 * listen actively to up to 6 other radios at once.  The network is arranged in a
 * <a href="http://en.wikipedia.org/wiki/Network_Topology#Tree">Tree Topology</a>, where
 * one node is the base, and all other nodes are children either of that node, or of another.
 * Unlike a true mesh network, multiple nodes are not connected together, so there is only one
 * path to any given node.
 *
 * @section Octal Octal Addressing and Topology
 *
 * Each node must be assigned an 15-bit address by the administrator.  This address exactly
 * describes the position of the node within the tree.  The address is an octal number.  Each
 * digit in the address represents a position in the tree further from the base.
 *
 * @li Node 00 is the base node.
 * @li Nodes 01-05 are nodes whose parent is the base.
 * @li Node 021 is the second child of node 01.
 * @li Node 0321 is the third child of node 021, an so on.
 * @li The largest node address is 05555, so up to 781 nodes are allowed on a single channel.
 * An example topology is shown below, with 5 nodes in direct communication with the master node,
 * and multiple leaf nodes spread out at a distance, using intermediate nodes to reach other nodes.
 *
 *|   |    | 00 |    |    | 00 |    |    |    | Master Node (00)                                    |
 *|---|----|----|----|----|----|----|----|----|-----------------------------------------------------|
 *|   |    | 01 |    |    | 04 |    |    |    | 1st level children of master (00)                   |
 *|   | 011|    | 021|    |    |014 |    |    | 2nd level children of master. Children of 1st level.|
 *|111|    |    | 121| 221|    |    | 114|    | 3rd level children of master. Children of 2nd level.|
 *|   |    |    |    |1221|    |1114|2114|3114| 4th level children of master. Children of 3rd level.|
 *
 * @section Routing How routing is handled
 *
 * When sending a message using RF24Network::write(), you fill in the header with the logical
 * node address.  The network layer figures out the right path to find that node, and sends
 * it through the system until it gets to the right place.  This works even if the two nodes
 * are far separated, as it will send the message down to the base node, and then back out
 * to the final destination.
 *
 * All of this work is handled by the RF24Network::update() method, so be sure to call it
 * regularly or your network will miss packets.
 *
 * @section Startup Starting up a node
 *
 * When a node starts up, it only has to contact its parent to establish communication.
 * No direct connection to the Base node is needed.  This is useful in situations where
 * relay nodes are being used to bridge the distance to the base, so leaf nodes are out
 * of range of the base.
 *
 * @section Directionality Directionality
 *
 * By default all nodes are always listening, so messages will quickly reach
 * their destination.
 *
 * You may choose to sleep any nodes on the network if using interrupts. This is useful in a
 * case where the nodes are operating on batteries and need to sleep. This greatly decreases
 * the power requirements for a sensor network. The leaf nodes can sleep most of the time,
 * and wake every few minutes to send in a reading. Routing nodes can be triggered to wake up
 * whenever a payload is received See sleepNode() in the class documentation, and RFNetwork_config.h
 * to enable sleep mode.
 *
 *
 * @page Addressing Addressing Format: Understanding Addressing and Topology
 * An overview of addressing in RF24Network
 *
 * @section Overview Overview
 * The nrf24 radio modules typically use a 40-bit address format, requiring 5-bytes of storage space per address, and allowing a wide
 * array of addresses to be utilized. In addition, the radios are limited to direct communication with 6 other nodes while using the 
 * Enhanced-Shock-Burst (ESB) functionality of the radios.  
 *
 * RF24Network uses a simple method of data compression to store the addresses using only 2 bytes, in a format designed to represent the
 * network topology in an intuitive way.
 * See the <a href="Tuning.html"> Topology and Overview</a> page for more info regarding topology.
 *
 * @section Octal_Binary Decimal, Octal and Binary formats
 * 
 * Say we want to designate a logical address to a node, using a tree topology as defined by the manufacturer.
 * In the simplest format, we could assign the first node the address of 1, the second 2 and so on.  
 * Since a single node can only connect to 6 other nodes (1 parent and 5 children) subnets need to be created if using more than 6 nodes.<br>
 * In this case the children of node 1 could simply be designated as 11,21,31,41, and 51<br>
 * Children of node 2 could be designated as 12,22,32,42, and 52  
 * 
 * The above example is exactly how RF24Network manages the addresses, but they are represented in Octal format.
 * 
 * <b>Decimal, Octal and Binary</b>  
 * <table> 
 * <tr bgcolor="#a3b4d7" >
 * <td> Decimal </td> <td> Binary </td><td> Decimal </td> <td> Binary </td><td> Decimal </td> <td> Binary </td>
 * </tr><tr>
 * <td> 1 </td> <td> 00000001 </td><td> 11 </td> <td> 00001011 </td><td> 111 </td> <td> 01101111 </td>
 * </tr><tr bgcolor="#a3b4d7" >
 * <td> Octal </td> <td> Binary </td><td> Octal </td> <td> Binary </td><td> Octal </td> <td> Binary </td>
 * </tr><tr>
 * <td> 1 </td> <td> 00000001 </td><td> 011 </td> <td> 00001001 </td><td> 0111 </td> <td> 1001001 </td>
 * </tr>
 * </table>
 *
 *  
 * Since the numbers 0-7 can be represented in exactly three bits, each digit is represented by exactly 3 bits when viewed in octal format.  
 * This allows a very simple method of managing addresses via masking and bit shifting.  
 *   
 * @section DisplayAddresses Displaying Addresses
 *
 * When using Arduino devices, octal addresses can be printed in the following manner:
 * @code
 * uint16_t address = 0111; 
 * Serial.println(address,OCT);
 * @endcode
 *
 * Printf can also be used, if enabled, or if using linux/RPi
 * @code
 * uint16_t address = 0111;
 * printf("0%o\n",address);
 * @endcode
 *
 * See http://www.cplusplus.com/doc/hex/ for more information<br>
 * See the <a href="Tuning.html"> Topology and Overview</a> page for more info regarding topology.  
 *
 * @page AdvancedConfig Advanced Configuration
 *
 * RF24Network offers many features, some of which can be configured by editing the RF24Network_config.h file
 *
 * | Configuration Option | Description |
 * |----------------------|-------------|
 * |<b> #define RF24NetworkMulticast </b> | This option allows nodes to send and receive multicast payloads. Nodes with multicast enabled can also be configured to relay multicast payloads on to further multicast levels. See multicastRelay |
 * | <b> #define DISABLE_FRAGMENTATION </b> | Fragmentation is enabled by default, and uses an additional 144 bytes of memory. |
 * | <b> #define MAX_PAYLOAD_SIZE 144 </b> | The maximum size of payloads defaults to 144 bytes. If used with RF24toTUN and two Raspberry Pi, set this to 1514 (TAP) or 1500 (TUN) |
 * | <b> #define NUM_USER_PAYLOADS 5 </b> | This is the number of 24-byte payloads the network layer will cache for the user. If using fragmentation, this number * 24 must be larger than MAX_PAYLOAD_SIZE |
 * | <b> #define DISABLE_USER_PAYLOADS </b> | This option will disable user-caching of payloads entirely. Use with RF24Ethernet to reduce memory usage. (TCP/IP is an external data type, and not cached) |
 * | <b> #define ENABLE_SLEEP_MODE </b> | Uncomment this option to enable sleep mode for AVR devices. (ATTiny,Uno, etc) |
 * | **#define ENABLE_NETWORK_STATS** | Enable counting of all successful or failed transmissions, routed or sent directly |
 *
 ** @page Tuning Performance and Data Loss: Tuning the Network
 * Tips and examples for tuning the network and general operation.
 *
 *  <img src="tmrh20/topologyImage.jpg" alt="Topology" height="75%" width="75%">
 *
 * @section General Understanding Radio Communication and Topology
 * When a transmission takes place from one radio module to another, the receiving radio will communicate
 * back to the sender with an acknowledgement (ACK) packet, to indicate success. If the sender does not
 * receive an ACK, the radio automatically engages in a series of timed retries, at set intervals. The 
 * radios use techniques like addressing and numbering of payloads to manage this, but it is all done 
 * automatically by the nrf chip, out of sight from the user.
 *
 * When working over a radio network, some of these automated techniques can actually hinder data transmission to a degree.
 * Retrying failed payloads over and over on a radio network can hinder communication for nearby nodes, or 
 * reduce throughput and errors on routing nodes.
 *
 * Radios in this network are linked by <b>addresses</b> assigned to <b>pipes</b>. Each radio can listen
 * to 6 addresses on 6 pipes, therefore each radio has a parent pipe and 5 child pipes, which are used
 * to form a tree structure. Nodes communicate directly with their parent and children nodes. Any other
 * traffic to or from a node must be routed through the network.
 *
 * @section Topology Topology of RF24Network
 *
 * Anybody who is familiar at all with IP networking should be able to easily understand RF24Network topology. The
 * master node can be seen as the gateway, with up to 4 directly connected nodes. Each of those nodes creates a
 * subnet below it, with up to 4 additional child nodes. The numbering scheme can also be related to IP addresses,
 * for purposes of understanding the topology via subnetting. Nodes can have 5 children if multicast is disabled.
 *
 * Expressing RF24Network addresses in IP format:  
 *
 * As an example, we could designate the master node in theory, as Address 10.10.10.10 <br> 
 * The children nodes of the master would be 10.10.10.1, 10.10.10.2, 10.10.10.3, 10.10.10.4 and 10.10.10.5 <br> 
 * The children nodes of 10.10.10.1 would be 10.10.1.1, 10.10.2.1, 10.10.3.1, 10.10.4.1 and 10.10.5.1 <br> 
 *    
 * In RF24Network, the master is just 00  <br>
 * Children of master are 01,02,03,04,05  <br>
 * Children of 01 are 011,021,031,041,051  <br>
 * 
 * @section Network Routing
 *
 * Routing of traffic is handled invisibly to the user, by the network layer. If the network addresses are
 * assigned in accordance with the physical layout of the network, nodes will route traffic automatically
 * as required. Users simply constuct a header containing the appropriate destination address, and the network
 * will forward it through to the correct node. Individual nodes only route individual fragments, so if using
 * fragmentation, routing nodes do not need it enabled, unless sending or receiving fragmented payloads themselves.
 *
 * If routing data between parent and child nodes (marked by direct links on the topology image above) the network
 * uses built-in acknowledgement and retry functions of the chip to prevent data loss. When payloads are sent to
 * other nodes, they need to be routed. Routing is managed using a combination of built in ACK requests, and
 * software driven network ACKs. This allows all routing nodes to forward data very quickly, with only the final
 * routing node confirming delivery and sending back an
 * acknowledgement.
 *
 * Example: Node 00 sends to node 01. The nodes will use the built in auto-retry and auto-ack functions.<br>
 * Example: Node 00 sends to node 011. Node 00 will send to node 01 as before. Node 01 will forward the message to
 * 011. If delivery was successful, node 01 will also forward a message back to node 00, indicating success.
 *
 * Old Functionality: Node 00 sends to node 011 using auto-ack. Node 00 first sends to 01, 01 acknowledges.
 * Node 01 forwards the payload to 011 using auto-ack. If the payload fails between 01 and 011, node 00 has
 * no way of knowing. 
 * 
 * @note When retrying failed payloads that have been routed, there is a chance of duplicate payloads if the network-ack
 * is not successful. In this case, it is left up to the user to manage retries and filtering of duplicate payloads.
 *
 * Acknowledgements can and should be managed by the application or user. If requesting a response from another node,
 * an acknowledgement is not required, so a user defined type of 0-64 should be used, to prevent the network from
 * responding with an acknowledgement. If not requesting a response, and wanting to know if the payload was successful
 * or not, users can utilize header types 65-127.
 * 
 * @section TuningOverview Tuning Overview
 * The RF24 radio modules are generally only capable of either sending or receiving data at any given
 * time, but have built-in auto-retry mechanisms to prevent the loss of data. These values are adjusted
 * automatically by the library on startup, but can be further adjusted to reduce data loss, and
 * thus increase throughput of the network. This page is intended to provide a general overview of its
 * operation within the context of the network library, and provide guidance for adjusting these values.
 *
 * @section RetryTiming Auto-Retry Timing
 *
 * The core radio library provides the functionality of adjusting the internal auto-retry interval of the
 * radio modules. In the network configuration, the radios can be set to automatically retry failed
 * transmissions at intervals ranging anywhere from 500us (.5ms) up to 4000us (4ms). When operating any
 * number of radios larger than two, it is important to stagger the assigned intervals, to prevent the
 * radios from interfering with each other at the radio frequency (RF) layer.
 *
 * The library should provide fairly good working values, as it simply staggers the assigned values within
 * groups of radios in direct communication. This value can be set manually by calling radio.setRetries(X,15);
 * and adjusting the value of X from 1 to 15 (steps of 250us).
 *
 * @section AutoRetry Auto-Retry Count and Extended Timeouts
 *
 * The core radio library also provides the ability to adjust the internal auto-retry count of the radio
 * modules. The default setting is 15 automatic retries per payload, and can be extended by configuring
 * the network.txTimeout variable. This default retry count should generally be left at 15, as per the
 * example in the above section. An interval/retry setting of (15,15) will provide 15 retrys at intervals of
 * 4ms, taking up to 60ms per payload. The library now provides staggered timeout periods by default, but
 * they can also be adjusted on a per-node basis.
 *
 * The txTimeout variable is used to extend the retry count to a defined duration in milliseconds. See the
 * network.txTimeout variable. Timeout periods of extended duration (500+) will generally not help when payloads
 * are failing due to data collisions, it will only extend the duration of the errors. Extended duration timeouts
 * should generally only be configured on leaf nodes that do not receive data.
 *
 * @section Examples
 *
 * <b>Example 1:</b> Network with master node and three leaf nodes that send data to the master node. None of the leaf
 * nodes need to receive data.
 *
 * a: Master node uses default configuration<br>
 * b: Leaf nodes can be configured with extended timeout periods to ensure reception by the master.<br>
 * c:
 * @code
 * Leaf 01: network.txTimeout = 500;   Leaf 02: network.txTimeout = 573;  Leaf 03: network.txTimeout = 653;
 * @endcode
 * This configuration will provide a reduction in errors, as the timeouts have been extended, and are staggered
 * between devices.
 *
 *
 * <b>Example 2:</b> Network with master node and three leaf nodes that send data to the master node. The second leaf
 * node needs to receive configuration data from the master at set intervals of 1 second, and send data back to the
 * master node. The other leaf nodes will send basic sensor information every few seconds, and a few dropped payloads
 * will not affect the operation greatly.
 *
 * a: Master node configured with extended timeouts of .5 seconds, and increased retry delay:
 *   @code
 * 		radio.setRetries(11,15);
 * 		network.txTimeout = 500;
 * 	 @endcode
 * b: Second leaf node configured with a similar timeout period and retry delay:
 * @code
 * 		radio.setRetries(8,15);
 * 		network.txTimeout = 553;
 * @endcode
 * c: First and third leaf nodes configured with default timeout periods or slightly increased timout periods.
 *
 *
 *
 *
 *
 * @page Zigbee Comparison to ZigBee
 *
 * This network layer is influenced by the design of ZigBee, but does not implement it
 * directly.
 *
 * @section Advantage Which is better?
 *
 * ZigBee is a much more robust, feature-rich set of protocols, with many different vendors
 * providing compatible chips.
 *
 * RF24Network is cheap.  While ZigBee radios are well over $20, nRF24L01 modules can be found
 * for under $2.  My personal favorite is
 * <a href="http://www.mdfly.com/index.php?main_page=product_info&products_id=82">MDFly RF-IS2401</a>.
 *
 * @section Contrast Similiarities & Differences
 *
 * Here are some comparisons between RF24Network and ZigBee.
 *
 * @li Both networks support Star and Tree topologies.  Only Zigbee supports a true mesh.
 * @li In ZigBee networks, only leaf nodes can sleep
 * @li ZigBee nodes are configured using AT commands, or a separate Windows application.
 * RF24 nodes are configured by recompiliing the firmware or writing to EEPROM.
 *
 * @section NodeNames Node Naming
 *
 * @li Leaf node: A node at the outer edge of the network with no children.  ZigBee calls it
 * an End Device node.
 * @li Relay node: A node which has both parents and children, and relays messages from one
 * to the other.  ZigBee calls it a Router.
 * @li Base node.  The top of the tree node with no parents, only children.  Typically this node
 * will bridge to another kind of network like Ethernet.  ZigBee calls it a Co-ordinator node.
 *
 * 
 *
 *
 */

#endif // __RF24NETWORK_H__

