/*
 Copyright (C) 2012 James Coliz, Jr. <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/**
 * Simplest possible example of using RF24Network,
 *
 * RECEIVER NODE
 * Listens for messages from the transmitter and prints them out.
 */

#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <Sleep_n0m1.h>


uint8_t key[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
uint8_t  iv[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

// nRF24L01(+) radio attached using Getting Started board
//Parameters
//    [in]	chipEnablePin	the Arduino pin to use to enable the chip for4 transmit/receive
//    [in]	chipSelectPin	the Arduino pin number of the output to use to select the NRF24 before accessing it 
RF24 radio(9,10);

// Network uses that radio
RF24Network network(radio);

// Address of our node
const uint16_t this_node = 0;

// Address of the other node
const uint16_t other_node = 1;

// // Structure of our payload
// struct payload_t
// {
//   unsigned long ms;
//   unsigned long counter;
//   char message[8];
// };

RF24NetworkHeader header;
// payload_t payload;

Sleep sleep;

char buffer[144];


void setup(void)
{
  Serial.begin(9600);
  Serial.println("RF24Network/examples/helloworld_rx/");

  SPI.begin();
  radio.begin();
  //radio.printDetails();
  network.begin(/*channel*/ 90, /*node address*/ this_node,key,iv);
  //network.begin(/*channel*/ 90, /*node address*/ this_node,NULL,NULL);
  delay(300);
}

void loop(void)
{

  //radio.maskIRQ(0,0,0);
  //radio.startListening();

  sleep.pwrDownMode(); //sets the Arduino into power Down Mode sleep, the most power saving, all systems are powered down except the watch dog timer and external reset
  //sleep.standbyMode(); //sets the Arduino into standby Mode sleep, this mode is identical to Power-down with the exception that the Oscillator is kept running for fast wake up

  //sleep.sleepDelay(sleepTime); //sleep for: sleepTime
  //sleep.sleepInterrupt(5,FALLING);
  //sleep.sleepInterrupt(5,LOW);

  //Serial.println("network update");
  //delay(300);

  // Pump the network regularly
  network.update();
  // Is there anything ready for us?
  while ( network.available() ){
      
    // If so, grab it and print it out
    //network.read(header,&payload,sizeof(payload));
    size_t size = network.readmulti(header,buffer,sizeof(buffer));

    if (size >0){

      Serial.print("received bytes:");
      Serial.println(size);

      buffer[size-1]='\0';
      Serial.print(" message: ");
      Serial.println(buffer);
      delay(100);
      
      //Serial.print(" Received packet #");
      //Serial.print(payload.counter);
      //Serial.print(" at ");
      //Serial.print(payload.ms);
      //Serial.print(" message: ");
      //Serial.println(payload.message);
    }
    network.update();
  }
}
