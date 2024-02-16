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

// CE Pin, CSN Pin, SPI Speed (Hz)
RF24 radio(22, 0);

RF24Network network(radio);

// Address of our node in Octal format
const uint16_t this_node = 00;

// Address of the other node in Octal format (01, 021, etc)
const uint16_t other_node = 01;

struct payload_t
{ // Structure of our payload
    uint32_t ms;
    uint32_t counter;
};

int main(int argc, char** argv)
{
    // Refer to RF24 docs or nRF24L01 Datasheet for settings

    if (!radio.begin()) {
        printf("Radio hardware not responding!\n");
        return 0;
    }

    delay(5);
    radio.setChannel(90);
    network.begin(/*node address*/ this_node);
    radio.printDetails();

    while (1) {

        network.update();
        while (network.available()) { // Is there anything ready for us?

            RF24NetworkHeader header; // If so, grab it and print it out
            payload_t payload;
            network.read(header, &payload, sizeof(payload));

            printf("Received payload: counter=%u, origin timestamp=%u\n", payload.counter, payload.ms);
        }
        //sleep(2);
        delay(2000);
    }

    return 0;
}
