/*
 Copyright (C) 2011 James Coliz, Jr. <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/**
 * Example of a sensor network 
 *
 * This sketch demonstrates how to use the RF24Network library to
 * manage a set of low-power sensor nodes which mostly sleep but
 * awake regularly to send readings to the base.
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
#include "sleep.h"
#include "printf.h"

// Avoid spurious warnings
#undef PROGMEM 
#define PROGMEM __attribute__(( section(".progmem.data") )) 
#undef PSTR 
#define PSTR(s) (__extension__({static prog_char __c[] PROGMEM = (s); &__c[0];}))

// This exact same code will work with an unlimited number of nodes connected in a giant mesh.  
// Increase the sleep delay with many more nodes!
RF24NodeLine topology[] = 
{
  RF24NODELINE_LIST_BEGIN
  { 0xE7E7E7E7FELL, 0xE7E7E7E701LL, 0 }, // Node 1: Base, has no parent
  { 0xE7E7E7E7F8LL, 0xE7E7E7E708LL, 1 }, // Node 2: Leaf, child of #1
  { 0xE7E7E7E7F0LL, 0xE7E7E7E710LL, 1 }, // Node 3: Leaf, child of #1
  { 0xE7E7E7E7E9LL, 0xE7E7E7E717LL, 1 }, // Node 4: Leaf, child of #1
  { 0xE7E7E7E7E4LL, 0xE7E7E7E71CLL, 1 }, // Node 5: Leaf, child of #1
  RF24NODELINE_LIST_END
};

RF24 radio(8,9);
RF24Network network(radio,topology);

// Our node address
uint16_t this_node;

// The message that we send is just a ulong, containing the time
unsigned long message;

// Sleep constants.  In this example, the watchdog timer wakes up
// every 1s, and every 4th wakeup we power up the radio and send
// a reading.  In real use, these numbers which be much higher.
// Try wdt_8s and 7 cycles for one reading per minute.
const wdt_prescalar_e wdt_prescalar = wdt_1s;
const short sleep_cycles_per_transmission = 4;

void setup(void)
{
  //
  // Print preamble
  //
  
  Serial.begin(57600);
  printf_begin();
  printf("\n\rRF24Network/examples/sensornet/\n\r");
  
  //
  // Pull node address out of eeprom 
  //

  // Which node are we?
  this_node = nodeconfig_read();

  //
  // Prepare sleep parameters
  //

  // Only the leaves sleep.  
  if ( this_node > 1 ) 
    Sleep.begin(wdt_prescalar,sleep_cycles_per_transmission);

  //
  // Bring up the RF network
  //

  SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 95, /*node address*/ this_node, /*directionality*/ RF24_NET_UNIDIRECTIONAL);
}

void loop(void)
{
  // Pump the network regularly
  network.update();

  // If we are the base, is there anything ready for us?
  while ( this_node == 1 && network.available() )
  {
    // If so, grab it and print it out
    RF24NetworkHeader header;
    network.read(header,&message,sizeof(unsigned long));
    printf_P(PSTR("%lu: APP Received %lu from %u\n\r"),millis(),message,header.from_node);
  }

  // If we are not the base, send sensor readings to the base
  if ( this_node > 1 )
  {
    // Take a 'reading'.  Just using the millis() clock for an example 'reading'
    message = millis();

    printf_P(PSTR("---------------------------------\n\r"));
    printf_P(PSTR("%lu: APP Sending %lu to %u...\n\r"),millis(),message,1);
    
    // Send it to the base
    RF24NetworkHeader header(/*to node*/ 1);
    bool ok = network.write(header,&message,sizeof(unsigned long));
    if (ok)
      printf_P(PSTR("%lu: APP Send ok\n\r"),millis());
    else
      printf_P(PSTR("%lu: APP Send failed\n\r"),millis());
     
    // Power down the radio.  Note that the radio will get powered back up
    // on the next write() call.
    radio.powerDown();

    // Sleep the MCU.  The watchdog timer will awaken in a short while, and
    // continue execution here.
    Sleep.go();
  }

  // Listen for a new node address
  nodeconfig_listen();
}
// vim:ai:cin:sts=2 sw=2 ft=cpp
