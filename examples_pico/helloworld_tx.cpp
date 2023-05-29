/**
 * Simplest possible example of using RF24Network,
 *
 * TRANSMITTER NODE
 * Transmits messages to the reciever every 2 seconds.
 */
#include "pico/stdlib.h" // printf(), sleep_ms(), to_ms_since_boot(), get_absolute_time()
#include <tusb.h>        // tud_cdc_connected()
#include <RF24.h>        // RF24 radio object
#include <RF24Network.h> // RF24Network network object
#include "defaultPins.h" // board presumptive default pin numbers for CE_PIN and CSN_PIN

// instantiate an object for the nRF24L01 transceiver
RF24 radio(CE_PIN, CSN_PIN);

RF24Network network(radio);

// Address of our node in Octal format (01,021, etc)
const uint16_t this_node = 01;

// Address of the other node
const uint16_t other_node = 00;

// How often (in milliseconds) to send a message to the `other_node`
const unsigned long interval = 2000;

unsigned long last_sent;    // When did we last send?
unsigned long packets_sent; // How many have we sent already

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
    printf("RF24Network/examples_pico/helloworld_tx\n");

    // For debugging info
    // radio.printDetails();       // (smaller) function that prints raw register values
    // radio.printPrettyDetails(); // (larger) function that prints human readable data

    return true;
} // setup

void loop()
{
    network.update();
    unsigned long now = to_ms_since_boot(get_absolute_time());
    if (now - last_sent >= interval) { // If it's time to send a message, send it!
        last_sent = now;

        printf("Sending ..\n");
        payload_t payload = {now, packets_sent++};
        RF24NetworkHeader header(/*to node*/ other_node);
        bool ok = network.write(header, &payload, sizeof(payload));
        printf("%s.\n", ok ? "ok" : "failed");
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
