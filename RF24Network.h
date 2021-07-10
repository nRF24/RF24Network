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

#if (defined(__linux) || defined(linux)) && !defined(__ARDUINO_X86__)
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>
#include <stddef.h>
#include <assert.h>
#include <map>
#include <utility> // std::pair
#include <queue>

//ATXMega
#elif defined(XMEGA_D3)
#include "../../rf24lib/rf24lib/RF24.h"
#endif

/* Header types range */
#define MIN_USER_DEFINED_HEADER_TYPE 0
#define MAX_USER_DEFINED_HEADER_TYPE 127

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
 * **Arduino/AVR/Etc:** Data transmitted with the type set to @ref EXTERNAL_DATA_TYPE will not be loaded into the user cache. <br>
 * External systems can extract external data using the following process, while internal data types are cached in the user buffer, and accessed using network.read() :
 * @code
 * uint8_t return_type = network.update();
 * if(return_type == EXTERNAL_DATA_TYPE){
 *     uint16_t size = network.frag_ptr->message_size;
 *     memcpy(&myDataBuffer, network.frag_ptr->message_buffer, network.frag_ptr->message_size);
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
 * Any (non-master) node receiving a message of this type will manually forward it to the master node using a normal network write.
 */
#define NETWORK_REQ_ADDRESS 195
//#define NETWORK_ADDR_LOOKUP 196
//#define NETWORK_ADDR_RELEASE 197
/** @} */

#define NETWORK_MORE_FRAGMENTS_NACK 200

/** Internal defines for handling written payloads */
#define TX_NORMAL 0
#define TX_ROUTED 1
#define USER_TX_TO_PHYSICAL_ADDRESS 2 // no network ACK
#define USER_TX_TO_LOGICAL_ADDRESS 3  // network ACK
#define USER_TX_MULTICAST 4

#define MAX_FRAME_SIZE 32    // Size of individual radio frames
#define FRAME_HEADER_SIZE 10 // Size of RF24Network frames - data

#define USE_CURRENT_CHANNEL 255 // Use current radio channel when setting up the network

/** Internal defines for handling internal payloads - prevents reading additional data from the radio
 * when buffers are full
 */
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
    uint16_t from_node; /** Logical address where the message was generated */
    uint16_t to_node;   /** Logical address where the message is going */
    uint16_t id;        /** Sequential message ID, incremented every time a new frame is constructed */
    /**
     * Message Types:
     * User message types 1 through 64 will NOT be acknowledged by the network, while message types 65 through 127 will receive a network ACK.
     * System message types 192 through 255 will NOT be acknowledged by the network. Message types 128 through 192 will receive a network ACK. <br>
     * <br><br>
     */
    unsigned char type; /** <b>Type of the packet.</b> 0-127 are user-defined types, 128-255 are reserved for system */

    /**
     * During fragmentation, it carries the fragment_id, and on the last fragment
     * it carries the header_type.<br>
     */
    unsigned char reserved; /** *Reserved for system use* */

    static uint16_t next_id; /** The message ID of the next message to be sent (unused)*/

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
     * uint16_t recipient_address = 011;
     *
     * RF24NetworkHeader header(recipient_address, 't');
     *
     * network.write(header, &message, sizeof(message));
     * @endcode
     *
     * @param _to The Octal format, logical node address where the message is going
     * @param _type The type of message which follows.  Only 0-127 are allowed for
     * user messages. Types 1-64 will not receive a network acknowledgement.
     */
    RF24NetworkHeader(uint16_t _to, unsigned char _type = 0) : to_node(_to), id(next_id++), type(_type) {}

    /**
     * Create debugging string
     *
     * Useful for debugging. Dumps all members into a single string, using
     * internal static memory. This memory will get overridden next time
     * you call the method.
     *
     * @return String representation of this object
     */
    const char *toString(void) const;
};

