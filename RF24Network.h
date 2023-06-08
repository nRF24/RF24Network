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
 * The network will determine whether to automatically acknowledge payloads based on their general
 * `RF24NetworkHeader::type`.
 *
 * - **User types** (1 - 127) 1 - 64 will NOT be acknowledged
 * - **System types** (128 - 255) 192 - 255 will NOT be acknowledged
 *
 * System types can also contain message data.
 *
 * @defgroup DEFINED_TYPES Reserved System Message Types
 *
 * @{
 */

/**
 * A @ref NETWORK_ADDR_RESPONSE type is utilized to manually route custom messages containing a
 * single RF24Network address.
 *
 * Used by RF24Mesh
 *
 * If a node receives a message of this type that is directly addressed to it, it will read the
 * included message, and forward the payload on to the proper recipient.
 *
 * This allows nodes to forward multicast messages to the master node, receive a response, and
 * forward it back to the requester.
 */
#define NETWORK_ADDR_RESPONSE 128

/**
 * Messages of type NETWORK_PING will be dropped automatically by the recipient.
 * A NETWORK_ACK or automatic radio-ack will indicate to the sender whether the payload was
 * successful. The time it takes to successfully send a NETWORK_PING is the round-trip-time.
 */
#define NETWORK_PING 130

/**
 * External data types are used to define messages that will be passed to an external data system.
 * This allows RF24Network to route and pass any type of data, such as TCP/IP frames, while still
 * being able to utilize standard RF24Network messages etc.
 *
 * - **Linux**
 *
 *   Linux devices (defined with `RF24_LINUX` macro) will buffer all data types in the user cache.
 *
 * - **Arduino/AVR/Etc**
 *
 *   Data transmitted with the type set to EXTERNAL_DATA_TYPE will not be loaded into the user cache.
 *
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
 * Messages of this type do not receive a @ref NETWORK_ACK
 */
#define NETWORK_LAST_FRAGMENT 150
//#define NETWORK_LAST_FRAGMENT 201

// NO ACK Response Types
//#define NETWORK_ACK_REQUEST 192

/**
 * Messages of this type signal the sender that a network-wide transmission has been completed.
 *
 * - **Not fool-proof**
 * @par
 * RF24Network does not directly have a built-in transport layer protocol, so message delivery is
 * not 100% guaranteed. Messages can be lost via corrupted dynamic payloads, or a NETWORK_ACK can
 * fail (despite successful transmission of the message).
 *
 * - **Traffic analysis**
 * @par
 * NETWORK_ACK messages can be utilized as a traffic/flow control mechanism. Transmitting nodes
 * that emit NETWORK_ACK qualifying messages will be forced to wait, before sending additional
 * data, until the payload is transmitted across the network and acknowledged.
 *
 * - **Different from Radio ACK Packets**
 * @par
 * In the event that the transmitting device will be sending directly to a parent or child node,
 * a NETWORK_ACK is not required. This is because the radio's auto-ack feature is utilized for
 * connections between directly related network nodes. For example: nodes `01` and `011` use the
 * radio's auto-ack feature for transmissions between them, but nodes `01` and `02` do not use
 * the radio's auto-ack feature for transmissions between them as messages will be routed through
 * other nodes.
 * @par
 * Multicasted messages do use the radio's auto-ack feature because of the hardware limitations of
 * nRF24L01 transceivers. This applies to all multicasted messages (directly related nodes or
 * otherwise).
 *
 * @remark
 * Remember, user messages types with a decimal value of 64 or less will not be acknowledged across
 * the network via NETWORK_ACK messages.
 *
 * @note NETWORK_ACK messages are only sent by the last node in the route to a target node.
 * ie: When node `00` sends an instigating message to node `011`, node `01` will send the
 * NETWORK_ACK message to `00` upon successful delivery of instigating message to node `011`.
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

// The following 2 atrifacts now exist in RF24Mesh as their significance was specific to RF24Mesh.
//#define NETWORK_ADDR_LOOKUP 196
//#define NETWORK_ADDR_RELEASE 197
/** @} */

/* This isn't actually used anywhere. */
#define NETWORK_MORE_FRAGMENTS_NACK 200

/* Internal defines for handling written payloads */
#define TX_NORMAL                   0
#define TX_ROUTED                   1
#define USER_TX_TO_PHYSICAL_ADDRESS 2 // no network ACK
#define USER_TX_TO_LOGICAL_ADDRESS  3 // network ACK
#define USER_TX_MULTICAST           4

#define MAX_FRAME_SIZE    32 // Size of individual radio frames
#define FRAME_HEADER_SIZE 10 // Size of RF24Network frames - data

/**
 * A sentinel value signifying that the current radio channel should be unchanged when setting
 * up the network node with RF24Network::begin(uint8_t _channel, uint16_t _node_address).
 */
#define USE_CURRENT_CHANNEL 255

/**
 * This flag (when asserted in RF24Network::networkFlags) prevents repetitively configuring the
 * radio during transmission of fragmented messages.
 */
#define FLAG_FAST_FRAG 4
/**
 * This flag (when asserted in RF24Network::networkFlags) prevents a node from responding to
 * mesh nodes looking to connect to the network. Calling RF24Mesh::setChild() uses this flag
 * accordingly.
 */
#define FLAG_NO_POLL 8

class RF24;
#if defined(ARDUINO_ARCH_NRF52) || defined(ARDUINO_ARCH_NRF52840) || defined(ARDUINO_ARCH_NRF52833)
class nrf_to_nrf;
#endif

/**
 * Header which is sent with each message
 *
 * The frame put over the air consists of this header and a message
 *
 * Headers are addressed to the appropriate node, and the network forwards them on to their final destination.
 */
struct RF24NetworkHeader
{
    /** Logical address where the message was generated */
    uint16_t from_node;

    /** Logical address where the message is going */
    uint16_t to_node;

    /** Sequential message ID, incremented every time a new frame is constructed */
    uint16_t id;

    /**
     * **Type of the packet.** 0 - 127 are user-defined types, 128 - 255 are reserved for system.
     *
     * User message types 1 through 64 will NOT be acknowledged by the network, while message types 65 through 127 will receive a network ACK.
     * System message types 192 through 255 will NOT be acknowledged by the network. Message types 128 through 192 will receive a network ACK.
     * @see @ref DEFINED_TYPES
     */
    unsigned char type;

    /**
     * **Reserved for system use**
     *
     * During fragmentation, it carries the fragment_id, and on the last fragment
     * it carries the header_type.
     */
    unsigned char reserved;

    /** The message ID of the next message to be sent. This attribute is not sent with the header. */
    static uint16_t next_id;

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
     * Fragmentation is enabled by default for all devices except ATTiny.
     * Configure fragmentation and max payload size in RF24Network_config.h
     *
     * Use this constructor to create a header and then send a message
     * @code
     * uint16_t recipient_address = 011;
     *
     * RF24NetworkHeader header(recipient_address, 't');
     *
     * network.write(header, &message, sizeof(message));
     * @endcode
     *
     * @param _to The Octal format, logical node address where the message is going
     * @param _type The type of message which follows.  Only 0 - 127 are allowed for
     * user messages. Types 1 - 64 will not receive a network acknowledgement.
     */
    RF24NetworkHeader(uint16_t _to, unsigned char _type = 0) : to_node(_to), id(next_id++), type(_type) {}

    /**
     * Create debugging string
     *
     * Useful for debugging. Dumps all members into a single string, using
     * internal static memory. This memory will get overridden next time
     * you call the method.
     *
     * @return String representation of the object's significant members.
     */
    const char* toString(void) const;
};

/**
 * Frame structure for internal message handling, and for use by external applications
 *
 * The actual frame put over the air consists of a header (8-bytes) and a message payload (Up to 24-bytes).
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
    uint8_t message_buffer[MAX_PAYLOAD_SIZE]; // Array to store the message
#else
    uint8_t* message_buffer; // Pointer to the buffer storing the actual message
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
    RF24NetworkFrame(RF24NetworkHeader& _header, const void* _message = NULL, uint16_t _len = 0) : header(_header), message_size(_len)
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
     * @see ESBNetwork.frag_ptr
     * @param _header The RF24Network header to be stored in the frame
     * @param _message_size The size of the 'message' or data
     *
     * Frames are used internally and by external systems. See RF24NetworkHeader.
     */
    RF24NetworkFrame(RF24NetworkHeader& _header, uint16_t _message_size) : header(_header), message_size(_message_size)
    {
    }
#endif
};

