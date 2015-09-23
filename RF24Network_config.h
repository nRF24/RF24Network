
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

    /** The size of the main buffer. This is the user-cache, where incoming data is stored.
     * Data is stored using Frames: Header (8-bytes) + Frame_Size (2-bytes) + Data (?-bytes)
     * 
     * @note The MAX_PAYLOAD_SIZE is (MAIN_BUFFER_SIZE - 10), and the result must be divisible by 24.
     */
    #define MAIN_BUFFER_SIZE 144 + 10

    /** Maximum size of fragmented network frames and fragmentation cache. This MUST BE divisible by 24.
    * @note: Must be a multiple of 24.
    * @note: If used with RF24Ethernet, this value is used to set the buffer sizes.
    */
    #define MAX_PAYLOAD_SIZE  MAIN_BUFFER_SIZE-10

    /** Disable user payloads. Saves memory when used with RF24Ethernet or software that uses external data.*/
    //#define DISABLE_USER_PAYLOADS 

    /** Enable tracking of success and failures for all transmissions, routed and user initiated */
    //#define ENABLE_NETWORK_STATS
    
    /** Enable dynamic payloads - If using different types of NRF24L01 modules, some may be incompatible when using this feature **/
    #define ENABLE_DYNAMIC_PAYLOADS

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
    #define MAIN_BUFFER_SIZE 96 + 10
    //#define MAX_PAYLOAD_SIZE  MAIN_BUFFER_SIZE-10
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
    #if defined (ARDUINO_ARCH_ESP8266)
	  #include <pgmspace.h>
	  #define PRIPSTR "%S"
	  #define printf_P printf
	  #define sprintf_P sprintf
    
    #elif defined(ARDUINO) && ! defined(__arm__)  && !defined (__ARDUINO_X86__)
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