/**
 * Frame structure for internal message handling, and for use by external applications
 *
 * The actual frame put over the air consists of a header (8-bytes) and a message payload (Up to 24-bytes)<br>
 * When data is received, it is stored using the RF24NetworkFrame structure, which includes:
 * 1. The header containing information about routing the message and the message type
 * 2. The size of the included message
 * 3. The 'message' or data being received
 *
 */
struct RF24NetworkFrame
{
    /** Header which is sent with each message */
    RF24NetworkHeader header;
    /** The size in bytes of the payload length */
    uint16_t message_size;

    /**
     * On Arduino, the message buffer is just a pointer, and can be pointed to any memory location.
     * On Linux the message buffer is a standard byte array, equal in size to the defined MAX_PAYLOAD_SIZE
     */
    #if defined(RF24_LINUX)
    uint8_t message_buffer[MAX_PAYLOAD_SIZE]; //< Array to store the message
    #else
    uint8_t *message_buffer; //< Pointer to the buffer storing the actual message
    #endif

    /**
     * Default constructor
     *
     * Simply constructs a blank frame. Frames are generally used internally. See RF24NetworkHeader.
     */
    RF24NetworkFrame() {}

    /**
     * **Constructor for Linux platforms** - create a network frame with data
     * Frames are constructed and handled differently on Arduino/AVR and Linux devices (`#if defined RF24_LINUX`)
     *
     * @param _header The RF24Network header to be stored in the frame
     * @param _message The 'message' or data.
     * @param _len The size of the 'message' or data.
     *
     * Frames are used internally and by external systems. See RF24NetworkHeader.
     */
    #if defined(RF24_LINUX) || defined(DOXYGEN_FORCED)
    RF24NetworkFrame(RF24NetworkHeader &_header, const void *_message = NULL, uint16_t _len = 0) : header(_header), message_size(_len)
    {
        if (_message && _len) {
            memcpy(message_buffer, _message, _len);
        }
    }
    #endif
    #if defined(DOXYGEN_FORCED) || !defined(RF24_LINUX)
    /**
     * **Constructor for Arduino/AVR/etc. platforms** - create a network frame with data
     * Frames are constructed and handled differently on Arduino/AVR and Linux devices (`#if defined RF24_LINUX`)
     *
     * @see RF24Network.frag_ptr
     * @param _header The RF24Network header to be stored in the frame
     * @param _message_size The size of the 'message' or data
     *
     * Frames are used internally and by external systems. See RF24NetworkHeader.
     */
    RF24NetworkFrame(RF24NetworkHeader &_header, uint16_t _message_size) : header(_header), message_size(_message_size)
    {
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
    const char *toString(void) const;
};

/**
 * 2014-2020 - Optimized Network Layer for RF24 Radios
 *
 * This class implements an OSI Network Layer using nRF24L01(+) radios driven
 * by RF24 library.
 */
class RF24Network
{

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
     */
    RF24Network(RF24 &_radio);

    /**
     * Bring up the network using the current radio frequency/channel.
     * Calling begin brings up the network, and configures the address, which designates the location of the node within RF24Network topology.
     * @note Node addresses are specified in Octal format, see [RF24Network Addressing](md_docs_addressing.html) for more information.
     * @warning Be sure to first call `RF24::begin()` to initialize the radio properly.
     * @note The address `04444` is resreved for RF24Mesh usage
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
     * @see begin(uint8_t _channel, uint16_t _node_address)
     * @param _node_address The logical address of this node
     */
    inline void begin(uint16_t _node_address)
    {
        begin(USE_CURRENT_CHANNEL, _node_address);
    }

