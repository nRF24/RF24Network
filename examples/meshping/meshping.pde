/*
 Copyright (C) 2011 James Coliz, Jr. <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/**
 * Example of a cross-pinging mesh network
 *
 * Using this sketch, each node will send a ping to one other node every
 * 2 seconds.  The RF24Network library will route the message across
 * the mesh to the correct node.
 *
 * To see the underlying frames being relayed, compile RF24Network with
 * #define SERIAL_DEBUG.
 *
 * The logical node address of each node is set in EEPROM.  The nodeconfig
 * module handles this by listening for a digit (0-9) on the serial port,
 * and writing that number to EEPROM.
 *
 * Set up the topology below to accurately reflect the layout of your
 * network.  Each line corresponds to one node address, and specifies
 * the unique pipe addresses for talking/listening to the parent, and
 * specifies the parent node for each parent.
 *
 * Any node can have up to 6 children.
 */

#include <avr/pgmspace.h>
#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include "nodeconfig.h"
#include "printf.h"

// Avoid spurious warnings
#undef PROGMEM 
#define PROGMEM __attribute__(( section(".progmem.data") )) 
#undef PSTR 
#define PSTR(s) (__extension__({static prog_char __c[] PROGMEM = (s); &__c[0];}))

RF24 radio(8,9);
RF24Network network(radio);

// Our node address
uint16_t this_node;
uint16_t next_node_to;
const uint16_t max_node = 2; 

// The message that we send is just a ulong, containing the time
unsigned long message;

// Delay manager to send pings regularly
const unsigned long interval = 2000; // ms
unsigned long last_time_sent;

void setup(void)
{
  //
  // Print preamble
  //
  
  Serial.begin(57600);
  printf_begin();
  printf("\n\rRF24Network/examples/meshping/\n\r");
  
  //
  // Pull node address out of eeprom 
  //

  // Which node are we?
  this_node = nodeconfig_read();

  // Decide which node to send to next
  next_node_to = this_node + 1;
  if ( next_node_to > max_node )
    next_node_to = 0;

  //
  // Bring up the RF network
  //

  SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 100, /*node address*/ this_node, /*directionality*/ RF24_NET_BIDIRECTIONAL);
}

void loop(void)
{
  // Pump the network regularly
  network.update();

  // Is there anything ready for us?
  while ( network.available() )
  {
    // If so, grab it and print it out
    RF24NetworkHeader header;
    network.read(header,&message,sizeof(unsigned long));
    printf_P(PSTR("%lu: APP Received %lu from %u\n\r"),millis(),message,header.from_node);
  }

  // Send a ping to the other guy every 'interval' ms
  unsigned long now = millis();
  if ( now - last_time_sent >= interval )
  {
    last_time_sent = now;

    printf_P(PSTR("---------------------------------\n\r"));
    printf_P(PSTR("%lu: APP Sending %lu to %u...\n\r"),millis(),now,next_node_to);
    
    message = now;
    RF24NetworkHeader header(/*to node*/ next_node_to);
    bool ok = network.write(header,&message,sizeof(unsigned long));
    if (ok)
    {
      printf_P(PSTR("%lu: APP Send ok\n\r"),millis());
    }
    else
    {
      printf_P(PSTR("%lu: APP Send failed\n\r"),millis());

      // Try sending at a different time next time
      last_time_sent -= 100;
    }
    
    // Decide which node to send to next
    if ( ++next_node_to > max_node )
      next_node_to = 0;
  }

  // Listen for a new node address
  nodeconfig_listen();
}
// vim:ai:cin:sts=2 sw=2 ft=cpp
