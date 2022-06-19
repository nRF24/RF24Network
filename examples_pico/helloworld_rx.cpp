/**
 * Simplest possible example of using RF24Network,
 *
 * RECEIVER NODE
 * Listens for messages from the transmitter and prints them out.
 */
#include "pico/stdlib.h" // printf(), sleep_ms(), to_ms_since_boot(), get_absolute_time()
#include <tusb.h>        // tud_cdc_connected()
#include <RF24.h>        // RF24 radio object
#include <RF24Network.h> // RF24Network network object
#include "defaultPins.h" // board presumptive default pin numbers for CE_PIN and CSN_PIN

// instantiate an object for the nRF24L01 transceiver
RF24 radio(CE_PIN, CSN_PIN);

RF24Network network(radio);

// Address of our node in Octal format
const uint16_t this_node = 00;

// Address of the other node in Octal format (01, 021, etc)
const uint16_t other_node = 01;

struct payload_t
{ // Structure of our payload
    unsigned long ms;
    unsigned long counter;
};

bool setup()
{
    // wait here until the CDC ACM (serial port emulation) is connected
    while (!tud_cdc_connected()) {
        sleep_ms(10);
    }

    // initialize the transceiver on the SPI bus
    if (!radio.begin()) {
        printf("radio hardware is not responding!!\n");
        return false;
    }

    radio.setChannel(90);
    network.begin(/*node address*/ this_node);

    // print example's introductory prompt
    printf("RF24Network/examples_pico/helloworld_rx\n");

    // For debugging info
    // radio.printDetails();       // (smaller) function that prints raw register values
    // radio.printPrettyDetails(); // (larger) function that prints human readable data

    return true;
} // setup

void loop()
{
    network.update();
    while (network.available()) { // Is there anything ready for us?
        // If so, grab it and print it out
        RF24NetworkHeader header;
        payload_t payload;
        network.read(header, &payload, sizeof(payload));

        printf("Received payload: counter=%lu, origin timestamp=%lu\n", payload.counter, payload.ms);
    }
}

int main()
{
    stdio_init_all(); // init necessary IO for the RP2040

    while (!setup()) { // if radio.begin() failed
        // hold program in infinite attempts to initialize radio
    }
    while (true) {
        loop();
    }
    return 0; // we will never reach this
}
