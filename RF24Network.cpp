/*
 Copyright (C) 2011 James Coliz, Jr. <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#include <WProgram.h>
#include <RF24Network.h>
#include <RF24.h>

// Avoid spurious warnings
#undef PROGMEM 
#define PROGMEM __attribute__(( section(".progmem.data") )) 
#undef PSTR 
#define PSTR(s) (__extension__({static prog_char __c[] PROGMEM = (s); &__c[0];}))

RF24Network::RF24Network( RF24& _radio, const RF24NodeLine* _topology ): radio(_radio), topology(_topology)
{
}

void RF24Network::begin(uint8_t _channel, uint16_t _node_address, rf24_direction_e /*_direction*/ )
{
  node_address = _node_address;
  radio.setChannel(_channel);
}

void RF24Network::update(void)
{
}

bool RF24Network::available(void)
{
  return false;
}

size_t RF24Network::read(RF24NetworkHeader& /*header*/,void* /*buf*/, size_t /*maxlen*/)
{
  return 0;
}

bool RF24Network::write(RF24NetworkHeader& /*header*/,const void* /*buf*/, size_t /*len*/)
{
  return false;
}

// vim:ai:cin:sts=2 sw=2 ft=cpp
