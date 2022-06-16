"""Simplest possible example of using RF24Network in RX role.
Sends messages from to receiver.
"""
import time
import struct
from RF24 import RF24
from RF24Network import RF24Network, RF24NetworkHeader


########### USER CONFIGURATION ###########
# See https://github.com/TMRh20/RF24/blob/master/pyRF24/readme.md
# Radio CE Pin, CSN Pin, SPI Speed
# CE Pin uses GPIO number with BCM and SPIDEV drivers, other platforms use
# their own pin numbering
# CS Pin addresses the SPI bus number at /dev/spidev<a>.<b>
# ie: RF24 radio(<ce_pin>, <a>*10+<b>); spidev1.0 is 10, spidev1.1 is 11 etc..

# Generic:
radio = RF24(22, 0)
################## Linux (BBB,x86,etc) #########################
# See http://nRF24.github.io/RF24/pages.html for more information on usage
# See http://iotdk.intel.com/docs/master/mraa/ for more information on MRAA
# See https://www.kernel.org/doc/Documentation/spi/spidev for more
# information on SPIDEV

# instantiate the network node using `radio` object
network = RF24Network(radio)

# Address of our node in Octal format (01,021, etc)
this_node = 0o1

# Address of the other node
other_node = 0o0

# How long to wait before sending the next message
interval = 2000  # in milliseconds

# initialize the radio
if not radio.begin():
    raise RuntimeError("radio hardware not responding")

radio.channel = 90

# initialize the network node
network.begin(this_node)

# radio.printDetails()
radio.printPrettyDetails()
packets_sent = 0
last_sent = 0

try:
    while True:
        network.update()
        now = int(time.monotonic_ns() / 1000000)
        # If it's time to send a message, send it!
        if now - last_sent >= interval:
            last_sent = now
            packets_sent += 1
            payload = struct.pack("<LL", now, packets_sent)
            ok = network.write(RF24NetworkHeader(other_node), payload)
            print(f"Sending {packets_sent}...", "ok." if ok else "failed.")
except KeyboardInterrupt:
    print("powering down radio and exiting.")
    radio.powerDown()
