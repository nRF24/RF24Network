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

class RF24;

/**
 * Header which is sent with each message
 *
 * The frame put over the air consists of this header and a message
 */
struct RF24NetworkHeader
{
  uint16_t from_node; /**< Logical address where the message was generated */
  uint16_t to_node; /**< Logical address where the message is going */
  uint16_t id; /**< Sequential message ID, incremented every message */
  unsigned char type; /**< Type of the packet.  0-127 are user-defined types, 128-255 are reserved for system */
  unsigned char reserved; /**< Reserved for future use */

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
 * 2014 - Optimized Network Layer for RF24 Radios
 *
 * This class implements an OSI Network Layer using nRF24L01(+) radios driven
 * by RF24 library.
 */

class RF24Network
{
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
   * @note: Optimization: Users can optionally define an extended timeout
   * period for use in noisy or low-signal scenarios.
   *
   * Set the timeout period for individual payloads in milliseconds
   * Payloads will be retried automatically until success or timeout
   * Set to 0 to use the normal auto retry period defined by radio.setRetries()
   */

  unsigned long txTimeout;

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

/**
   * Sleep this node
   * @note NEW - Nodes can now be slept while the radio is not actively transmitting.
   * @note Setting cycles to 0 will disable the timed wakeup.
   * @note Setting the interruptPin to 255 will disable interrupt wake-ups
   *
   * This function will sleep the node only, while leaving the radio active in StandBy-I mode, which consumes
   * 22uA compared to 0.9uA in full powerDown() mode.
   *
   * The node can be awoken in two ways, both of which can be enabled simultaneously:
   * 1. An interrupt - usually triggered by the radio receiving a payload. Must use pin 2 (interrupt 0) or 3 (interrupt 1) on Uno, Nano, etc.
   * 2. The watchdog timer waking the MCU after a designated period of time.
   * @code
   * if(!network.available()){ network.sleepNode(1,0); }  //Sleeps the node for 1 second or a payload is received
   *
   * Other options:
   * network.sleepNode(0,0);         // Sleep this node forever or until a payload is received.
   * network.sleepNode(1,255);       // Sleep this node for 1 second. Do not wake up until then, even if a payload is received ( no interrupt )
   * @endcode
   * @param Cycles: The node will sleep in cycles of 1s. Using 2 will sleep 2 seconds, 3 sleeps 3s...
   * @param interruptPin: The interrupt number to use (0,1) for pins two and three on Uno,Nano. More available on Mega etc.
   *
   */
 void sleepNode( unsigned int cycles, int interruptPin );

  /**
   * This node's parent address
   *
   * @return This node's parent address, or -1 if this is the base
   */
  uint16_t parent() const;

protected:
  void open_pipes(void);
  uint16_t find_node( uint16_t current_node, uint16_t target_node );
  bool write(uint16_t);
  bool write_to_pipe( uint16_t node, uint8_t pipe );
  bool enqueue(void);

  bool is_direct_child( uint16_t node );
  bool is_descendant( uint16_t node );
  uint16_t direct_child_route_to( uint16_t node );
  uint8_t pipe_to_descendant( uint16_t node );
  void setup_address(void);

private:
  RF24& radio; /**< Underlying radio driver, provides link/physical layers */
  uint16_t node_address; /**< Logical node address of this unit, 1 .. UINT_MAX */
  const static int frame_size = 32; /**< How large is each frame over the air */
  uint8_t frame_buffer[frame_size]; /**< Space to put the frame that will be sent/received over the air */
  uint8_t frame_queue[5*frame_size]; /**< Space for a small set of frames that need to be delivered to the app layer */
  uint8_t* next_frame; /**< Pointer into the @p frame_queue where we should place the next received frame */