    /**
     * Main layer loop
     *
     * This function must be called regularly to keep the layer going.  This is where payloads are
     * re-routed, received, and all the action happens.
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
    uint16_t peek(RF24NetworkHeader &header);

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
    void peek(RF24NetworkHeader &header, void *message, uint16_t maxlen);

    /**
     * Read a message
     *
     * @code
     * while (network.available()) {
     *   RF24NetworkHeader header;
     *   uint32_t time;
     *   network.peek(header);
     *   if (header.type == 'T') {
     *     network.read(header, &time, sizeof(time));
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
    uint16_t read(RF24NetworkHeader &header, void *message, uint16_t maxlen);

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
     * network.write(header, &time, sizeof(time));
     * @endcode
     * @param[in,out] header The header (envelope) of this message.  The critical
     * thing to fill in is the @p to_node field so we know where to send the
     * message.  It is then updated with the details of the actual header sent.
     * @param message Pointer to memory where the message is located
     * @param len The size of the message
     * @return Whether the message was successfully received
     */
    bool write(RF24NetworkHeader &header, const void *message, uint16_t len);

    /**@}*/
    /**
     * @name Advanced Configuration
     *
     * For advanced configuration of the network
     */
    /**@{*/

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
     * @note This value is automatically assigned based on the node address
     * to reduce errors and increase throughput of the network.
     *
     * Sets the timeout period for individual payloads in milliseconds at staggered intervals.
     * Payloads will be retried automatically until success or timeout
     * Set to 0 to use the normal auto retry period defined by radio.setRetries()
     *
     */
    uint32_t txTimeout; /** Network timeout value */

    /**
     * This only affects payloads that are routed by one or more nodes.
     * This specifies how long to wait for an ack from across the network.
     * Radios sending directly to their parent or children nodes do not
     * utilize this value.
     */
    uint16_t routeTimeout; /** Timeout for routed payloads */

    /**@}*/
    /**
     * @name Advanced Operation
     *
     * For advanced operation of the network
     */
    /**@{*/

    /**
     * Return the number of failures and successes for all transmitted payloads, routed or sent directly
     * @note This needs to be enabled via `#define ENABLE_NETWORK_STATS` in RF24Network_config.h
     *
     * @code
     * bool fails, success;
     * network.failures(&fails, &success);
     * @endcode
     */
    void failures(uint32_t *_fails, uint32_t *_ok);

    #if defined(RF24NetworkMulticast)

    /**
     * Send a multicast message to multiple nodes at once
     * Allows messages to be rapidly broadcast through the network
     *
     * Multicasting is arranged in levels, with all nodes on the same level listening to the same address
     * Levels are assigned by network level ie: nodes 01-05: Level 1, nodes 011-055: Level 2
     * @see multicastLevel
     * @see multicastRelay
     * @param header reference to the RF24NetworkHeader object used for this @p message
     * @param message Pointer to memory where the message is located
     * @param len The size of the message
     * @param level Multicast level to broadcast to
     * @return Whether the message was successfully sent
     */
    bool multicast(RF24NetworkHeader &header, const void *message, uint16_t len, uint8_t level);

    #endif

    /**
     * Writes a direct (unicast) payload. This allows routing or sending messages outside of the usual routing paths.
     * The same as write, but a physical address is specified as the last option.
     * The payload will be written to the physical address, and routed as necessary by the recipient
     */
    bool write(RF24NetworkHeader &header, const void *message, uint16_t len, uint16_t writeDirect);

    /**
     * Sleep this node - For AVR devices only
     * @note NEW - Nodes can now be slept while the radio is not actively transmitting. This must be manually enabled by uncommenting
     * the `#define ENABLE_SLEEP_MODE` in RF24Network_config.h
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
     * @param INTERRUPT_MODE an identifying number to indicate what type of state for which the @p interrupt_pin will be used to wake up the radio.
     * | @p INTERRUPT_MODE | type of state |
     * |:-----------------:|:-------------:|
     * | 0 | LOW      |
     * | 1 | RISING   |
     * | 2 | FALLING  |
     * | 3 | CHANGE   |
     * @return True if sleepNode completed normally, after the specified number of cycles. False if sleep was interrupted
     */
    bool sleepNode(unsigned int cycles, int interruptPin, uint8_t INTERRUPT_MODE = 0); //added interrupt mode support (default 0=LOW)

