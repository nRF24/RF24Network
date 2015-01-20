/*
 Update 2014 - TMRh20
 */

/**
 * Simplest possible example of using RF24Network,
 *
 * RECEIVER NODE
 * Listens for messages from the transmitter and prints them out.
 */

#include <RF24/RF24.h>
#include <RF24Network/RF24Network.h>
#include <iostream>
#include <ctime>
#include <stdio.h>
#include <time.h>


/**
 * g++ -L/usr/lib main.cc -I/usr/include -o main -lrrd
 **/
//using namespace std;

// CE Pin, CSN Pin, SPI Speed

// Setup for GPIO 22 CE and GPIO 25 CSN with SPI Speed @ 1Mhz
//RF24 radio(RPI_V2_GPIO_P1_22, RPI_V2_GPIO_P1_18, BCM2835_SPI_SPEED_1MHZ);

// Setup for GPIO 22 CE and CE0 CSN with SPI Speed @ 4Mhz
//RF24 radio(RPI_V2_GPIO_P1_15, BCM2835_SPI_CS0, BCM2835_SPI_SPEED_4MHZ); 

// Setup for GPIO 22 CE and CE1 CSN with SPI Speed @ 8Mhz
RF24 radio(RPI_V2_GPIO_P1_15, BCM2835_SPI_CS0, BCM2835_SPI_SPEED_8MHZ);  

RF24Network network(radio);

// Address of our node in Octal format
const uint16_t this_node = 00;

// Address of the other node in Octal format (01,021, etc)
const uint16_t other_node = 01;

const unsigned long interval = 2000; //ms  // How often to send 'hello world to the other unit

unsigned long last_sent;             // When did we last send?
unsigned long packets_sent;          // How many have we sent already


struct payload_t {                  // Structure of our payload
  unsigned long ms;
  unsigned long counter;
};

int main(int argc, char** argv) 
{
	// Refer to RF24.h or nRF24L01 DS for settings

	radio.begin();
	
	delay(5);
	network.begin(/*channel*/ 90, /*node address*/ this_node);
	radio.printDetails();
	
	while(1)
	{

		  network.update();
  		  while ( network.available() ) {     // Is there anything ready for us?
    			
		 	RF24NetworkHeader header;        // If so, grab it and print it out
   			 payload_t payload;
  			 network.read(header,&payload,sizeof(payload));
			
			printf("Received payload # %lu at %lu \n",payload.counter,payload.ms);
  }		  
		 //sleep(2);
		 delay(2000);
		 //fclose(pFile);
	}

	return 0;
}

