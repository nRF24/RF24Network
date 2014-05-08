/*
 Copyright (C) 2012 James Coliz, Jr. <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/**
 * Simplest possible example of using RF24Network 
 *
 * TRANSMITTER NODE
 * Every 2 seconds, send a payload to the receiver node.
 */

#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>

uint8_t key[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
uint8_t  iv[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

// nRF24L01(+) radio attached using Getting Started board 
RF24 radio(9,10);

// Network uses that radio
RF24Network network(radio);

// Address of our node
const uint16_t this_node = 1;

// Address of the other node
const uint16_t other_node = 0;

// How often to send 'hello world to the other unit
const unsigned long interval = 2000; //ms

// When did we last send?
unsigned long last_sent;

// // How many have we sent already
// unsigned long packets_sent;

// // Structure of our payload
// struct payload_t
// {
//   unsigned long ms;
//   unsigned long counter;
//   char message[8];
// };

void setup(void)
{
  Serial.begin(9600);
  Serial.println("RF24Network/examples/helloworld_tx/");
 
  SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 93, /*node address*/ this_node, key, iv);
  //network.begin(/*channel*/ 93, /*node address*/ this_node, NULL,NULL);

  radio.setRetries(11,15);
  network.txTimeout=500;

}


void loop(void)
{
  // Pump the network regularly
  network.update();

  // If it's time to send a message, send it!
  unsigned long now = millis();
  if ( now - last_sent >= interval  )
  {
    last_sent = now;

    char message[]="messaggio molto lungo da inviare senza interruzone e senza problemi di sorta che verrà frammentato e ricomposto";

    Serial.println("Sending...");
    RF24NetworkHeader header( other_node,0);

    //payload_t payload = { millis(), packets_sent++ };
    //strcpy (payload.message,"msg bot");
    //bool ok = network.write(header,&payload,sizeof(payload));

    bool ok = network.writemulti(header,message,strlen(message)+1);

    if (ok)
      Serial.println("ok.");
    else
      Serial.println("failed.");

  }
}
// vim:ai:cin:sts=2 sw=2 ft=cpp
