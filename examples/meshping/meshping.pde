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
  { 0xE7E7E7E7FELL, 0xE7E7E7E70ELL, 1 }, // Node 2: Leaf, child of #1
  RF24NODELINE_LIST_END
};

RF24 radio(8,9);
RF24Network network(radio,topology);

// Node identities
const int role_pin = 7; // Connect to ground on one unit, leave disconnected on the other
uint16_t this_node;
uint16_t other_node;

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
  // Node configuration 
  //

  pinMode(role_pin,INPUT);
  digitalWrite(role_pin,HIGH);
  switch ( digitalRead(role_pin) )
  {
    case LOW:
      this_node = 1;
      other_node = 2;
      break;
    case HIGH:
      this_node = 2;
      other_node = 1;
      break;
  }
  printf("ADDRESS: %i\n\r",this_node);

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
  if ( now - last_time_sent > interval )
  {
    last_time_sent = now;

    printf_P(PSTR("%lu: APP Sending %lu...\n\r"),millis(),now);
    
    message = now;
    RF24NetworkHeader header(/*to node*/ other_node);
    bool ok = network.write(header,&message,sizeof(unsigned long));
    if (ok)
    {
      printf_P(PSTR("%lu: APP Send ok\n\r"),millis());
    }
    else
    {
      printf_P(PSTR("%lu: APP Send failed\n\r"),millis());

      // Try sending at a different time next time
      last_time_sent += 100;
    }
  }
}
// vim:ai:cin:sts=2 sw=2 ft=cpp
