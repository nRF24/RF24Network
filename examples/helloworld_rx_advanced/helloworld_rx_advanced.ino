/**
 * Copyright (C) 2020 TMRh20(tmrh20@gmail.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 */

/**
 * More advanced example of using RF24Network:
 * Fragmentation and Reassembly:
 * - nrf24l01+ radios can tx/rx 32 bytes of data per transmission
 * - RF24Network will fragment and re-assemble payloads of any size
 * Demonstrates use of differing sized payloads using peek() function
 *
 * RECEIVER NODE
 * Every X milliseconds, send a payload to the receiver node.
 */

#include "printf.h"
#include <RF24.h>
#include <RF24Network.h>

RF24 radio(7, 8);  // nRF24L01(+) radio attached using Getting Started board

RF24Network network(radio);      // Network uses that radio
const uint16_t this_node = 00;   // Address of our node in Octal format ( 04,031, etc)
const uint16_t other_node = 01;  // Address of the other node in Octal format

/**** Create a large array for data to be received ****
* MAX_PAYLOAD_SIZE is defined in RF24Network_config.h
* Payload sizes of ~1-2 KBytes or more are practical when radio conditions are good
*/
uint8_t dataBuffer[MAX_PAYLOAD_SIZE];  //MAX_PAYLOAD_SIZE is defined in RF24Network_config.h


void setup(void) {

  Serial.begin(115200);
  while (!Serial) {
    // some boards need this because of native USB capability
  }
  Serial.println(F("RF24Network/examples/helloworld_rx_advanced/"));

  if (!radio.begin()) {
    Serial.println(F("Radio hardware not responding!"));
    while (1) {
      // hold in infinite loop
    }
  }
  radio.setChannel(90);
  network.begin(/*node address*/ this_node);

  printf_begin();        // needed for RF24* libs' internal printf() calls
  radio.printDetails();  // requires printf support
}

// Variable for calculating how long between RX
uint32_t timeBetweenPackets = 0;

void loop(void) {

  network.update();  // Check the network regularly

  while (network.available()) {  // Is there anything ready for us?

    RF24NetworkHeader header;                        // If so, grab it and print it out
    uint16_t payloadSize = network.peek(header);     // Use peek() to get the size of the payload
    network.read(header, &dataBuffer, payloadSize);  // Get the data
    Serial.print("Received packet, size ");          // Print info about received data
    Serial.print(payloadSize);
    Serial.print("(");
    Serial.print(millis() - timeBetweenPackets);
    Serial.println("ms since last)");
    timeBetweenPackets = millis();

    // Uncomment below to print the entire payload
    /*
    for(uint32_t i = 0; i < payloadSize; i++) {
      Serial.print(dataBuffer[i]);
      Serial.print(F(": "));
      if(i % 50 == 49) {
        //Add a line break every 50 characters
        Serial.println();
      }
    }
    Serial.println();
    */
  }
}
