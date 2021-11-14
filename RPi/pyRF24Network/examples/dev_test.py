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
    raise RuntimeError("radio hardware not responding")

radio.channel = 90

network = pyRF24Network.RF24Network(radio)
network.begin(this_node[0])

header = pyRF24Network.RF24NetworkHeader(other_node[0], 0)

def master(count=1, interval=1):
    for _ in range(count):
        msg = bytes(range(count))  # using dynamic payload sizes
        count -= 1
        print("message", msg, end=" ")
        header = pyRF24Network.RF24NetworkHeader(other_node[0], 0)
        if network.write(header, msg):
            print("sent successfully")
        else:
            print("failed to transmit")

def slave(timeout=10):
    end_timer = time.monotonic() + timeout
    while time.monotonic() <= end_timer:
        # let SERIAL_DEBUG output do the printing to stdout
        network.update()
    # let dev do post-reception work in REPL (using `header`)

def print_details():
    radio.printPrettyDetails()
    print("node address\t\t=", oct(this_node[0]))

print("""
    testing script for RF24Network python wrapper\n
    run `master(<msg_count>)` to transmit
    run `slave(<timeout_seconds>)` to receive"""
)