/**
 * 2014-2021 - Optimized Network Layer for RF24 Radios
 *
 * This class implements an OSI Network Layer using nRF24L01(+) radios driven
 * by RF24 library.
 *
 * @tparam radio_t The `radio` object's type. Defaults to `RF24` for legacy behavior.
 * This new abstraction is really meant for using the nRF52840 SoC as a drop-in replacement
 * for the nRF24L01 radio. For more detail, see the
 * [nrf_to_nrf Arduino library](https://github.com/TMRh20/nrf_to_nrf).
 */
template<class radio_t = RF24>
class ESBNetwork
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
     * v2.0 supports a backward compatible constructor:
     * @code
     * RF24 radio(7, 8);
     * RF24Network network(radio); // for nRF24L01
     *
     * nrf_to_nrf radio1;
     * RF52Network network(radio1); // for nRF52xxx family
     * @endcode
     * 
     * @see v2.0 supports [nrf_to_nrf Arduino library](https://github.com/TMRh20/nrf_to_nrf)
     * for nrf52 chips' internal radio.
     * 
     * @param _radio The underlying radio driver instance
     */
    ESBNetwork(radio_t& _radio);

    /**
     * Bring up the network using the current radio frequency/channel.
     * Calling begin brings up the network, and configures the address, which designates the
     * location of the node within [RF24Network topology](md_docs_tuning.html).
     *
     * @note Node addresses are specified in Octal format, see
     * [RF24Network Addressing](md_docs_addressing.html) for more information. The address `04444`
     * is reserved for RF24Mesh usage (when a mesh node is connecting to the network).
     * @warning Be sure to first call `RF24::begin()` to initialize the radio properly.
     *
     * **Example 1:** Begin on current radio channel with address 0 (master node)
     * @code network.begin(00); @endcode
     * **Example 2:** Begin with address 01 (child of master)
     * @code network.begin(01); @endcode
     * **Example 3:** Begin with address 011 (child of 01, grandchild of master)
     * @code network.begin(011); @endcode
     *
     * @see begin(uint8_t _channel, uint16_t _node_address)
     * @param _node_address The logical address of this node.
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
     * @return Returns the @ref RF24NetworkHeader::type of the last received payload.
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
     * incoming message.  Useful for doing a switch on the message type.
     *
     * @param[out] header The RF24NetworkHeader (envelope) of the next message.
     * If there is no message available, the referenced `header` object is not touched
     * @return The length of the next available message in the queue.
     */
    uint16_t peek(RF24NetworkHeader& header);

    /**
     * Read the next available payload
     *
     * Reads the next available payload without advancing to the next
     * incoming message.  Useful for doing a transparent packet
     * manipulation layer on top of RF24Network.
     *
     * @param[out] header The RF24NetworkHeader (envelope) of this message
     * @param[out] message Pointer to memory where the message should be placed
     * @param maxlen Amount of bytes to copy to @p message .
     * If this parameter is left unspecified, the entire length of the message is fetched.
     * Hint: Use peek(RF24NetworkHeader) to get the length of next available message in the queue.
     */
    void peek(RF24NetworkHeader& header, void* message, uint16_t maxlen = MAX_PAYLOAD_SIZE);

    /**
     * Read a message
     * @note This function assumes there is a frame in the queue.
     * @code
     * while (network.available()) {
     *   RF24NetworkHeader header;
     *   uint32_t time;
     *   uint16_t msg_size = network.peek(header);
     *   if (header.type == 'T') {
     *     network.read(header, &time, sizeof(time));
     *     Serial.print("Got time: ");
     *     Serial.println(time);
     *   }
     * }
     * @endcode
     * @param[out] header The RF24NetworkHeader (envelope) of this message
     * @param[out] message Pointer to memory where the message should be placed
     * @param maxlen The largest message size which can be held in @p message .
     * If this parameter is left unspecified, the entire length of the message is fetched.
     * Hint: Use peek(RF24NetworkHeader &) to get the length of next available message in the queue.
     * @return The total number of bytes copied into @p message
     */
    uint16_t read(RF24NetworkHeader& header, void* message, uint16_t maxlen = MAX_PAYLOAD_SIZE);

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
    bool write(RF24NetworkHeader& header, const void* message, uint16_t len);

    /**@}*/
    /**
     * @name Advanced Configuration
     *
     * For advanced configuration of the network
     */
    /**@{*/

    /**
     * By default, multicast addresses are divided into 5 network levels:
     * - The master node is the only node on level 0 (the lowest level)
     * - Nodes 01-05 (level 1) share a multicast address
     * - Nodes 0n1-0n5 (level 2) share a multicast address
     * - Nodes 0n11-0n55 (level 3) share a multicast address
     * - Nodes 0n111-0n555 (level 4) share a multicast address
     *
     * Notice "n" (used in the list above) stands for an octal digit in range [0, 5]
     *
     * This optional function is used to override the default level set when a node's logical
     * address changes, and it can be used to create custom multicast groups that all share a
     * single address.
     * @see
     * - multicastRelay
     * - multicast()
     * - [The topology image](http://github.com/nRF24/RF24Network/blob/master/images/topologyImage.jpg)
     * @param level Levels 0 to 4 are available. All nodes at the same level will receive the same
     * messages if in range. Messages will be routed in order of level, low to high, by default.
     */
    void multicastLevel(uint8_t level);

    /**
     * Enabling this will allow this node to automatically forward received multicast frames to the next highest
     * multicast level. Forwarded frames will also be enqueued on the forwarding node as a received frame.
     *
     * This is disabled by default.
     * @see multicastLevel()
     */
    bool multicastRelay;

    /**
     * Set up the watchdog timer for sleep mode using the number 0 through 10 to represent the following time periods:<br>
     * wdt_16ms = 0, wdt_32ms, wdt_64ms, wdt_128ms, wdt_250ms, wdt_500ms, wdt_1s, wdt_2s, wdt_4s, wdt_8s
     * @code
     * setup_watchdog(7);   // Sets the WDT to trigger every second
     * @endcode
     * @param prescalar The WDT prescaler to define how often the node will wake up. When defining sleep mode cycles, this time period is 1 cycle.
     */
    void setup_watchdog(uint8_t prescalar);

    /**
     * @brief Network timeout value
     * @note This value is automatically assigned based on the node address
     * to reduce errors and increase throughput of the network.
     *
     * Sets the timeout period for individual payloads in milliseconds at staggered intervals.
     * Payloads will be retried automatically until success or timeout.
     * Set to 0 to use the normal auto retry period defined by `radio.setRetries()`.
     */
    uint32_t txTimeout;

    /**
     * @brief Timeout for routed payloads
     *
     * This only affects payloads that are routed through one or more nodes.
     * This specifies how long to wait for an ack from across the network.
     * Radios sending directly to their parent or children nodes do not
     * utilize this value.
     */
    uint16_t routeTimeout;

    /**@}*/
    /**
     * @name Advanced Operation
     *
     * For advanced operation of the network
     */
    /**@{*/

#if defined(ENABLE_NETWORK_STATS) || defined(DOXYGEN_FORCED)

    /**
     * Return the number of failures and successes for all transmitted payloads, routed or sent directly
     * @note This needs to be enabled via `#define ENABLE_NETWORK_STATS` in RF24Network_config.h
     *
     * @code
     * bool fails, success;
     * network.failures(&fails, &success);
     * @endcode
     */
    void failures(uint32_t* _fails, uint32_t* _ok);

#endif // defined (ENABLE_NETWORK_STATS)
#if defined(RF24NetworkMulticast)

    /**
     * Send a multicast message to multiple nodes at once
     * Allows messages to be rapidly broadcast through the network
     *
     * Multicasting is arranged in levels, with all nodes on the same level listening to the same address
     * Levels are assigned by network level ie: nodes 01-05: Level 1, nodes 011-055: Level 2
     * @see
     * - multicastLevel()
     * - multicastRelay
     * @param header reference to the RF24NetworkHeader object used for this @p message
     * @param message Pointer to memory where the message is located
     * @param len The size of the message
     * @param level Multicast level to broadcast to. If this parameter is unspecified, then the
     * node's current multicastLevel() is used.
     * @return Whether the message was successfully sent
     */
    bool multicast(RF24NetworkHeader& header, const void* message, uint16_t len, uint8_t level = 7);

#endif

    /**
     * Writes a direct (unicast) payload. This allows routing or sending messages outside of the usual routing paths.
     * The same as write, but a physical address is specified as the last option.
     * The payload will be written to the physical address, and routed as necessary by the recipient.
     */
    bool write(RF24NetworkHeader& header, const void* message, uint16_t len, uint16_t writeDirect);

    /**
     * Sleep this node - For AVR devices only
     * @note NEW - Nodes can now be slept while the radio is not actively transmitting. This must be manually enabled by uncommenting
     * the `#define ENABLE_SLEEP_MODE` in RF24Network_config.h
     * @note The watchdog timer should be configured in the sketch's `setup()` if using sleep mode.
     * This function will sleep the node, with the radio still active in receive mode. See setup_watchdog().
     *
     * The node can be awoken in two ways, both of which can be enabled simultaneously:
     * 1. An interrupt - usually triggered by the radio receiving a payload. Must use pin 2 (interrupt 0) or 3 (interrupt 1) on Uno, Nano, etc.
     * 2. The watchdog timer waking the MCU after a designated period of time, can also be used instead of delays to control transmission intervals.
     *
     * @code
     * if(!network.available())
     *     network.sleepNode(1, 0); // Sleep the node for 1 second or a payload is received
     *
     * // Other options:
     * network.sleepNode(0, 0);     // Sleep this node for the designated time period, or a payload is received.
     * network.sleepNode(1, 255);   // Sleep this node for 1 cycle. Do not wake up until then, even if a payload is received ( no interrupt )
     * @endcode
     *
     * @param cycles: The node will sleep in cycles of 1s. Using 2 will sleep 2 WDT cycles, 3 sleeps 3WDT cycles...
     * @param interruptPin: The interrupt number to use (0, 1) for pins 2 and 3 on Uno & Nano. More available on Mega etc.
     * Setting this parameter to 255 will disable interrupt wake-ups.
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
     * @return This node's parent address, or 65535 (-1 when casted to a signed int16_t) if this is the master node.
     */
    uint16_t parent() const;

    /**
     * Provided a node address and a pipe number, will return the RF24Network address of that child pipe for that node.
     */
    uint16_t addressOfPipe(uint16_t node, uint8_t pipeNo);

    /**
     * Validate a network address as a proper logical address
     * @note Addresses are specified in octal form, ie 011, 034.
     * Review [RF24Network addressing](md_docs_addressing.html) for more information.
     * @param node The specified logical address of a network node.
     * @return True if the specified `node` address is a valid network address, otherwise false.
     * @remark This function will validate an improper address of `0100` as it is the reserved
     * @ref NETWORK_MULTICAST_ADDRESS used for multicasted messages.
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
     * @deprecated Use `RF24::setChannel()` to configure the radio channel.
     * Use ESBNetwork::begin(uint16_t _node_address) to set the node address.
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
     * @param _channel The RF channel to operate on.
     * @param _node_address The logical address of this node.
     */
    void begin(uint8_t _channel, uint16_t _node_address);

    /**@}*/
    /**
     * @name External Applications/Systems
     *
     * Interface for External Applications and Systems ( RF24Mesh, RF24Ethernet )
     */
    /**@{*/

    /**
     * @brief The raw system frame buffer.
     *
     * This member can be accessed to retrieve the latest received data just after it is enqueued.
     * This buffer is also used for outgoing data.
     * @warning Conditionally, this buffer may only contain fragments of a message (either
     * outgoing or incoming).
     * @note The first 8 bytes of this buffer is latest handled frame's RF24NetworkHeader data.
     */
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
     * memory location of the received frame. This is used by external data systems (RF24Ethernet) to immediately copy the received
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
     * | System Message Types (Not Returned) |
     * |-----------------------|
     * | @ref NETWORK_ADDR_RESPONSE |
     * | @ref NETWORK_ACK           |
     * | @ref NETWORK_PING          |
     * | @ref NETWORK_POLL (With multicast enabled) |
     * | @ref NETWORK_REQ_ADDRESS   |
     */
    bool returnSysMsgs;

    /**
     * Network Flags allow control of data flow
     *
     * Incoming Blocking: If the network user-cache is full, lets radio cache fill up. Radio ACKs are not sent when radio internal cache is full.
     * This behaviour may seem to result in more failed sends, but the payloads would have otherwise been dropped due to the cache being full.
     *
     * | FLAGS | Value | Description |
     * |-------|-------|-------------|
     * | @ref FLAG_FAST_FRAG| 4 (bit 2 asserted) | INTERNAL: Replaces the fastFragTransfer variable, and allows for faster transfers between directly connected nodes. |
     * | @ref FLAG_NO_POLL| 8 (bit 3 asserted) | EXTERNAL/USER: Disables @ref NETWORK_POLL responses on a node-by-node basis. |
     *
     * @note Bit posistions 0 & 1 in the `networkFlags` byte are no longer used as they once were
     * during experimental development.
     */
    uint8_t networkFlags;

protected:
#if defined(RF24NetworkMulticast)
    /**
     * The current node's network level (used for multicast TX/RX-ing).
     * @see Use multicastLevel() to adjust this when needed.
     */
    uint8_t _multicast_level;
#endif
    /**
     * Logical node address of this unit, typically in range [0, 2925] (that's [0, 05555] in
     * octal).
     * @note The values 0 represents the network master node. Additionally, the value 1 is occupied
     * when using RF24Ethernet layer.
     */
    uint16_t node_address;

private:
    /**
     * @brief This function is the second to last stage a frame reaches before transmission.
     * @param to_node Sets the outgoing traffic direction. Values passed to this parameter usually the
     * destination node's logical address.
     * @param sendType Specifies what "behavior" the outgoing transmission will use.
     * | value |         macro name          |     corresponding behavior               |
     * |-------|-----------------------------|------------------------------------------|
     * |   0   | TX_NORMAL                   | First Payload, std routing    (auto-ack) |
     * |   1   | TX_ROUTED                   | routed payload                (auto-ack) |
     * |   2   | USER_TX_TO_PHYSICAL_ADDRESS | direct Route to host       (no auto-ack) |
     * |   3   | USER_TX_TO_LOGICAL_ADDRESS  | direct Route to Route      (no auto-ack) |
     * |   4   | USER_TX_MULTICAST           | multicast to several nodes (no auto-ack) |
     * @note The "auto-ack" description in the above table pretains to the radio's auto-ack feature. This
     * does not necessarily mean that the behavior will or won't invoke a NETWORK_ACK message (which depends
     * on the outgoing message's type). NETWORK ACK messages are served to message types in range [65, 191]
     * (excluding message fragments that aren't the last fragment of the message).
     */
    bool write(uint16_t to_node, uint8_t sendType);

    /**
     * @brief The last stage an outgoing frame reaches (actual/inital transmission is done here).
     *
     * The parameters for this function are the result translation of `logicalToPhysicalAddress()`.
     * Internally, the `networkFlags` FLAG_FAST_FRAG is used here (set beforehand) to avoid unnecessarily
     * re-configuring the radio during transmission of fragmented messages.
     */
    bool write_to_pipe(uint16_t node, uint8_t pipe, bool multicast);

    /**
     * @brief Enqueue a frame (referenced by its beginning header) in the node's queue.
     * @returns
     * - 0 if queue's size is maxed out
     * - 1 if frame is successfully enqueued
     * - 2 if EXTERNAL_DATA is detected (indicating that it is in the queue and should be
     *   handled by an external system like RF24Gateway or RF24Ethernet).
     */
    uint8_t enqueue(RF24NetworkHeader* header);

    /*
     * Called from begin(), this sets up the radio to act accordingly per the
     * logical `_node_address` parameter passed to `begin()`.
     *
     * Based on the value of the private member `node_address`, the resulting confiuration affects
     * private members `node_mask`, `parent_node`, `parent_pipe`, and `_multicast_level`.
     */
    void setup_address(void);

    /*
     * This (non-overloaded) function copies the outgoing frame into the `frame_buffer` and detirmines
     * the initial values passed into `logicalToPhysicalAddress()` (based on the value passed
     * to the `writeDirect` parameter). This is always called from either of the overloaded public
     * `write()` functions.
     */
    bool _write(RF24NetworkHeader& header, const void* message, uint16_t len, uint16_t writeDirect);

    struct logicalToPhysicalStruct
    {
        /** The immediate destination (1 hop) of an outgoing frame */
        uint16_t send_node;
        /** The pipe number of the `send_node` for which outgoing packets are aimed at */
        uint8_t send_pipe;
        /** A flag to indicate that the outgoing frame does not want an auto-ack from `send_node` */
        bool multicast;
    };

    /*
     * Translates an outgoing frame's header information into the current node's
     * required information (`logicalToPhysicalStruct`) for making the transmission.
     *
     * This returns void because the translated results are stored in the
     * `logicalToPhysicalStruct` passed by reference.
     */
    void logicalToPhysicalAddress(logicalToPhysicalStruct* conversionInfo);

    /********* only called from `logicalToPhysicalAddress()` ***************/

    /* Returns true if the given logical address (`node` parameter) is a direct child of the current node; otherwise returns false. */
    bool is_direct_child(uint16_t node);
    /* Returns true if the given logical address (`node` parameter) is a descendent of the current node; otherwise returns false. */
    bool is_descendant(uint16_t node);
    /* Returns a logical address for the first child en route to a child node */
    uint16_t direct_child_route_to(uint16_t node);

    /***********************************************************************/

    radio_t& radio; /** Underlying radio driver, provides link/physical layers */

    uint8_t frame_size;                                                                            /* The outgoing frame's total size including the header info. Ranges [8, MAX_PAYLOAD_SIZE] */
    const static unsigned int max_frame_payload_size = MAX_FRAME_SIZE - sizeof(RF24NetworkHeader); /* always 24 bytes to compensate for the frame's header */

#if defined(RF24_LINUX)
    std::queue<RF24NetworkFrame> frame_queue;
    std::map<uint16_t, RF24NetworkFrame> frameFragmentsCache;
    bool appendFragmentToFrame(RF24NetworkFrame frame);
#else // Not Linux:

    #if defined(DISABLE_USER_PAYLOADS)
    uint8_t frame_queue[1];  /** Space for a small set of frames that need to be delivered to the app layer */
    #else
    uint8_t frame_queue[MAIN_BUFFER_SIZE]; /** Space for a small set of frames that need to be delivered to the app layer */
    #endif

    uint8_t* next_frame;                                 /** Pointer into the @p frame_queue where we should place the next received frame */

    #if !defined(DISABLE_FRAGMENTATION)
    RF24NetworkFrame frag_queue;                         /* a cache for re-assembling incoming message fragments */
    uint8_t frag_queue_message_buffer[MAX_PAYLOAD_SIZE]; //frame size + 1
    #endif

#endif // Linux/Not Linux

    uint16_t parent_node; /** Our parent's node address */
    uint8_t parent_pipe;  /** The pipe our parent uses to listen to us */
    uint16_t node_mask;   /** The bits which contain significant node address information */

    /* Given the Logical node address & a pipe number, this returns the Physical address assigned to the radio's pipes. */
    uint64_t pipe_address(uint16_t node, uint8_t pipe);

#if defined ENABLE_NETWORK_STATS
    uint32_t nFails;
    uint32_t nOK;
#endif

#if defined(RF24NetworkMulticast)
    /* translates network level number (0-3) to a Logical address (used for TX multicasting) */
    uint16_t levelToAddress(uint8_t level);
#endif

    /** @} */
};

/**
 * A type definition of the template class `ESBNetwork` to maintain backward compatibility.
 *
 * ```.cpp
 * RF24 radio(7, 8);
 *
 * RF24Network network(radio);
 * // is equivalent to
 * ESBNetwork<RF24> network(radio);
 * ```
 */
typedef ESBNetwork<RF24> RF24Network;
#if defined(ARDUINO_ARCH_NRF52) || defined(ARDUINO_ARCH_NRF52840) || defined(ARDUINO_ARCH_NRF52833)
typedef ESBNetwork<nrf_to_nrf> RF52Network;
#endif

/**
 * @example helloworld_tx.ino
 *
 * Simplest possible example of using RF24Network. Put this sketch
 * on one node, and helloworld_rx.pde on the other. Tx will send
 * Rx a nice message every 2 seconds which rx will print out for us.
 */

/**
 * @example helloworld_rx.ino
 *
 * Simplest possible example of using RF24Network. Put this sketch
 * on one node, and helloworld_tx.pde on the other. Tx will send
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
 *
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
 *
 * - Using this sketch, each node will send a ping to every other node in the network every few seconds.<br>
 * - The RF24Network library will route the message across the mesh to the correct node.
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
