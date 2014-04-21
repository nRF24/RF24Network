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
   * Sleep this node - Still Under Development
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
   *
   */
 void sleepNode( unsigned int cycles, int interruptPin );

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
   * This node's parent address
   *
   * @return This node's parent address, or -1 if this is the base
   */
  uint16_t parent() const;

  /**@}*/
  /**
   * @name Advanced Operation
   *
   *  For advanced configuration of the network
   */
  /**@{*/

  /**
   * Construct the network in dual head mode using two radio modules.
   * @note Radios will share MISO, MOSI and SCK pins, but require separate CE,CS pins.
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
   * @note: Optimization:This value is automatically assigned based on the node address
   * to reduce errors and increase throughput of the network.
   *
   * Sets the timeout period for individual payloads in milliseconds at staggered intervals.
   * Payloads will be retried automatically until success or timeout
   * Set to 0 to use the normal auto retry period defined by radio.setRetries()
   *
   */

  unsigned long txTimeout;

private:
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

#if defined (DUAL_HEAD_RADIO)
  RF24& radio1;
#endif
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
 * @li <b>New</b> (2014): Dual headed operation: The use of dual radios for busy routing nodes or the master node enhances throughput and decreases errors. See the <a href="Tuning.html">Tuning</a> section.
 * @li <b>New</b> (2014): Extended timeouts and staggered timeout intervals. The new txTimeout variable allows fully automated extended timeout periods via auto-retry/auto-reUse of payloads.
 * @li <b>New</b> (2014): Optimization to the core library provides improvements to reliability, speed and efficiency. See https://tmrh20.github.io/RF24 for more info.
 * @li <b>New</b> (2014): Built in sleep mode using interrupts. (Still under development. (Enable via RF24Network_config.h))
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
 * @li <a href="classRF24Network.html">RF24Network Class Documentation</a>
 * @li <a href="Tuning.html"> Performance and Data Loss: Tuning the Network</a>
 * @li <a href="http://tmrh20.blogspot.com/2014/03/high-speed-data-transfers-and-wireless.html">My Blog: RF24 Optimization Overview</a>
 * @li <a href="http://tmrh20.blogspot.com/2014/03/arduino-radiointercomwireless-audio.html">My Blog: RF24 Wireless Audio</a>
 * @li <a href="http://tmrh20.github.com/RF24/">RF24: Underlying radio driver</a>
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
 * @page Tuning Performance and Data Loss: Tuning the Network
 * Tips and examples for tuning the network and Dual-head operation.
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
 * example in the above section. The txTimeout variable is used to extend the retry count to a defined
 * duration in milliseconds. An interval/retry setting of (15,15) will provide 15 retrys at intervals of
 * 4ms, taking up to 60ms per payload.
 *
 * The library now provides staggered timout periods by default, but they can also be adjusted on a per-node
 * basis. See the network.txTimeout variable.
 * Timeout periods of extended duration (500+) will generally not help when payloads are failing due to data
 * collisions, it will only extend the duration of the errors. Extended duration timeouts should generally only
 * be configured on leaf nodes that do not receive data, or on a dual-headed node.
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
 * 		network.txTimeout(500);
 * 	 @endcode
 * b: Second leaf node configured with a similar timeout period and retry delay:
 * @code
 * 		radio.setRetries(8,15);
 * 		network.txTimeout(553);
 * @endcode
 * c: First and third leaf nodes configured with default timeout periods or slightly increased timout periods.
 *
 * @section DualHead Dual Headed Operation
 *
 * The library now supports a dual radio configuration to further enhance network performance, while reducing errors on
 * busy networks. Master nodes or relay nodes with a large number of child nodes can benefit greatly from a dual headed
 * configuration, since one radio is used for receiving, and the other entirely for transmission.
 *
 * To configure a dual headed node:
 * 1. Edit the RF24Network_config.h file, and uncomment #define DUAL_HEAD_RADIO
 * 2. Connect another radio, using the same MOSI, MISO, and SCK lines.
 * 3. Choose another two pins to use for CE and CS on the second radio. Connect them.
 * 4. Setup the radio and network like so:
 *
 * @code
 * 	RF24 radio(7,8);  			// Using CE (7) and CS (8) for first radio
 * 	RF24 radio1(4,5); 			// Using CE (4) and CS (5) for second radio
 * 	RF24Network network(radio,radio1);	// Set up the network using both radios
 *
 *  Then in setup(), call radio.begin(); and radio1.begin(); before network.begin();
 * @endcode
 *
 * 5. Upload to MCU. The node will now use the first radio to receive data, and radio1 to transmit, preventing data loss on a busy network.
 * 6. Re-comment the #define in the config file as required if configuring other single-headed radios.
 *
 *
 * Any node can be configured in dual-head mode.
 *
 *
 */

#endif // __RF24NETWORK_H__