    /**
     * This node's parent address
     *
     * @return This node's parent address, or -1 if this is the base
     */
    uint16_t parent() const;

    /**
     * Provided a node address and a pipe number, will return the RF24Network address of that child pipe for that node
     */
    uint16_t addressOfPipe(uint16_t node, uint8_t pipeNo);

    /**
     * @note Addresses are specified in octal: 011, 034
     * @return True if a supplied address is valid
     */
    bool is_valid_address(uint16_t node);

    /**@}*/
    /**
     * @name Deprecated
     *
     * Maintained for backwards compatibility
     */
    /**@{*/

    /**
     * Bring up the network on a specific radio frequency/channel.
     * @deprecated Use radio.setChannel() to configure the radio channel.
     * Use RF24Network::begin(uint16_t _node_address) to set the node address.
     *
     * **Example 1:** Begin on channel 90 with address 0 (master node)
     * @code
     * network.begin(90, 0);
     * @endcode
     * **Example 2:** Begin on channel 90 with address 01 (child of master)
     * @code
     * network.begin(90, 01);
     * @endcode
     * **Example 3:** Begin on channel 90 with address 011 (child of 01, grandchild of master)
     * @code
     * network.begin(90, 011);
     * @endcode
     *
     * @param _channel The RF channel to operate on
     * @param _node_address The logical address of this node
     */
    void begin(uint8_t _channel, uint16_t _node_address);

    /**@}*/
    /**
     * @name External Applications/Systems
     *
     * Interface for External Applications and Systems ( RF24Mesh, RF24Ethernet )
     */
    /**@{*/

    /** The raw system frame buffer of received data. */
    uint8_t frame_buffer[MAX_FRAME_SIZE];

    /**
     * **Linux platforms only**
     *
     * Data with a header type of @ref EXTERNAL_DATA_TYPE will be loaded into a separate queue.
     * The data can be accessed as follows:
     * @code
     * RF24NetworkFrame f;
     * while(network.external_queue.size() > 0) {
     *   f = network.external_queue.front();
     *   uint16_t dataSize = f.message_size;
     *
     *   // read the frame message buffer
     *   memcpy(&myBuffer, &f.message_buffer, dataSize);
     *   network.external_queue.pop();
     * }
     * @endcode
     */
    #if defined(RF24_LINUX) || defined(DOXYGEN_FORCED)
    std::queue<RF24NetworkFrame> external_queue;
    #endif

    #if (!defined(DISABLE_FRAGMENTATION) && !defined(RF24_LINUX)) || defined(DOXYGEN_FORCED)
    /**
     * **ARDUINO platforms only**
     *
     * The `frag_ptr` is only used with Arduino (not RPi/Linux) and is mainly used for external data systems like RF24Ethernet. When
     * a payload of type @ref EXTERNAL_DATA_TYPE is received, and returned from update(), the `frag_ptr` will always point to the starting
     * memory location of the received frame. <br>This is used by external data systems (RF24Ethernet) to immediately copy the received
     * data to a buffer, without using the user-cache.
     *
     * @see RF24NetworkFrame
     *
     * @code
     * uint8_t return_type = network.update();
     * if(return_type == EXTERNAL_DATA_TYPE) {
     *     memcpy(&myDataBuffer, network.frag_ptr->message_buffer, network.frag_ptr->message_size);
     * }
     * @endcode
     * Linux devices (defined as `RF24_LINUX`) currently cache all payload types, and do not utilize `frag_ptr`.
     */
    RF24NetworkFrame *frag_ptr;
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
     * | @ref NETWORK_ADDR_RESPONSE |
     * | @ref NETWORK_ACK           |
     * | @ref NETWORK_PING          |
     * | @ref NETWORK_POLL <br>(With multicast enabled) |
     * | @ref NETWORK_REQ_ADDRESS   |
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
     * | @ref FLAG_HOLD_INCOMING| 1 (bit 0 asserted) | INTERNAL: Set automatically when a fragmented payload will exceed the available cache |
     * | @ref FLAG_BYPASS_HOLDS| 2 (bit 1 asserted) | EXTERNAL: Can be used to prevent holds from blocking. Note: Holds are disabled & re-enabled by RF24Mesh when renewing addresses. This will cause data loss if incoming data exceeds the available cache space|
     * | @ref FLAG_FAST_FRAG| 4 (bit 2 asserted) | INTERNAL: Replaces the fastFragTransfer variable, and allows for faster transfers between directly connected nodes. |
     * | @ref FLAG_NO_POLL| 8 (bit 3 asserted) | EXTERNAL/USER: Disables @ref NETWORK_POLL responses on a node-by-node basis. |
     *
     */
    uint8_t networkFlags;

private:
    bool write(uint16_t, uint8_t sendType);
    bool write_to_pipe(uint16_t node, uint8_t pipe, bool multicast);
    uint8_t enqueue(RF24NetworkHeader *header);

