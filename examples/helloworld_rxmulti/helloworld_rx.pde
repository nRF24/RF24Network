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
//#include <Sleep_n0m1.h>
#include <avr/sleep.h>

/*
Board	          int.0	    int.1	int.2	int.3	int.4	int.5
Uno, Ethernet	      2 	3	 	 	 	 
Mega2560	      	        	   21	   20	   19	   18
*/
const int interu = 5;
const int interupin = 18;

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

RF24NetworkHeader header;
// payload_t payload;

//Sleep sleep;

char buffer[144];

/*
void wait_receive()
{
  sleep_disable();
  detachInterrupt(interu);
}
*/

void setup()
{
  Serial.begin(9600);
  Serial.println("RF24Network/examples/helloworld_rx/");

  SPI.begin();
  radio.begin();
  //radio.printDetails();
  network.begin(/*channel*/ 93, /*node address*/ this_node,key,iv);
  //network.begin(/*channel*/ 93, /*node address*/ this_node,NULL,NULL);

  radio.maskIRQ(1,1,0);
  radio.powerUp();
  //sleep.pwrDownMode(); //sets the Arduino into power Down Mode sleep, the most power saving, all systems are powered down except the watch dog timer and external reset
  pinMode (interupin, INPUT);

}

void loop()
{

  Serial.println("sleep");
  delay(10);

  /*

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();

  // Do not interrupt before we go to sleep, or the
  // ISR will detach interrupts and we won't wake.
  noInterrupts ();   // same as cli();
  attachInterrupt(interu, wait_receive, LOW);
  //sleep_bod_disable(); // do not use for mega2560

  // We are guaranteed that the sleep_cpu call will be done
  // as the processor executes the next instruction after
  // interrupts are turned on.
  interrupts ();  // one cycle // same as sei();
  sleep_cpu ();   // one cycle
  sleep_disable();

  */

  network.sleep(interu,LOW,SLEEP_MODE_PWR_DOWN);

  // Pump the network regularly
  network.update();
  // Is there anything ready for us?
  while ( network.available() ){
      
  //   // If so, grab it and print it out
  //   //network.read(header,&payload,sizeof(payload));
    //size_t size = network.readmulti(header,buffer,sizeof(buffer));
    size_t size = network.readmulti(header,buffer,sizeof(buffer));

    if (size >0){
      //Serial.print("received bytes:");
      //Serial.println(size);
      //Serial.println(header.toString());
      buffer[size-1]='\0';
      Serial.print(" message: ");
      Serial.println(buffer);
      delay(200);
    }
    network.update();
  }
}
