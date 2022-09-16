
/*
 Copyright (C) 2011 James Coliz, Jr. <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#ifndef __RF24NETWORK_CONFIG_H__
#define __RF24NETWORK_CONFIG_H__

/** @brief A reserved valid address for use with RF24Mesh (when a mesh node requests an assigned address) */
#ifndef NETWORK_DEFAULT_ADDRESS
    #define NETWORK_DEFAULT_ADDRESS 04444
#endif // NETWORK_DEFAULT_ADDRESS

/** @brief A sentinel address value for multicasting purposes */
#define NETWORK_MULTICAST_ADDRESS 0100

/** @brief A sentinel value for internally indicating that the frame should be automatically routed as necessary */
#define NETWORK_AUTO_ROUTING 070

#ifdef DOXYGEN_FORCED
    /**
     * @brief Adds a delay to node prior to transmitting @ref NETWORK_ADDR_RESPONSE messages
     *
     * By default this is undefined for speed. This defined number of milliseconds is
     * only applied to the master node when replying to a child trying to connect to the
     * mesh network.
     * @note It is advised to define this if any child node is running CircuitPython because
     * the execution speed in pure python is inherently slower than it is in C++.
     */
    #define SLOW_ADDR_POLL_RESPONSE 10
#endif // defined DOXYGEN_FORCED

#if !defined(__AVR_ATtiny85__) && !defined(__AVR_ATtiny84__)

    /********** USER CONFIG - non ATTiny **************/

    //#define ENABLE_SLEEP_MODE  //AVR only
    /** @brief When defined, this will allow the use of multicasting messages */
    #define RF24NetworkMulticast

    /* Saves memory by disabling fragmentation */
    //#define DISABLE_FRAGMENTATION

    /* System defines */

    /**
     * @brief Maximum size of fragmented network frames and fragmentation cache.
     * @note This buffer can now be any size > 24. Previously this needed to be a multiple of 24 (changed in v1.0.15).
     * @note If used with RF24Ethernet, this value is used to set the buffer sizes.
     * @note For nodes driven by an ATTiny based chip, this is set to 72. However, defining `DISABLE_FRAGMENTION` truncates
     * the actual transmitted payload to 24 bytes (which is also the default behavior on ATTiny devices).
     */
    #ifndef MAX_PAYLOAD_SIZE
        #if defined linux || defined __linux
            #define MAX_PAYLOAD_SIZE 1514
        #else
            #define MAX_PAYLOAD_SIZE 144
        #endif
    #endif // MAX_PAYLOAD_SIZE

    /**
     * @brief The allocated size of the incoming frame buffer.
     *
     * This is the user-cache, where incoming data is stored.
     * Data is stored using Frames: Header (8 bytes) + Message_Size (2 bytes) + Message_Data (? bytes)
     * @note Over-The-Air (OTA) transmissions don't include the message size in the transmitted packet.
     */
    #define MAIN_BUFFER_SIZE (MAX_PAYLOAD_SIZE + FRAME_HEADER_SIZE)

    /* Disable user payloads. Saves memory when used with RF24Ethernet or software that uses external data.*/
    //#define DISABLE_USER_PAYLOADS

    /* Enable tracking of success and failures for all transmissions, routed and user initiated */
    //#define ENABLE_NETWORK_STATS

    #ifndef DISABLE_DYNAMIC_PAYLOADS
        /** Enable dynamic payloads - If using different types of nRF24L01 modules, some may be incompatible when using this feature **/
        #define ENABLE_DYNAMIC_PAYLOADS
    #endif // DISABLE_DYNAMIC_PAYLOADS

    /* Debug Options */
    //#define SERIAL_DEBUG
    //#define SERIAL_DEBUG_MINIMAL
    //#define SERIAL_DEBUG_ROUTING
    //#define SERIAL_DEBUG_FRAGMENTATION
    //#define SERIAL_DEBUG_FRAGMENTATION_L2
    /*************************************/

#else // Different set of defaults for ATTiny - fragmentation is disabled and user payloads are set to 3 max
    /********** USER CONFIG - ATTiny **************/
    //#define ENABLE_SLEEP_MODE  //AVR only
    #define RF24NetworkMulticast
    // NOTE: Only 24 bytes of a payload are used when DISABLE_FRAGMENTATION is defined
    #define MAX_PAYLOAD_SIZE 72
    #define MAIN_BUFFER_SIZE (MAX_PAYLOAD_SIZE + FRAME_HEADER_SIZE)
    #define DISABLE_FRAGMENTATION
    #define ENABLE_DYNAMIC_PAYLOADS
    //#define DISABLE_USER_PAYLOADS
#endif
/*************************************/

#endif // RF24_NETWORK_CONFIG_H

#ifdef __cplusplus

#if (defined(__linux) || defined(linux)) && !defined(__ARDUINO_X86__) && !defined(USE_RF24_LIB_SRC)
    #include <RF24/RF24_config.h>

// ATXMega
#elif defined(XMEGA)
    #include "../../rf24lib/rf24lib/RF24_config.h"
#else
    #include <RF24_config.h>
#endif

#if !defined(ARDUINO_ARCH_AVR)
    // sprintf is used by RF24NetworkHeader::toString
    #ifndef sprintf_P
        #define sprintf_P sprintf
    #endif
#endif

#if defined(SERIAL_DEBUG_MINIMAL)
    #define IF_SERIAL_DEBUG_MINIMAL(x) ({ x; })
#else
    #define IF_SERIAL_DEBUG_MINIMAL(x)
#endif

#if defined(SERIAL_DEBUG_FRAGMENTATION)
    #define IF_SERIAL_DEBUG_FRAGMENTATION(x) ({ x; })
#else
    #define IF_SERIAL_DEBUG_FRAGMENTATION(x)
#endif

#if defined(SERIAL_DEBUG_FRAGMENTATION_L2)
    #define IF_SERIAL_DEBUG_FRAGMENTATION_L2(x) ({ x; })
#else
    #define IF_SERIAL_DEBUG_FRAGMENTATION_L2(x)
#endif

#if defined(SERIAL_DEBUG_ROUTING)
    #define IF_SERIAL_DEBUG_ROUTING(x) ({ x; })
#else
    #define IF_SERIAL_DEBUG_ROUTING(x)
#endif

#endif // RF24_CONFIG_H
