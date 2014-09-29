
/*
 Copyright (C) 2011 James Coliz, Jr. <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#ifndef __RF24NETWORK_CONFIG_H__
#define __RF24NETWORK_CONFIG_H__

#if ARDUINO < 100
#include <WProgram.h>
#else
#include <Arduino.h>
#endif

#include <stddef.h>

/********** USER CONFIG **************/

//#define DUAL_HEAD_RADIO
//#define ENABLE_SLEEP_MODE
#define RF24NetworkMulticast
#define DISABLE_FRAGMENTATION // Saves a bit of space by disabling fragmentation

//#define SERIAL_DEBUG
//#define SERIAL_DEBUG_MINIMAL
//#define SERIAL_DEBUG_ROUTING
//#define SERIAL_DEBUG_FRAGMENTATION
/*************************************/
 
#endif

#ifndef __RF24_CONFIG_H__
#define __RF24_CONFIG_H__

  // Define _BV for non-Arduino platforms and for Arduino DUE
#if defined (ARDUINO) && !defined (__arm__)
	#if !defined(__AVR_ATtiny25__) && !defined(__AVR_ATtiny45__) && !defined(__AVR_ATtiny85__) && !defined(__AVR_ATtiny24__) && !defined(__AVR_ATtiny44__) && !defined(__AVR_ATtiny84__)
		#include <SPI.h>
	#endif
#else

  #include <stdint.h>
  #include <stdio.h>
  #include <string.h>


 #if defined(__arm__) || defined (CORE_TEENSY)
   #include <SPI.h>
 #endif

 #if !defined(CORE_TEENSY)
   #define _BV(x) (1<<(x))
   #if !defined(__arm__)
     extern HardwareSPI SPI;
   #endif
 #endif


#endif


  
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
#if defined(ARDUINO) && ! defined(__arm__)
	#include <avr/pgmspace.h>
	#define PRIPSTR "%S"
#else
#if ! defined(ARDUINO) // This doesn't work on Arduino DUE
	typedef char const char;
#else // Fill in pgm_read_byte that is used, but missing from DUE
	#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
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
#endif // __RF24_CONFIG_H__
// vim:ai:cin:sts=2 sw=2 ft=cpp
