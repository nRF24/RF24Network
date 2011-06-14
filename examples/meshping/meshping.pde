/*
 Copyright (C) 2011 James Coliz, Jr. <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/**
 * Example of pinging across a mesh network
 *
 * Using this sketch, each node will send a ping to the base every
 * few seconds.  The RF24Network library will route the message across
 * the mesh to the correct node.
 *
 * To see the underlying frames being relayed, compile RF24Network with
 * #define SERIAL_DEBUG.
 *
 * The logical node address of each node is set in EEPROM.  The nodeconfig
 * module handles this by listening for a digit (0-9) on the serial port,
 * and writing that number to EEPROM.
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
  if ( this_node > 0 && now - last_time_sent >= interval )
  {
    last_time_sent = now;

    printf_P(PSTR("---------------------------------\n\r"));
    printf_P(PSTR("%lu: APP Sending %lu to base...\n\r"),millis(),now);
    
    message = now;
    RF24NetworkHeader header(/*to node*/ 0, /*type*/ 'T');
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
  }

  // Listen for a new node address
  nodeconfig_listen();
}
// vim:ai:cin:sts=2 sw=2 ft=cpp
