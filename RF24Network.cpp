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
  num_nodes = 0;
  const RF24NodeLine* i = topology;
  while ( (i++)->parent_node != 0xFFFF )
    ++num_nodes;
}

void RF24Network::begin(uint8_t _channel, uint16_t _node_address, rf24_direction_e /*_direction*/ )
{
  if ( node_address < num_nodes )
    node_address = _node_address;

  radio.setChannel(_channel);
  radio.setDataRate(RF24_1MBPS);
  radio.setCRCLength(RF24_CRC_16);
  radio.startListening();
  radio.printDetails();
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

void RF24Network::open_pipes(void)
{
  // In order to open the right pipes, we need to know whether the node has parents
  // and/or children
  bool has_parent = ( topology[node_address].parent_node != 0 );
  bool has_children = false;

  // If there are any nodes in the topology table which consider this
  // a parent, then we do have children 
  int i = num_nodes;
  while (i-- && !has_children)
    if ( topology[i].parent_node == node_address )
      has_children = true; 

  // Open pipes for parent 
  if ( has_parent )
  {
    // Writing pipe to speak to our parent
    radio.openWritingPipe(topology[node_address].talking_pipe);

    // Listen to our parent.  If we have children, we need to do so
    // on pipe 0 to make room for more children
    if ( has_children )
      radio.openReadingPipe(0,topology[node_address].listening_pipe);
    else
      radio.openReadingPipe(1,topology[node_address].listening_pipe);
  }

  // Listen on children's talking pipes
  if ( has_children )
  {
    // First child listening pipe is #1
    uint8_t current_pipe = 1;
  
    // The topology table tells us who our children are
    int i = num_nodes;
    while (i--)
    {
      if ( topology[i].parent_node == node_address )
	radio.openReadingPipe(current_pipe++,topology[i].talking_pipe);
    }
  }

}

// vim:ai:cin:sts=2 sw=2 ft=cpp
