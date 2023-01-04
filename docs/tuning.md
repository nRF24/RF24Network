# Performance and Data Loss: Tuning the Network

@tableofcontents

<!-- markdownlint-disable MD031-->
Tips and examples for tuning the network and general operation.

Observe:

![@image html images/topologyImage.jpg width=70% height=70%](https://github.com/nRF24/RF24Network/raw/master/images/topologyImage.jpg)

## Understanding Radio Communication and Topology

When a transmission takes place from one radio module to another, the receiving radio will communicate
back to the sender with an acknowledgement (ACK) packet, to indicate success. If the sender does not
receive an ACK, the radio automatically engages in a series of timed retries, at set intervals. The
radios use techniques like addressing and numbering of payloads to manage this, but it is all done
automatically by the nrf chip, out of sight from the user.

When working over a radio network, some of these automated techniques can actually hinder data transmission to a degree.
Retrying failed payloads over and over on a radio network can hinder communication for nearby nodes, or
reduce throughput and errors on routing nodes.

Radios in this network are linked by **addresses** assigned to **pipes**. Each radio can listen
to 6 addresses on 6 pipes, therefore each radio has a parent pipe and 4-5 child pipes, which are used
to form a tree structure. Nodes communicate directly with their parent and children nodes. Any other
traffic to or from a node must be routed through the network.

## Topology of RF24Network

Anybody who is familiar at all with IP networking should be able to easily understand RF24Network topology. The
master node can be seen as the gateway, with up to 4 directly connected nodes. Each of those nodes creates a
subnet below it, with up to 4 additional child nodes. The numbering scheme can also be related to IP addresses,
for purposes of understanding the topology via subnetting. Nodes can have 5 children if multicast is disabled.

### Expressing RF24Network addresses in IP format

As an example, we could designate the master node in theory, as Address `10.10.10.10`

- The children nodes of the master would be `10.10.10.1`, `10.10.10.2`, `10.10.10.3`, `10.10.10.4` and `10.10.10.5`
- The children nodes of `10.10.10.1` would be `10.10.1.1`, `10.10.2.1`, `10.10.3.1`, `10.10.4.1` and `10.10.5.1`

In RF24Network, the master is just `00`

- Children of master are `01`, `02`, `03`, `04`, `05`
- Children of `01` are `011`, `021`, `031`, `041`, `051`

## Multicast

Multicast is enabled by default, which limits the master node to 5 child pipes and other nodes to 4. Nodes are
arranged in multicast 'levels' with the master node being level 0, nodes 01-05 are level 1, nodes n1-n5 are level 2,
and so on. The multicast level of each node can be configured as desired by the user, or multicast can be
disabled by editing RF24Network_config.h. For example, if all nodes are in range of the master node, all nodes can
be configured to use multicast level 1, allowing the master node to contact all of them by sending a single payload.
Multicasting is also used by the RF24Mesh layer for dynamic addressing requests.

## Routing

Routing of traffic is handled invisibly to the user, by the network layer. If the network addresses are
assigned in accordance with the physical layout of the network, nodes will route traffic automatically
as required. Users simply constuct a header containing the appropriate destination address, and the network
will forward it through to the correct node. Individual nodes only route individual fragments, so if using
fragmentation, routing nodes do not need it enabled, unless sending or receiving fragmented payloads themselves.

If routing data between parent and child nodes (marked by direct links on the topology image above) the network
uses built-in acknowledgement and retry functions of the chip to prevent data loss. When payloads are sent to
other nodes, they need to be routed. Routing is managed using a combination of built in ACK requests, and
software driven network ACKs. This allows all routing nodes to forward data very quickly, with only the final
routing node confirming delivery and sending back an
acknowledgement.

Example: Node 00 sends to node 01. The nodes will use the built in auto-retry and auto-ack functions.

Example: Node 00 sends to node 011. Node 00 will send to node 01 as before. Node 01 will forward the message
to 011. If delivery was successful, node 01 will also forward a message back to node 00, indicating success.

Old Functionality: Node 00 sends to node 011 using auto-ack. Node 00 first sends to 01, 01 acknowledges.
Node 01 forwards the payload to 011 using auto-ack. If the payload fails between 01 and 011, node 00 has
no way of knowing.

@note When retrying failed payloads that have been routed, there is a chance of duplicate payloads if the network-ack
is not successful. In this case, it is left up to the user to manage retries and filtering of duplicate payloads.

Acknowledgements can and should be managed by the application or user. If requesting a response from another node,
an acknowledgement is not required, so a user defined type of 0-64 should be used, to prevent the network from
responding with an acknowledgement. If not requesting a response, and wanting to know if the payload was successful
or not, users can utilize header types 65-127.

## Tuning Overview

The RF24 radio modules are generally only capable of either sending or receiving data at any given
time, but have built-in auto-retry mechanisms to prevent the loss of data. These values are adjusted
automatically by the library on startup, but can be further adjusted to reduce data loss, and
thus increase throughput of the network. This page is intended to provide a general overview of its
operation within the context of the network library, and provide guidance for adjusting these values.

## Auto-Retry Timing

The core radio library provides the functionality of adjusting the internal auto-retry interval of the
radio modules. In the network configuration, the radios can be set to automatically retry failed
transmissions at intervals ranging anywhere from 500us (0.5ms) up to 4000us (4ms). When operating any
number of radios larger than two, it is important to stagger the assigned intervals, to prevent the
radios from interfering with each other at the radio frequency (RF) layer.

The library should provide fairly good working values, as it simply staggers the assigned values within
groups of radios in direct communication. This value can be set manually by calling `radio.setRetries(X, 15);`
and adjusting the value of X from 1 to 15 (steps of 250us).

## Auto-Retry Count and Extended Timeouts

The core radio library also provides the ability to adjust the internal auto-retry count of the radio
modules. The default setting is 15 automatic retries per payload, and can be extended by configuring
the network.txTimeout variable. This default retry count should generally be left at 15, as per the
example in the above section. An interval/retry setting of (15,15) will provide 15 retrys at intervals of
4ms, taking up to 60ms per payload. The library now provides staggered timeout periods by default, but
they can also be adjusted on a per-node basis.

The txTimeout variable is used to extend the retry count to a defined duration in milliseconds. See the
network.txTimeout variable. Timeout periods of extended duration (500+) will generally not help when payloads
are failing due to data collisions, it will only extend the duration of the errors. Extended duration timeouts
should generally only be configured on leaf nodes that do not receive data.

## Scenarios

### Example 1

Network with master node and three leaf nodes that send data to the master node. None of the leaf
nodes need to receive data.

1. Master node uses default configuration
2. Leaf nodes can be configured with extended timeout periods to ensure reception by the master.
3. The following configuration will provide a reduction in errors, as the timeouts have been extended and are staggered
   between devices.
   ```text
   Leaf 01: network.txTimeout = 500;
   Leaf 02: network.txTimeout = 573;
   Leaf 03: network.txTimeout = 653;
   ```

### Example 2

Network with master node and three leaf nodes that send data to the master node. The second leaf
node needs to receive configuration data from the master at set intervals of 1 second, and send data back to the
master node. The other leaf nodes will send basic sensor information every few seconds, and a few dropped payloads
will not affect the operation greatly.

1. Master node configured with extended timeouts of 0.5 seconds, and increased retry delay:
   ```cpp
   radio.setRetries(11, 15);
   network.txTimeout = 500;
   ```
2. Second leaf node configured with a similar timeout period and retry delay:
   ```cpp
   radio.setRetries(8, 15);
   network.txTimeout = 553;
   ```
3. First and third leaf nodes configured with default timeout periods or slightly increased timout periods.
