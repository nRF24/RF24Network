/**
 * Copyright (C) 2012 James Coliz, Jr. <maniacbug@ymail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * Update 2014 - TMRh20
 */

/**
 * Simplest possible example of using RF24Network,
 *
 * RECEIVER NODE
 * Listens for messages from the transmitter and prints them out.
 */

#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>


RF24 radio(7, 8);  // nRF24L01(+) radio attached using Getting Started board

RF24Network network(radio);      // Network uses that radio
const uint16_t this_node = 00;   // Address of our node in Octal format (04, 031, etc)
const uint16_t other_node = 01;  // Address of the other node in Octal format

struct payload_t {  // Structure of our payload
  unsigned long ms;
  unsigned long counter;
};


void setup(void) {
  Serial.begin(115200);
  while (!Serial) {
    // some boards need this because of native USB capability
  }
  Serial.println(F("RF24Network/examples/helloworld_rx/"));

  if (!radio.begin()) {
    Serial.println(F("Radio hardware not responding!"));
    while (1) {
      // hold in infinite loop
    }
  }
  radio.setChannel(90);
  network.begin(/*node address*/ this_node);
}

void loop(void) {

  network.update();  // Check the network regularly

  while (network.available()) {  // Is there anything ready for us?

    RF24NetworkHeader header;  // If so, grab it and print it out
    payload_t payload;
    network.read(header, &payload, sizeof(payload));
    Serial.print(F("Received packet: counter="));
    Serial.print(payload.counter);
    Serial.print(F(", origin timestamp="));
    Serial.println(payload.ms);
  }
}
