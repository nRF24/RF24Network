import time, struct
import RF24 as pyRF24
import RF24Network as pyRF24Network

this_node = [
    int(
        input("What is this node's logical address? ") or 0,
        8
    )
]

other_node = [
    int(
        input("What is the target node's logical address? ") or 1,
        8
    )
]

radio = pyRF24.RF24(22, 0)
if not radio.begin():
    print("radio mot respoding")

network = pyRF24Network.RF24Network(radio)
network.begin(90, this_node[0])

head_out = pyRF24Network.RF24NetworkHeader(other_node[0], 0)
head_in = pyRF24Network.RF24NetworkHeader()

def master(count=1, interval=1):
    for _ in range(count):
        msg = bytes(range(count))  # using dynamic payload sizes
        count -= 1
        print("message", msg, end=" ")
        if network.write(head_out, msg):
            print("sent successfully")
        else:
            print("failed to transmit")

def slave(timeout=10):
    end_timer = time.monotonic() + timeout
    while time.monotonic() <= end_timer:
        # let SERIAL_DEBUG output do the printing to stdout
        network.update()
    # let dev do post-reception work in REPL (using `head_in`)

def print_details():
    radio.printPrettyDetails()
    print("node address\t:", oct(this_node[0]))

print("""\n
    testing script for RF24Network python wrapper\n\n
    run `master(<msg_count>)` to transmit\n
    run `slave(<timeout_seconds>)` to receive"""
)
