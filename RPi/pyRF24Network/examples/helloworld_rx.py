"""Simplest possible example of using RF24Network in RX role.
Listens for messages from the transmitter and prints them out.
"""
import struct
from RF24 import RF24
from RF24Network import RF24Network


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

# Address of our node in Octal format (01, 021, etc)
this_node = 0o0

# Address of the other node
other_node = 0o1

# initialize the radio
if not radio.begin():
    raise RuntimeError("radio hardware not responding")

radio.channel = 90

# initialize the network node
network.begin(this_node)

# radio.printDetails()
radio.printPrettyDetails()

radio.startListening()  # put radio in RX mode
try:
    while True:
        network.update()
        while network.available():
            header, payload = network.read(8)
            print("payload length ", len(payload))
            millis, number = struct.unpack("<LL", bytes(payload))
            print(
                f"Received payload {number} from {oct(header.from_node)}",
                f"to {oct(header.to_node)} at (origin's timestamp) {millis}",
            )
except KeyboardInterrupt:
    print("powering down radio and exiting.")
    radio.powerDown()
