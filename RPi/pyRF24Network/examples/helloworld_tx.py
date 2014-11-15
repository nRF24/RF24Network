#!/usr/bin/env python

#
# Simplest possible example of using RF24Network,
#
#  TRANSMITTER NODE
#  Sends messages from to receiver.
#

import time
from struct import *
from RF24 import *
from RF24Network import *

# CE Pin, CSN Pin, SPI Speed

# Setup for GPIO 22 CE and GPIO 25 CSN with SPI Speed @ 1Mhz
#radio = radio(RPI_V2_GPIO_P1_22, RPI_V2_GPIO_P1_18, BCM2835_SPI_SPEED_1MHZ)

# Setup for GPIO 22 CE and CE0 CSN with SPI Speed @ 4Mhz
#radio = RF24(RPI_V2_GPIO_P1_15, BCM2835_SPI_CS0, BCM2835_SPI_SPEED_4MHZ)

# Setup for GPIO 22 CE and CE1 CSN with SPI Speed @ 8Mhz
#radio = RF24(RPI_V2_GPIO_P1_15, BCM2835_SPI_CS0, BCM2835_SPI_SPEED_8MHZ)

# Setup for GPIO 22 CE and CE0 CSN for RPi B+ with SPI Speed @ 8Mhz
radio = RF24(RPI_BPLUS_GPIO_J8_22, RPI_BPLUS_GPIO_J8_24, BCM2835_SPI_SPEED_8MHZ)

network = RF24Network(radio)

# Address of our node in Octal format (01,021, etc)
this_node = 01

# Address of the other node
other_node = 00

interval = 2000     #ms -  How often to send 'hello world' to the other unit

millis = lambda: int(round(time.time() * 1000))

radio.begin()
time.sleep(0.1);
network.begin(90, this_node)    # channel 90
radio.printDetails()
packets_sent = 0
last_sent = 0
while 1:
    network.update()
    now = millis()              # If it's time to send a message, send it!
    if ( now - last_sent >= interval  ):
        last_sent = now
        print 'Sending ..',
        payload = pack('<qi', millis(), packets_sent )
        packets_sent += 1
        ok = network.write(RF24NetworkHeader(other_node), payload)
        if ok:
            print 'ok.'
        else:
            print 'failed.'
