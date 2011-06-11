/*
 Copyright (C) 2011 James Coliz, Jr. <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
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

// This is a very simple example, using 2 nodes.  This exact same code will work
// With an unlimited number of nodes connected in a giant mesh.  Increase the ping
// interval with many more nodes!
RF24NodeLine topology[] = 
{
  RF24NODELINE_LIST_BEGIN
  { 0xE7E7E7E7F1LL, 0xE7E7E7E701LL, 0 }, // Node 1: Base, has no parent
  { 0xE7E7E7E7F8LL, 0xE7E7E7E708LL, 1 }, // Node 2: Leaf, child of #1
  RF24NODELINE_LIST_END
};

RF24 radio(8,9);
RF24Network network(radio,topology);

// Our node address
uint16_t this_node;
uint16_t next_node_to;
const uint16_t max_node = sizeof(topology) / sizeof(RF24NodeLine) - 2 ;

// The message that we send is just a ulong, containing the time
unsigned long message;

// Delay manager to send pings regularly
const unsigned long interval = 2000; // ms
unsigned long last_time_sent;
bool running = false;

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
    next_node_to = 1;
  
  // Node 1 starts running right away, the rest wait
  // This makes it easier to test because we can set up
  // a repeatable system.  Bring all the other nodes up
  // first, and then bring up node 1, and it will all
  // work the same every time.
  if ( this_node == 1 )
    running = true;

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

    // We can start running once we get our first message
    if ( !running )
      running = true;
  }

  // Send a ping to the other guy every 'interval' ms
  unsigned long now = millis();
  if ( running && now - last_time_sent >= interval )
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
      next_node_to = 1;
  }

  // Listen for a new node address
  nodeconfig_listen();
}
// vim:ai:cin:sts=2 sw=2 ft=cpp
