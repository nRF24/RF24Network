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
 * The example uses TWO sensors, a 'temperature' sensor and a 'voltage'
 * sensor.
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
#include "sleep.h"
#include "printf.h"

// Avoid spurious warnings
#undef PROGMEM 
#define PROGMEM __attribute__(( section(".progmem.data") )) 
#undef PSTR 
#define PSTR(s) (__extension__({static prog_char __c[] PROGMEM = (s); &__c[0];}))

// This is for git version tracking.  Safe to ignore
#ifdef VERSION_H
#include "version.h"
#else
const char program_version[] = "Unknown";
#endif

RF24 radio(8,9);
RF24Network network(radio);

// Our node address
uint16_t this_node;

// The message that we send is just an unsigned int, containing a sensor reading.
struct message_t
{
  uint16_t temp_reading;
  uint16_t voltage_reading;
  message_t(void): temp_reading(0), voltage_reading(0) {}
};

// The pin our sensor is on
const int temp_sensor_pin = A2;
const int voltage_sensor_pin = A3;

// How many measurements to take.  64*1024 = 65536, so 64 is the max we can fit in a uint16_t.
const int num_measurements = 64;

// Sleep constants.  In this example, the watchdog timer wakes up
// every 1s, and every 4th wakeup we power up the radio and send
// a reading.  In real use, these numbers which be much higher.
// Try wdt_8s and 7 cycles for one reading per minute.> 1
const wdt_prescalar_e wdt_prescalar = wdt_1s;
const int sleep_cycles_per_transmission = 4;

void setup(void)
{
  //
  // Print preamble
  //
  
  Serial.begin(57600);
  printf_begin();
  printf_P(PSTR("\n\rRF24Network/examples/sensornet/\n\r"));
  printf_P(PSTR("VERSION: %s\n\r"),program_version);
  
  //
  // Pull node address out of eeprom 
  //

  // Which node are we?
  this_node = nodeconfig_read();

  //
  // Prepare sleep parameters
  //

  // Only the leaves sleep.  
  if ( this_node > 0 ) 
    Sleep.begin(wdt_prescalar,sleep_cycles_per_transmission);

  //
  // Bring up the RF network
  //

  SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 95, /*node address*/ this_node);
}

void loop(void)
{
  // Pump the network regularly
  network.update();

  // If we are the base, is there anything ready for us?
  while ( network.available() )
  {
    // If so, grab it and print it out
    RF24NetworkHeader header;
    message_t message;
    network.read(header,&message,sizeof(message));
    printf_P(PSTR("%lu: APP Received %x/%x from %u\n\r"),millis(),message.temp_reading,message.voltage_reading,header.from_node);
  }

  // If we are not the base, send sensor readings to the base
  if ( this_node > 0 )
  {
    int i;
    message_t message;
    
    // Take the temp reading 
    i = num_measurements;
    while(i--)
      message.temp_reading += analogRead(temp_sensor_pin); 
   
    // Take the voltage reading 
    i = num_measurements;
    while(i--)
      message.voltage_reading += analogRead(voltage_sensor_pin);

    printf_P(PSTR("---------------------------------\n\r"));
    printf_P(PSTR("%lu: APP Sending %x/%x to %u...\n\r"),millis(),message.temp_reading,message.voltage_reading,0);
    
    // Send it to the base
    RF24NetworkHeader header(/*to node*/ 0, /*type*/ 'S');
    bool ok = network.write(header,&message,sizeof(message));
    if (ok)
      printf_P(PSTR("%lu: APP Send ok\n\r"),millis());
    else
      printf_P(PSTR("%lu: APP Send failed\n\r"),millis());
     
    // Power down the radio.  Note that the radio will get powered back up
    // on the next write() call.
    radio.powerDown();

    // Be sure to flush the serial first before sleeping, so everything
    // gets printed properly
    Serial.flush();
    
    // Sleep the MCU.  The watchdog timer will awaken in a short while, and
    // continue execution here.
    Sleep.go();
  }

  // Listen for a new node address
  nodeconfig_listen();
}
// vim:ai:cin:sts=2 sw=2 ft=cpp
