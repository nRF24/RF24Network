#!/usr/bin/env python

#
# Simplest possible example of using RF24Network,
#
#  RECEIVER NODE
#  Listens for messages from the transmitter and prints them out.
#
from __future__ import print_function
import time
import struct
from RF24 import RF24
from RF24Network import RF24Network

# generic Setup for GPIO 22 CE and GPIO 8 CSN
radio = RF24(22, 0)
network = RF24Network(radio)

# Address of our node in Octal format (01, 021, etc)
this_node = 0o0

# Address of the other node
other_node = 0o1

radio.begin()
time.sleep(0.1)
network.begin(90, this_node)    # channel 90
# radio.printDetails()
radio.printPrettyDetails()
packets_sent = 0
last_sent = 0

while 1:
    network.update()
    while network.available():
        header, payload = network.read(8)
        print("payload length ", len(payload))
        ms, number = struct.unpack('<LL', bytes(payload))
        print('Received payload ', number, ' at ', ms, ' from ', oct(header.from_node))
    time.sleep(1)

