"""
A simple example of sending data from 1 nRF24L01 transceiver to another.
This example was written to be used on 2 devices acting as 'nodes'.
"""
import sys
import argparse
import time
import struct
from RF24 import RF24, RF24_PA_LOW
from RF24Network import RF24Network, RF24NetworkHeader


parser = argparse.ArgumentParser(
    description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter
)
parser.add_argument(
    "-n", "--node", help="the identifying network node address (a 2-byte octal number)"
)
parser.add_argument(
    "-r",
    "--role",
    type=int,
    choices=range(2),
    help="'1' specifies the TX role. '0' specifies the RX role.",
)


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

# instantiate the network node using the radio object
network = RF24Network(radio)

# using the python keyword global is bad practice. Instead we'll use a 1 item
# list to store our float number for the payloads sent/received
packets_sent = [0]


def master(radio_number):
    """Transmits 2 incrementing long ints every 2 second"""
    failures = 0
    while failures < 6:
        network.update()
        now = time.monotonic()
        last_sent = now + 2
        # If it's time to send a message, send it!
        if now - last_sent >= 2:
            last_sent = now
            ok = network.write(
                RF24NetworkHeader(not bool(radio_number % 8)),
                struct.pack("<LL", time.monotonic_ns() / 1000, packets_sent[0]),
            )
            packets_sent[0] += 1
            failures += not ok
            print("Sending %d..." % packets_sent[0], "ok." if ok else "failed.")
    print(failures, "failures detected. Leaving TX role.")


def slave(timeout=6):
    """Listen for any payloads and print the transaction

    :param int timeout: The number of seconds to wait (with no transmission)
        until exiting function.
    """
    radio.startListening()  # put radio in RX mode

    start_timer = time.monotonic()
    while (time.monotonic() - start_timer) < timeout:
        network.update()
        while network.available():
            header, payload = network.read(8)
            print("payload length ", len(payload))
            millis, number = struct.unpack("<LL", bytes(payload))
            print(
                "Received payload ", number,
                "at", millis,
                "from", oct(header.from_node),
                "to", oct(header.to_node),
            )
            start_timer = time.monotonic()  # reset timer
        time.sleep(0.05)  # wait 50 ms


def set_role(radio_number):
    """Set the role using stdin stream. Timeout arg for slave() can be
    specified using a space delimiter (e.g. 'R 10' calls `slave(10)`)

    :return:
        - True when role is complete & app should continue running.
        - False when app should exit
    """
    user_input = (
        input(
            "*** Enter 'R' for receiver role.\n"
            "*** Enter 'T' for transmitter role.\n"
            "*** Enter 'Q' to quit example.\n"
        )
        or "?"
    )
    user_input = user_input.split()
    if user_input[0].upper().startswith("R"):
        if len(user_input) > 1:
            slave(int(user_input[1]))
        else:
            slave()
        return True
    elif user_input[0].upper().startswith("T"):
        master(radio_number)
        return True
    elif user_input[0].upper().startswith("Q"):
        radio.powerDown()
        return False
    print(user_input[0], "is an unrecognized input. Please try again.")
    return set_role(radio_number)


if __name__ == "__main__":

    args = parser.parse_args()  # parse any CLI args

    # initialize the nRF24L01 on the spi bus
    if not radio.begin():
        raise RuntimeError("radio hardware is not responding")

    print(sys.argv[0])  # print example name

    # to use different addresses on a pair of radios, we need a variable to
    # uniquely identify which address this radio will use to transmit
    # 0 uses address[0] to transmit, 1 uses address[1] to transmit
    radio_number = args.node  # uses default value from `parser`
    if args.node is None:  # if '--node' arg wasn't specified
        radio_number = int(
            input("Which radio is this? Enter '0' or '1'. Defaults to '0' ") or "0",
            8,  # octal base
        )

    # set the Power Amplifier level to -12 dBm since this test example is
    # usually run with nRF24L01 transceivers in close proximity of each other
    radio.setPALevel(RF24_PA_LOW)  # RF24_PA_MAX is default

    # initialize the network node using channel=90, `radio_number` as
    # `network.node_address`
    network.begin(90, radio_number)

    # for debugging, we have 2 options that print a large block of details
    # (smaller) function that prints raw register values
    # radio.printDetails()
    # (larger) function that prints human readable data
    radio.printPrettyDetails()

    try:
        if args.role is None:  # if not specified with CLI arg '-r'
            while set_role(radio_number):
                pass  # continue example until 'Q' is entered
        else:  # if role was set using CLI args
            # run role once and exit
            master(radio_number) if bool(args.role) else slave(radio_number)
    except KeyboardInterrupt:
        print(" Keyboard Interrupt detected. Exiting...")
        radio.powerDown()
        sys.exit()