  uint16_t parent_node; /**< Our parent's node address */
  uint8_t parent_pipe; /**< The pipe our parent uses to listen to us */
  uint16_t node_mask; /**< The bits which contain signfificant node address information */

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
 * @example Network_Sleep_Timeouts.ino
 *
 * Example: This is almost exactly the same as the Network_Ping example, but with use  
 * of the integrated sleep mode and extended timeout periods.  
 *   
 * <br><br>
 * <b> &nbsp;&nbsp;&nbsp; SLEEP_MODE: </b>  
 * - Sleep mode is set with the command radio.sleepNode(<seconds>, <interrupt pin>);  <br>
 * - The node itself will sleep, with the radio in Standby-I mode, drawing 22uA compared to .9uA in powerdown mode.  <br>
 * - Sleep mode uses the WatchDog Timer (WDT) to sleep in 1-second intervals, or is awoken by the radio interrupt pin going  <br>
 * high, which indicates that a payload has been received. This allows much more efficient power use among the entire network.  <br>
 * - Max value is 65535 which sleeps for about 18 hours.  <br>
 * - The node power use can be reduced further by disabling unnessessary systems via the Power Reduction Register(s) (PRR) and by putting<br>
 * the radio into full sleep mode ( radio.powerDown() ), but it will not receive or send until powered up again.<br>
 * <br>
 * <b> EXTENDED TIMEOUTS: </b><br>
 * - This sketch utilizes newly introduced extended timeout periods to ensure reliability in transmission of payloads.<br>
 * - Users should be careful not to set too high a value on nodes that will receive data regularly, since payloads may be missed while<br>
 * retrying failed payloads. The maximum retry period at (15,15) = 16 time periods * 250us retry delay * 15 retries = 60ms per payload.<br>
 * - Setting this value lower than the calculated retry period will have no effect.<br>
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
 * New: Enhance the current functionality for maximum efficiency, reliability, and speed
 *
 * Xbees are excellent little radios, backed up by a mature and robust standard
 * protocol stack.  They are also expensive.
 *
 * For many Arduino uses, they seem like overkill.  So I am working to build
 * an alternative using nRF24L01 radios.  Modules are available for less than
 * $6 from many sources.  With the RF24Network layer, I hope to cover many
 * common communication scenarios.
 *
 * Please see the @ref Zigbee page for a comparison against the ZigBee protocols
 *
 * @section Features Features
 *
 * The layer provides:
 * @li <b>New</b> (2014): Power efficient listening. Nodes can now sleep for extended periods of time with minimal power usage:
 * 				   StandBy-I mode uses 22uA compared to 0.9uA in full power down mode. The Arduino is allowed to sleep,
 *				   and is awoken via interrupt when payloads are received, or via a user defined time period. See the docs.
 * @li <b>New</b> (2014): Extended timeouts. The maximum timeout period is approximately 60ms per payload with max delay between retries, and
 * 				   max retries set. Ths new txTimeout variable allows fully automated extended timeout periods via auto-retry/auto-reUse of payloads. 
 * @li <b>New</b> (2014): Optimization to the core library provides improvements to reliability, speed and efficiency. See https://github.com/TMRh20/RF24 for more info.
 * @li Host Addressing.  Each node has a logical address on the local network.
 * @li Message Forwarding.  Messages can be sent from one node to any other, and
 * this layer will get them there no matter how many hops it takes.
 * @li Ad-hoc Joining.  A node can join a network without any changes to any
 * existing nodes.
 *
 * The layer does not (yet) provide:
 * @li Fragmentation/reassembly.  Ability to send longer messages and put them
 * all back together before exposing them up to the app.
 * @li Dynamic address assignment.
 *
 * @section More How to learn more
 *
 * @li <a href="http://tmrh20.github.com/RF24/">RF24: Underlying radio driver</a>
 * @li <a href="classRF24Network.html">RF24Network Class Documentation</a>
 * @li <a href="http://tmrh20.blogspot.com/2014/03/high-speed-data-transfers-and-wireless.html">My Blog: RF24 Optimization Overview</a>
 * @li <a href="http://tmrh20.blogspot.com/2014/03/arduino-radiointercomwireless-audio.html">My Blog: RF24 Wireless Audio</a>
 * @li <a href="https://github.com/TMRh20/RF24Network/archive/master.zip">Download Current Package</a>
 * @li <a href="examples.html">Examples Page</a>.  Start with <a href="helloworld_rx_8ino-example.html">helloworld_rx</a> and <a href="helloworld_tx_8ino-example.html">helloworld_tx</a>.
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
 * @li The largest node address is 05555, so 3,125 nodes are allowed on a single channel.
 * An example topology is shown below, with 5 nodes in direct communication with the master node,
 * and multiple leaf nodes spread out at a distance, using intermediate nodes to reach other nodes.
 * 
 *|   |    | 00 |    |    | 00 |    |    |    | Master Node (00)                                    |
 *|---|----|----|----|----|----|----|----|----|-----------------------------------------------------|
 *|   |    | 01 |    |    | 04 |    |    |    | 1st level children of master (00)                   |
 *|   | 011|    |021 |    |    |014 |    |    | 2nd level children of master. Children of 1st level.|
 *|111|    |    |    |121 |    |    | 114|    | 3rd level children of master. Children of 2nd level.|
 *|   |    |    |    |1114|    |1114|2114|3114| 4th level children of master. Children of 3rd level.|
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
 * You may choose to sleep any nodes which do not have any active children on the network
 * (i.e. leaf nodes).  This is useful in a case where
 * the leaf nodes are operating on batteries and need to sleep.
 * This is useful for a sensor network.  The leaf nodes can sleep most of the time, and wake
 * every few minutes to send in a reading.  However, messages cannot be sent to these
 * sleeping nodes.
 *
 * In the future, I plan to write a system where messages can still be passed upward from
 * the base, and get delivered when a sleeping node is ready to receive them.  The radio
 * and underlying driver support 'ack payloads', which will be a handy mechanism for this.
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
 * for under $6.  My personal favorite is
 * <a href="http://www.mdfly.com/index.php?main_page=product_info&products_id=82">MDFly RF-IS2401</a>.
 *
 * @section Contrast Similiarities & Differences
 *
 * Here are some comparisons between RF24Network and ZigBee.
 *
 * @li Both networks support Star and Tree topologies.  Only Zigbee supports a true mesh.
 * @li In both networks, only leaf nodes can sleep (see @ref NodeNames).
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
 */

#endif // __RF24NETWORK_H__