    bool is_direct_child(uint16_t node);
    bool is_descendant(uint16_t node);

    uint16_t direct_child_route_to(uint16_t node);
    void setup_address(void);
    bool _write(RF24NetworkHeader &header, const void *message, uint16_t len, uint16_t writeDirect);

    struct logicalToPhysicalStruct
    {
        uint16_t send_node;
        uint8_t send_pipe;
        bool multicast;
    };

    void logicalToPhysicalAddress(logicalToPhysicalStruct *conversionInfo);

    RF24 &radio; /** Underlying radio driver, provides link/physical layers */

    #if defined(RF24NetworkMulticast)
    uint8_t multicast_level;
    #endif
    uint16_t node_address; /** Logical node address of this unit, 1 .. UINT_MAX */
    uint8_t frame_size;
    const static unsigned int max_frame_payload_size = MAX_FRAME_SIZE - sizeof(RF24NetworkHeader);

    #if defined(RF24_LINUX)
    std::queue<RF24NetworkFrame> frame_queue;
    std::map<uint16_t, RF24NetworkFrame> frameFragmentsCache;
    bool appendFragmentToFrame(RF24NetworkFrame frame);
    #else // Not Linux:

    #if defined(DISABLE_USER_PAYLOADS)
    uint8_t frame_queue[1]; /** Space for a small set of frames that need to be delivered to the app layer */
    #else
    uint8_t frame_queue[MAIN_BUFFER_SIZE]; /** Space for a small set of frames that need to be delivered to the app layer */
    #endif

    uint8_t *next_frame; /** Pointer into the @p frame_queue where we should place the next received frame */

    #if !defined(DISABLE_FRAGMENTATION)
    RF24NetworkFrame frag_queue;
    uint8_t frag_queue_message_buffer[MAX_PAYLOAD_SIZE]; //frame size + 1
    #endif

    #endif // Linux/Not Linux

    uint16_t parent_node; /** Our parent's node address */
    uint8_t parent_pipe;  /** The pipe our parent uses to listen to us */
    uint16_t node_mask;   /** The bits which contain signfificant node address information */
    uint64_t pipe_address(uint16_t node, uint8_t pipe);

    #if defined ENABLE_NETWORK_STATS
    uint32_t nFails;
    uint32_t nOK;
    #endif

    #if defined(RF24NetworkMulticast)
    uint16_t levelToAddress(uint8_t level);
    #endif

    /** @} */
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
 * @example helloworld_tx_advanced.ino
 *
 * A more advanced version of helloworld_tx using fragmentation/reassembly
 * and variable payload sizes
 */

/**
 * @example helloworld_rx_advanced.ino
 * A more advanced version of helloworld_rx using fragmentation/reassembly
 * and variable payload sizes
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
 * @example Network_Priority_TX.ino
 * An example of handling/prioritizing different types of data passing through the RF24Network
 */

/**
 * @example Network_Priority_RX.ino
 * An example of handling/prioritizing different types of data passing through the RF24Network
 */

#endif // __RF24NETWORK_H__
