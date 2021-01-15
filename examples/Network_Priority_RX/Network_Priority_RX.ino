/**
 * Copyright (C) 2020 TMRh20(tmrh20@gmail.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 */

/**
 * This sketch demonstrates handling of external data
 *
 * RF24Network contains a buffer for storing user payloads that have been received via the network.update()
 * function. If using protocols like TCP/IP over RF24Network, the memory on small devices is very limited.
 * Instead of using the user-payload buffer for such large payloads, they can be designated as an
 * EXTERNAL_DATA_TYPE in the header.type field. This allows users to prioritize these payloads, as they are
 * often very large, and would take up most or all of the user data buffer.
 *
 * The network.update function will return immediately upon receiving a payload marked as EXTERNAL_DATA_TYPE
 * Users can then process the data immediately.
 * All other payload types are handled via the network.available() and network.read() functionality.
 *
 * Functionality:
 * The TX node will send normal user data designated with header.type = 33, along with additional data
 * marked as header.type = EXTERNAL_DATA_TYPE.
 * The RX node demonstrates how to handle such data, allowing separation of standard data that is processed
 * normally vs data that needs to be passed elsewhere, like network interface for TCP/IP packets.
 * These methods are used in RF24Gateway & RF24Ethernet TCP/IP libraries for nrf24l01+.
 */

#include "printf.h"
#include <RF24.h>
#include <RF24Network.h>

RF24 radio(7, 8);                   // nRF24L01(+) radio attached using Getting Started board

RF24Network network(radio);          // Network uses that radio

const uint16_t this_node = 00;       // Address of our node in Octal format
const uint16_t other_node = 01;      // Address of the other node in Octal format

uint32_t myVariable = 0;

void setup() {

  Serial.begin(115200);
  if (!Serial) {
    // some boards need this because of native USB capability
  }
  printf_begin(); //Used to enable printf on AVR devices
  if (!Serial) {
    // some boards need this because of native USB capability
  }
  Serial.println(F("RF24Network/examples/Network_Separation_RX/"));

  if (!radio.begin()) {
    Serial.println(F("Radio hardware not responding!"));
    while (1) {
      // hold in infinite loop
    }
  }
  network.begin(/*channel*/ 90, /*node address*/ this_node);
  radio.printDetails();

}//setup


uint32_t sendTimer = 0;

/**** Create a large array for data to be received ****
* MAX_PAYLOAD_SIZE is defined in RF24Network_config.h
* Payload sizes of ~1-2 KBytes or more are practical when radio conditions are good
*/
#define EXTERNAL_DATA_MAX_SIZE  MAX_PAYLOAD_SIZE

uint8_t dataBuffer[EXTERNAL_DATA_MAX_SIZE];

uint32_t userDataTimer = 0;


/*
 * The main loop behaviour demonstrates the different prioritization of handling data
 * External data is handled immediately upon reception, with the network.update() function being
 * called very regularly to handle incoming/outgoing radio traffic.
 *
 * The network.available() function is only called every 5 seconds, to simulate a busy microcontroller,
 * so the user payloads will only print out every 5 seconds
 *
 * The radio has 3, 32-byte FIFO buffers operating independantly of the MCU, and RF24Network will buffer
 * up to MAX_PAYLOAD_SIZE (see RF24Network_config.h) of user data.
 */
void loop() {

  // Immediate handling of data with header type EXTERNAL_DATA_TYPE

  if (network.update() == EXTERNAL_DATA_TYPE) {
    uint16_t size = network.frag_ptr->message_size;
    memcpy(&dataBuffer, network.frag_ptr->message_buffer, network.frag_ptr->message_size);

    // Handle the external data however...
    Serial.print("External Data RX, size: ");
    Serial.println(network.frag_ptr->message_size);

    for (uint16_t i = 0; i < network.frag_ptr->message_size; i++) {
      Serial.print(dataBuffer[i]);
      Serial.print(":");
    }
    Serial.println();
  }


  // Use a timer to simulate a busy MCU where normal network data cannot be processed in a timely manner
  if (millis() - userDataTimer > 5000) {
    userDataTimer = millis();

    // Handling of standard RF24Network User Data
    while (network.available()) {

      RF24NetworkHeader header;                                    // Create an empty header
      uint16_t dataSize = network.peek(header);                    // Peek to get the size of the data
      uint32_t someVariable;
      if (header.type = '32') {                                    // If a certain header type is recieved
        network.read(header, &someVariable, sizeof(someVariable)); // Handle the data a specific way
        Serial.print("RX User Data:\nHeader Type ");
        Serial.print(header.type);
        Serial.print(" Value ");
        Serial.println(someVariable);
      } else {
        network.read(header, &someVariable, 0);                    // Clear the user data from the buffer if
        // some other header type is received
      }
    }
  }
}//loop
