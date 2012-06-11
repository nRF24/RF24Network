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
#include <Tictocs.h>
#include <Button.h>
#include <TictocTimer.h>
#include "nodeconfig.h"
#include "sleep.h"
#include "S_message.h"
#include "printf.h"

// This is for git version tracking.  Safe to ignore
#ifdef VERSION_H
#include "version.h"
#else
const char program_version[] = "Unknown";
#endif

// Pin definitions
#ifndef PINS_DEFINED
#define __PLATFORM__ "Getting Started board"

// Pins for radio
const int rf_ce = 9;
const int rf_csn = 10;

// Pins for sensors
const int temp_pin = A2;
const int voltage_pin = A3;

// Pins for status LED, or '0' for no LED connected
const int led_red = 0; 
const int led_yellow = 0; 
const int led_green = 0; 

// Button to control modes
const int button_a = 4;

// What voltage is a reading of 1023?
const unsigned voltage_reference = 5 * 256; // 5.0V
#endif

RF24 radio(rf_ce,rf_csn);
RF24Network network(radio);

// Our node address
uint16_t this_node;

// How many measurements to take.  64*1024 = 65536, so 64 is the max we can fit in a uint16_t.
const int num_measurements = 64;

// Sleep constants.  In this example, the watchdog timer wakes up
// every 4s, and every single wakeup we power up the radio and send
// a reading.  In real use, these numbers which be much higher.
// Try wdt_8s and 7 cycles for one reading per minute.> 1
const wdt_prescalar_e wdt_prescalar = wdt_4s;
const int sleep_cycles_per_transmission = 1;

// Non-sleeping nodes need a timer to regulate their sending interval
Timer send_timer(2000);

// Button controls functionality of the unit
Button ButtonA(button_a);

/**
 * Convenience class for handling LEDs.  Handles the case where the
 * LED may not be populated on the board, so always checks whether
 * the pin is valid before setting a value.
 */

class LED
{
private:
  int pin;
public:
  LED(int _pin): pin(_pin)
  {
    if (pin)
    {
      pinMode(pin,OUTPUT);
      digitalWrite(pin,LOW);
    }
  }
  void set(int state) const
  {
    if (pin)
      digitalWrite(pin,state);
  }

};

/**
 * Startup LED sequence.  Lights up the LEDs in sequence first, then dims 
 * them in the same sequence.
 */

class Startup: public Timer
{
private:
  const LED** leds;
  const LED** current;
  const LED** end;
  int state;
protected:
  virtual void onFired(void)
  {
    (*current)->set(state);
    ++current;
    if ( current >= end )
    {
      if ( state == HIGH )
      {
	state = LOW;
	current = leds;
      }
      else
	disable();
    }
  }
public:
  Startup(const LED** _leds, int _num): Timer(250), leds(_leds), current(_leds), end(_leds+_num), state(HIGH)
  {
  }
};

LED Red(led_red), Yellow(led_yellow), Green(led_green);

const LED* leds[] = { &Red, &Yellow, &Green }; 
const int num_leds = sizeof(leds)/sizeof(leds[0]);
Startup startup(leds,num_leds);

// Nodes in test mode do not sleep, but instead constantly try to send
bool test_mode = false;

void setup(void)
{
  //
  // Print preamble
  //
  
  Serial.begin(57600);
  printf_begin();
  printf_P(PSTR("\n\rRF24Network/examples/sensornet/\n\r"));
  printf_P(PSTR("PLATFORM: " __PLATFORM__ "\n\r"),program_version);
  printf_P(PSTR("VERSION: %s\n\r"),program_version);
  
  //
  // Pull node address out of eeprom 
  //

  // Which node are we?
  this_node = nodeconfig_read();

  //
  // Prepare sleep parameters
  //

  // Only the leaves sleep.  Nodes 01-05 are presumed to be relay nodes. 
  if ( this_node > 05 ) 
    Sleep.begin(wdt_prescalar,sleep_cycles_per_transmission);

  //
  // Set up board hardware
  //
  ButtonA.begin();

  // Sensors use the stable internal 1.1V voltage
#ifdef INTERNAL1V1
  analogReference(INTERNAL1V1);
#else
  analogReference(INTERNAL);
#endif

  //
  // Bring up the RF network
  //

  SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 92, /*node address*/ this_node);
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
    S_message message;
    network.read(header,&message,sizeof(message));
    printf_P(PSTR("%lu: APP Received #%u %s from 0%o\n\r"),millis(),header.id,message.toString(),header.from_node);
  }

  // If we are not the base, send sensor readings to the base
  send_timer.update();
  if ( this_node > 0 && ( Sleep || send_timer.wasFired() ) )
  {
    // Transmission beginning, TX LED ON
    Yellow.set(HIGH);
    if ( test_mode )
    {
      Green.set(LOW);
      Red.set(LOW);
    }

    int i;
    S_message message;
    
    // Take the temp reading 
    i = num_measurements;
    uint32_t reading = 0;
    while(i--)
      reading += analogRead(temp_pin);

    // Convert the reading to celcius*256
    // This is the formula for MCP9700.
    // C = reading * 1.1
    // C = ( V - 1/2 ) * 100
    message.temp_reading = ( ( ( reading * 0x120 ) - 0x800000 ) * 0x64 ) >> 16;

    // Take the voltage reading 
    i = num_measurements;
    reading = 0;
    while(i--)
      reading += analogRead(voltage_pin);

    // Convert the voltage reading to volts*256
    message.voltage_reading = ( reading * voltage_reference ) >> 16; 

    printf_P(PSTR("---------------------------------\n\r"));
    printf_P(PSTR("%lu: APP Sending %s to 0%o...\n\r"),millis(),message.toString(),0);
    
    // Send it to the base
    RF24NetworkHeader header(/*to node*/ 0, /*type*/ 'S');
    bool ok = network.write(header,&message,sizeof(message));
    if (ok)
    {
      if ( test_mode )
	Green.set(HIGH);
      printf_P(PSTR("%lu: APP Send ok\n\r"),millis());
    }
    else
    {
      if ( test_mode )
	Red.set(HIGH);
      printf_P(PSTR("%lu: APP Send failed\n\r"),millis());
    }

    // Transmission complete, TX LED OFF
    Yellow.set(LOW);
   
    if ( Sleep && ! test_mode )
    {
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
  }

  // Button
  ButtonA.update();
  if ( ButtonA.wasPressed() )
  {
    // Pressing the button during startup sequences engages test mode.
    // Pressing it after turns off test mode.
    if ( startup )
      test_mode = true;
    else
      if ( test_mode )
      {
	test_mode = false;
	Green.set(LOW);
	Red.set(LOW);
      }
  }

  // Continue the startup sequence
  startup.update();

  // Listen for a new node address
  nodeconfig_listen();
}
// vim:ai:cin:sts=2 sw=2 ft=cpp
