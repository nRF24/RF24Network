/*
 Copyright (C) 2011 James Coliz, Jr. <maniacbug@ymail.com>
 Copyright (C) 2014 TMRh20 <tmrh20@gmail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#ifndef __RF24NETWORK_CONFIG_H__
#define __RF24NETWORK_CONFIG_H__


/********** USER CONFIG **************/

#define RF24NetworkMulticast      //Use multicast capabilities
//#define SERIAL_DEBUG              //Set for full debug
#define SERIAL_DEBUG_MINIMAL        //Enable minimal debugging
//#define SERIAL_DEBUG_ROUTING
//#define SERIAL_DEBUG_FRAGMENTATION

/*************************************/

#endif

#ifndef __RF24_CONFIG_H__
#define __RF24_CONFIG_H__
// Stuff that is normally provided by Arduino
#ifndef ARDUINO
  #include <stdint.h>
  #include <stdio.h>
  #include <string.h>
  #define _BV(x) (1<<(x))
#endif

#if defined (SERIAL_DEBUG)
  #define IF_SERIAL_DEBUG(x) ({x;})
#else
  #define IF_SERIAL_DEBUG(x)
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
#if ! defined( NATIVE ) && defined( ARDUINO )
  #undef PROGMEM
  #define PROGMEM __attribute__(( section(".progmem.data") ))
  #undef PSTR
  #define PSTR(s) (__extension__({static const char __c[] PROGMEM = (s); &__c[0];}))
#endif

// Progmem is Arduino-specific
#ifdef ARDUINO
  #include <avr/pgmspace.h>
  #define PRIPSTR "%S"
#else
  typedef char const prog_char;
  typedef uint16_t prog_uint16_t;
  #define PSTR(x) (x)
  #define printf_P printf
  #define strlen_P strlen
  #define PROGMEM
  #define pgm_read_word(p) (*(p))
  #define PRIPSTR "%s"
#endif

#endif // __RF24_CONFIG_H__

// vim:ai:cin:sts=2 sw=2 ft=cpp
