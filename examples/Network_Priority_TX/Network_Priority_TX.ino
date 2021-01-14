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

#include <RF24.h>
#include <RF24Network.h>
#include "printf.h"

RF24 radio(7, 8);                   // nRF24L01(+) radio attached using Getting Started board

RF24Network network(radio);          // Network uses that radio

const uint16_t this_node = 01;       // Address of our node in Octal format
const uint16_t other_node = 00;      // Address of the other node in Octal format

uint8_t dataBuffer[33];

void setup() {

  Serial.begin(115200);
  if (!Serial) {
    // some boards need this because of native USB capability
  }
  Serial.println(F("RF24Network/examples/Network_Separation_TX/"));
  printf_begin(); //Used to enable printf on AVR devices

  if (!radio.begin()) {
    Serial.println(F("Radio hardware not responding!"));
    while (1) {
      // hold in infinite loop
    }
  }
  network.begin(/*channel*/ 90, /*node address*/ this_node);
  radio.printDetails();

  // Load our data buffer with numbered data
  for (uint16_t i = 0; i < 33; i++) {
    dataBuffer[i] = i;
  }

}//setup


uint32_t sendTimer = 0;

/*
 * The main loop sends two types of data to be processed with different priority per the RX
 * example
 */

void loop() {

  network.update();

  if (millis() - sendTimer > 1000) {
    sendTimer = millis();

    Serial.println(F("Sending data..."));

    // Sending of External data, which will be handled immediately
    RF24NetworkHeader header(other_node, EXTERNAL_DATA_TYPE);
    bool ok = network.write(header, &dataBuffer, 33);
    Serial.println(ok ? F("OK 1") : F("Fail 1"));

    // Sending normal user data, which may be buffered and handled later
    RF24NetworkHeader header2(other_node, 32);
    uint32_t someVariable = 1234;
    ok = network.write(header2, &someVariable, sizeof(someVariable));
    Serial.println(ok ? F("OK 2") : F("Fail 2"));
  }

  // Dummy operation to read 0 bytes from all incoming user payloads
  // Ensures the buffer doesnt fill up
  if (network.available()) {
    RF24NetworkHeader header;
    network.read(header, &dataBuffer, 0);
  }

}//loop
