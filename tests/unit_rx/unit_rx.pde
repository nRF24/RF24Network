/*
 Copyright (C) 2011 James Coliz, Jr. <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/**
 * Receiver for unit tests.
 */

#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <Sync.h>
#include "printf.h"

// nRF24L01(+) radio attached to SPI and pins 8 & 9
RF24 radio(8,9);

// Network uses that radio
RF24Network network(radio);

// Syncronizer
Sync sync(network);

// Address of our node
const uint16_t this_node = 1;

// Address of the other node
const uint16_t other_node = 0;

// Data to be synchronized
struct sync_data_t
{
  uint16_t first;
  uint16_t second;

  sync_data_t(void): first(1), second(2) {}
};

sync_data_t sync_data;

// Old value of 'first'
uint16_t old_first;

// Message buffer space
uint8_t message[32];

void send_finder_request(void);

void setup(void)
{
  Serial.begin(57600);
  Serial.println("RF24Network/test/unit_rx/");
  printf_begin();
 
  SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 100, /*node address*/ this_node);

  sync.register_me(sync_data);
  old_first = sync_data.first;
}

void loop(void)
{
  // Pump the network regularly
  sync.update();

  // Have the values changed?
  if ( old_first != sync_data.first )
  {
    printf_P(PSTR("%lu: APP Updated first to %u\n\r"),millis(),sync_data.first);

      // Move the first value over to the second
    sync_data.second = sync_data.first;

    // And remember the change for next time
    old_first = sync_data.first;
  }

  // Are there any messages for us?
  while ( network.available() )
  {
    uint16_t from;
    
    // If so, take a look at it 
    RF24NetworkHeader header;
    network.peek(header);
    
    // Dispatch the message to the correct handler.
    switch (header.type)
    {
    // Reset to initial state
    case 'R':
      network.read(header,0,0);
      printf_P(PSTR("%lu: APP Reset to initial state\n\r"),millis());

      sync_data = sync_data_t();
      old_first = sync_data.first;
      break;
    
    // Echo back to the sender.
    case 'E':
      network.read(header,message,sizeof(message));
      from = header.from_node;
      printf_P(PSTR("%lu: APP Received ECHO request from %o\n\r"),millis(),from);
      network.write(header = RF24NetworkHeader(from,'E'),message,sizeof(message));
      break;

    // Find child nodes
    case 'F':
      network.read(header,message,sizeof(message));
      from = header.from_node;
      printf_P(PSTR("%lu: APP Received FINDER request from %o\n\r"),millis(),from);

      // Send an 'E' Echo response back to the BASE
      network.write(header = RF24NetworkHeader(00,'E'),message,sizeof(message));

      send_finder_request();
      break;
    
    // Unrecognized message type
    default:
      printf_P(PSTR("*** WARNING *** Unknown message type %c\n\r"),header.type);
      network.read(header,0,0);
      break;
    };
  }
}

// 'Finder' message
void send_finder_request(void)
{
  // Send a 'F' Finder message to each child.  Only do this if it's possible for us
  // to have children.  Four-digit nodes (e.g. 05555) cannot have children
  if ( ! ( this_node & 07000 ) )
  {
    // Figure out the address of the first child.  e.g. if our node is 045, our
    // first child is 0145.  So we need to shift 01 up enough places to be the
    // highest digit
    uint16_t child = 01;
    uint16_t temp = this_node;
    while ( temp )
    {
      child <<= 3;
      temp >>= 3;
    }
    
    // Loop through each child address, and send an 'F' Finder message to them.
    uint16_t to_node = child + this_node;
    int i = 5;
    while ( i-- )
    {
      RF24NetworkHeader header(to_node,'F');
      bool ok = network.write(header,message,sizeof(message));
      to_node += child;

      // If it worked, let the base know to expect a response
      if ( ok )
      {
	printf_P(PSTR("%lu: APP Sent FINDER request to %o\n\r"),millis(),to_node);
	network.write(header = RF24NetworkHeader(00,'F'),&to_node,sizeof(to_node));
      }
      else
	printf_P(PSTR("%lu: APP Failed sending FINDER request to %o\n\r"),millis(),to_node);
    }
  }
}
// vim:ai:cin:sts=2 sw=2 ft=cpp
