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
 * TRANSMITTER NODE
 * Every X milliseconds, send a payload to the receiver node.
 */

#include "printf.h"
#include <RF24.h>
#include <RF24Network.h>

RF24 radio(7, 8);                   // nRF24L01(+) radio attached using Getting Started board

RF24Network network(radio);          // Network uses that radio

const uint16_t this_node = 01;       // Address of our node in Octal format
const uint16_t other_node = 00;      // Address of the other node in Octal format

const unsigned long interval = 500;  //ms  // How often to send 'hello world to the other unit

unsigned long last_sent;             // When did we last send?

/**** Create a large array for data to be sent ****
* MAX_PAYLOAD_SIZE is defined in RF24Network_config.h
* Payload sizes of ~1-2 KBytes or more are practical when radio conditions are good
*/
uint8_t dataBuffer[MAX_PAYLOAD_SIZE];

void setup(void) {
  Serial.begin(115200);
  if (!Serial) {
    // some boards need this because of native USB capability
  }
  Serial.println(F("RF24Network/examples/helloworld_tx_advanced/"));
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
  for (uint16_t i = 0; i < MAX_PAYLOAD_SIZE; i++) {
    dataBuffer[i] = i % 256; //Ensure the max value is 255
  }
}

uint16_t sizeofSend = 0; //Variable to indicate how much data to send
bool stopSending = 0;    //Used to stop/start sending of data

void loop() {

  //User input anything via Serial to stop/start data transmission
  if (Serial.available()) {
    Serial.read();
    stopSending = !stopSending;
  }

  network.update();                          // Check the network regularly

  unsigned long now = millis();              // If it's time to send a message, send it!
  if ( now - last_sent >= interval && !stopSending ) {
    last_sent = now;
    Serial.print("Sending size ");
    Serial.print(sizeofSend );

    // Fragmentation/reassembly is transparent. Just send payloads as usual.
    RF24NetworkHeader header(/*to node*/ other_node);
    bool ok = network.write(header, &dataBuffer, sizeofSend++);

    // If the size of data to be sent is larger than max payload size, reset at 0
    if (sizeofSend  > MAX_PAYLOAD_SIZE) {
      sizeofSend  = 0;
    }

    if (ok)
      Serial.println(" ok.");
    else
      Serial.println(" failed.");
  }
}
