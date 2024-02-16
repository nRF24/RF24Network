/*
 Update 2014 - TMRh20
 */

/**
 * Simplest possible example of using RF24Network,
 *
 * RECEIVER NODE
 * Listens for messages from the transmitter and prints them out.
 */

//#include <cstdlib>
#include <RF24/RF24.h>
#include <RF24Network/RF24Network.h>
#include <iostream>
#include <ctime>
#include <stdio.h>
#include <time.h>

using namespace std;

RF24 radio(22, 0); // (CE Pin, CSN Pin, [SPI Speed (in Hz)])

RF24Network network(radio);

// Address of our node in Octal format (01,021, etc)
const uint16_t this_node = 01;

// Address of the other node
const uint16_t other_node = 00;

// How often (in milliseconds) to send a message to the `other_node`
const unsigned long interval = 2000;

uint32_t last_sent;    // When did we last send?
uint32_t packets_sent; // How many have we sent already

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
        uint32_t now = millis(); // If it's time to send a message, send it!
        if (now - last_sent >= interval) {
            last_sent = now;

            printf("Sending ..\n");
            payload_t payload = {millis(), packets_sent++};
            RF24NetworkHeader header(/*to node*/ other_node);
            bool ok = network.write(header, &payload, sizeof(payload));
            printf("%s.\n", ok ? "ok" : "failed");
        }
    }

    return 0;
}
