# Network Layer for RF24 Radios

@tableofcontents

This class implements an [OSI Network Layer](http://en.wikipedia.org/wiki/Network_layer) using nRF24L01(+) radios driven
by the newly optimized [RF24 library fork](http://nRF24.github.com/RF24/) or using nRF52x radios with the newly created 
[nrf_to_nrf library](https://github.com/TMRh20/nrf_to_nrf).

@see
[RF24 Library docs](http://nRF24.github.io/RF24/) for general RF24 configuration and setup.
    - [Linux Installation](https://nrf24.github.io/RF24/md_docs_2linux__install.html) and [General Linux/RPi configuration and setup](https://nrf24.github.io/RF24/md_docs_2rpi__general.html) documentation

## Purpose/Goal

Original: Create an alternative to ZigBee radios for Arduino communication.

New: Enhance the current functionality for maximum efficiency, reliability, and speed

Xbees are excellent little radios, backed up by a mature and robust standard
protocol stack. They are also expensive.

For many Arduino uses, they seem like overkill. So I am working to improve the current
standard for nRF24L01 radios. The best RF24 modules are available for less than
$6 from many sources. With the RF24Network layer, I hope to cover many
common communication scenarios.

Please see [TMRh20's blog post](https://tmrh20.blogspot.com/2019/05/comparative-performance-analysis.html)
for a comparison against the ZigBee protocols

## News - 2023 API Changes
Introducing **RF24Network & RF24Mesh v2.0** with some *significant API changes*, adding the use of [C++ Templates](https://cplusplus.com/doc/oldtutorial/templates/)
in order to support a range of ESB enabled radios, most recently NRF52x radios.

**Important Notes:**
- Any network layer that uses v2 needs to have RF24Network/RF24Mesh dependencies of v2 or newer. RF24 v1.x is an exception here.
- General usage should remain backward compatible, see the included examples of the related libraries for more info
- Any third party libs that extend the network/mesh layer may also need to be updated to incorporate the new templated class prototypes:
```cpp
template<class radio_t>
class ESBNetwork;
  
template<class network_t, class radio_t>
class ESBMesh;
```
- Third party libs should also be able to use the backward-compatible typedef in their template:
  - ESBGateway.h:
  ```cpp
  template<typename network_t, typename mesh_t>
  class ESBGateway
  ```
  and inform the compiler what types they intend to support:
  - ESBGateway.cpp:
  ```cpp
  template class ESBGateway<RF24Network, RF24Mesh>;
  ```  
- The auto installers do not perform a version check like package managers, so having the correct versions of the software is important.
- We *will* be maintaining the v1.x versions with bugfixes etc for those who cannot or do not wish to migrate to the newer template approach.

Please see the recent changes listed in [the github releases page](https://github.com/nRF24/RF24Network/releases)

## Features

### The layer provides

- Network ACKs: Efficient acknowledgement of network-wide transmissions, via dynamic radio acks and network protocol acks.
- Updated addressing standard for optimal radio transmission.
- Extended timeouts and staggered timeout intervals. The new txTimeout variable allows fully automated extended timeout periods via auto-retry/auto-reUse of payloads.
- Optimization to the core library provides improvements to reliability, speed and efficiency. See [RF24 library documentation](https://nRF24.github.io/RF24) for more info.
- Built in sleep mode using interrupts. (Still under development -- enable via RF24Network_config.h)
- Host Addressing. Each node has a logical address on the local network.
- Message Forwarding. Messages can be sent from one node to any other, and
  this layer will get them there no matter how many hops it takes.
- Ad-hoc Joining. A node can join a network without any changes to any
  existing nodes.

### The layer does not provide

- Dynamic address assignment (See [RF24Mesh](https://github.com/nRF24/RF24Mesh))
- Layer 4 protocols (TCP/IP - See [RF24Ethernet](https://github.com/nRF24/RF24Ethernet) and [RF24Gateway](https://github.com/nRF24/RF24Gateway))

## How to learn more

- [RF24Network Class Documentation](classRF24Network.html)
- [Advanced Configuration Options](advanced_config.md)
- [Addressing format](addressing.md)
- [Topology and Overview](tuning.md)
- [Examples Page](examples.html). Start with `helloworld_*` examples.

### Additional Information & Add-ons

- [RF24Mesh: Dynamic Mesh Layer for RF24Network](https://github.com/nRF24/RF24Mesh)
- [RF24Ethernet: TCP/IP over RF24Network](https://github.com/nRF24/RF24Ethernet)
- [TMRh20's Blog: RF24 Optimization Overview](http://tmrh20.blogspot.com/2014/03/high-speed-data-transfers-and-wireless.html)
- [TMRh20's Blog: RF24 Wireless Audio](http://tmrh20.blogspot.com/2014/03/arduino-radiointercomwireless-audio.html)
- [RF24: Original Author](http://maniacbug.github.com/RF24/)

## Topology for Mesh Networks using nRF24L01(+)

This network layer takes advantage of the fundamental capability of the nRF24L01(+) radio to
listen actively to up to 6 other radios at once (8 with NRF52x). The network is arranged in a
[Tree Topology](http://en.wikipedia.org/wiki/Network_Topology#Tree), where
one node is the base, and all other nodes are children either of that node, or of another.
Unlike a true mesh network, multiple nodes are not connected together, so there is only one
path to any given node.

## Octal Addressing and Topology

Each node must be assigned an 15-bit address by the administrator. This address exactly
describes the position of the node within the tree. The address is an octal number. Each
digit in the address represents a position in the tree further from the base.

- Node 00 is the base node.
- Nodes 01-05 are nodes whose parent is the base.
- Node 021 is the second child of node 01.
- Node 0321 is the third child of node 021, an so on.
- The largest node address is 05555, so up to 781 nodes are allowed on a single channel.
  An example topology is shown below, with 5 nodes in direct communication with the master node,
  and multiple leaf nodes spread out at a distance, using intermediate nodes to reach other nodes.
- With the newer NRF52x devices, up to 3200 nodes are allowed on a single channel

@image html example_tree.svg

## How routing is handled

When sending a message using ESBNetwork::write(), you fill in the header with the logical
node address. The network layer figures out the right path to find that node, and sends
it through the system until it gets to the right place. This works even if the two nodes
are far separated, as it will send the message down to the base node, and then back out
to the final destination.

All of this work is handled by the ESBNetwork::update() method, so be sure to call it
regularly or your network will miss packets.

## Starting up a node

When a node starts up, it only has to contact its parent to establish communication.
No direct connection to the Base node is needed. This is useful in situations where
relay nodes are being used to bridge the distance to the base, so leaf nodes are out
of range of the base.

## Directionality

By default all nodes are always listening, so messages will quickly reach
their destination.

You may choose to sleep any nodes on the network if using interrupts. This is useful in a
case where the nodes are operating on batteries and need to sleep. This greatly decreases
the power requirements for a sensor network. The leaf nodes can sleep most of the time,
and wake every few minutes to send in a reading. Routing nodes can be triggered to wake up
whenever a payload is received See ESBNetwork::sleepNode() in the class documentation, and RF24Network_config.h
to enable sleep mode.
