
/*
 Copyright (C) 2011 James Coliz, Jr. <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#ifndef __RF24NETWORK_CONFIG_H__
#define __RF24NETWORK_CONFIG_H__

  #ifdef __cplusplus

    #if (defined (__linux) || defined (linux)) && !defined (__ARDUINO_X86__)
      #define RF24_LINUX
      #include <stdint.h>
      #include <stdio.h>
      #include <string.h>
      #define _BV(x) (1<<(x))
    #endif

    #if defined (ARDUINO)
      #if ARDUINO < 100
        #include <WProgram.h>
      #else
        #include <Arduino.h>
      #endif
    #endif

    #include <stddef.h>

  #endif //cplusplus: Allows RF24Ethernet(uIP) or other c programs to get access to the RF24Network config variables

  #if !defined(__AVR_ATtiny85__) && !defined(__AVR_ATtiny84__)

    /********** USER CONFIG - non ATTiny **************/

    //#define DUAL_HEAD_RADIO
    //#define ENABLE_SLEEP_MODE  //AVR only
    #define RF24NetworkMulticast

    /** \def
	 * Saves memory by disabling fragmentation
	 */
    //#define DISABLE_FRAGMENTATION 

    /** System defines */

    /** Size of fragmented network frames 
    *
    * @note: If used with RF24Ethernet, this value should match the uip buffer size (default 120). Different nodes can use different max payload sizes,
    * but are limited to the smallest of the two when in direct communication. Routing nodes do not need to support fragmentation.
    * With RF24ethernet, assign in multiples of 24. General minimum is 96 (a 32-byte ping from windows is 74 bytes, (Ethernet Header is 42))
    * 
    */
    #define MAX_PAYLOAD_SIZE  144

    /** The number of 24-byte payloads RF24Network will automatically buffer for network.read().
    * If using fragmentation, this value multiplied by 24 must be larger than the MAX_PAYLOAD_SIZE defined above */
    #define NUM_USER_PAYLOADS 5

    /** Disable user payloads. Saves memory when used with RF24Ethernet or software that uses external data.*/
    //#define DISABLE_USER_PAYLOADS 

    /** Enable tracking of success and failures for all transmissions, routed and user initiated */
    //#define ENABLE_NETWORK_STATS

    /** Debug Options */
    //#define SERIAL_DEBUG
    //#define SERIAL_DEBUG_MINIMAL
    //#define SERIAL_DEBUG_ROUTING
    //#define SERIAL_DEBUG_FRAGMENTATION
    //#define SERIAL_DEBUG_FRAGMENTATION_L2
    /*************************************/
 
  #else // Different set of defaults for ATTiny - fragmentation is disabled and user payloads are set to 3 max
    /********** USER CONFIG - ATTiny **************/
    //#define DUAL_HEAD_RADIO
    //#define ENABLE_SLEEP_MODE  //AVR only
    #define RF24NetworkMulticast
    #define DISABLE_FRAGMENTATION 
    //#define MAX_PAYLOAD_SIZE  96 
    #define NUM_USER_PAYLOADS 3
    //#define DISABLE_USER_PAYLOADS 
  #endif
  /*************************************/


#endif //RF24_NETWORK_CONFIG_H


#ifdef __cplusplus

  #ifndef rf24_max
    #define rf24_max(a,b) (a>b?a:b)
  #endif
  #ifndef rf24_min
    #define rf24_min(a,b) (a<b?a:b)
  #endif



  #ifndef __RF24_CONFIG_H__
  #define __RF24_CONFIG_H__

    #if defined (SERIAL_DEBUG)
      #define IF_SERIAL_DEBUG(x) ({x;})
    #else
	  #define IF_SERIAL_DEBUG(x)
	  #if defined(__AVR_ATtiny84__) || defined(__AVR_ATtiny85__)
	    #define printf_P(...)
      #endif
    #endif

    #if defined (SERIAL_DEBUG_MINIMAL)
      #define IF_SERIAL_DEBUG_MINIMAL(x) ({x;})
    #else
      #define IF_SERIAL_DEBUG_MINIMAL(x)
    #endif
  
    #if defined (SERIAL_DEBUG_FRAGMENTATION)
      #define IF_SERIAL_DEBUG_FRAGMENTATION(x) ({x;})
    #else
      #define IF_SERIAL_DEBUG_FRAGMENTATION(x)
    #endif

    #if defined (SERIAL_DEBUG_FRAGMENTATION_L2)
      #define IF_SERIAL_DEBUG_FRAGMENTATION_L2(x) ({x;})
    #else
      #define IF_SERIAL_DEBUG_FRAGMENTATION_L2(x)
    #endif
  
    #if defined (SERIAL_DEBUG_ROUTING)
      #define IF_SERIAL_DEBUG_ROUTING(x) ({x;})
    #else
      #define IF_SERIAL_DEBUG_ROUTING(x)
    #endif


    // Avoid spurious warnings
    // Arduino DUE is arm and uses traditional PROGMEM constructs
    #if 1
      #if ! defined( NATIVE ) && defined( ARDUINO ) && ! defined(__arm__)  && ! defined( CORE_TEENSY3 )
        #undef PROGMEM
        #define PROGMEM __attribute__(( section(".progmem.data") ))
        #undef PSTR
        #define PSTR(s) (__extension__({static const char __c[] PROGMEM = (s); &__c[0];}))
      #endif
    #endif

    // Progmem is Arduino-specific
    // Arduino DUE is arm and does not include avr/pgmspace
    #if defined(ARDUINO) && ! defined(__arm__)  && !defined (__ARDUINO_X86__)
	  #include <avr/pgmspace.h>
	  #define PRIPSTR "%S"
    #else
      #if ! defined(ARDUINO) && !defined (RF24_LINUX) // This doesn't work on Arduino DUE
	  typedef char const char;
      #else // Fill in pgm_read_byte that is used, but missing from DUE
	    #define pgm_read_byte(addr) (*(const unsigned char *)(addr))
      #endif

      #if !defined(sprintf_P)
	    #define sprintf_P sprintf
      #endif

      #if !defined (CORE_TEENSY)
	    typedef uint16_t prog_uint16_t;
	    #define PSTR(x) (x)
	    #define printf_P printf
	    #define strlen_P strlen
	    #define PROGMEM
	    #define pgm_read_word(p) (*(p))
      #endif

	  #define PRIPSTR "%s"

    #endif
  #endif  //cplusplus
#endif //RF24_CONFIG_H