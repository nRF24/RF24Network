#!/usr/bin/env python

#
# Simplest possible example of using RF24Network,
#
#  TRANSMITTER NODE
#  Sends messages from to receiver.
#
from __future__ import print_function
import time
import struct
from RF24 import RF24
from RF24Network import RF24Network, RF24NetworkHeader


# generic Setup for GPIO 22 CE and GPIO 8 CSN
radio = RF24(22, 0)
network = RF24Network(radio)

# Address of our node in Octal format (01,021, etc)
this_node = 0o1

# Address of the other node
other_node = 0o0

#ms -  How long to wait before sending the next message
interval = 2000

radio.begin()
time.sleep(0.1)
network.begin(90, this_node)    # channel 90
# radio.printDetails()
radio.printPrettyDetails()
packets_sent = 0
last_sent = 0

while 1:
    network.update()
    now = time.monotonic_ns() / 1000
    # If it's time to send a message, send it!
    if now - last_sent >= interval:
        last_sent = now
        payload = struct.pack('<LL', time.monotonic_ns() / 1000, packets_sent)
        packets_sent += 1
        ok = network.write(RF24NetworkHeader(other_node), payload)
        print("Sending...", "ok." if ok else "failed.")
