# Comparison to ZigBee

@tableofcontents

This network layer is influenced by the design of ZigBee, but does not implement it
directly.

## Which is better?

ZigBee is a much more robust, feature-rich set of protocols, with many different vendors
providing compatible chips.

RF24Network is cheap. While ZigBee radios are well over $20, nRF24L01 modules can be found
for under $2.

## Similiarities & Differences

Here are some comparisons between RF24Network and ZigBee.

- Both networks support Star and Tree topologies. Only Zigbee supports a true mesh.
- In ZigBee networks, only leaf nodes can sleep
- ZigBee nodes are configured using AT commands, or a separate Windows application.
  RF24 nodes are configured by recompiliing the firmware or writing to EEPROM.
- A paper was written comparing the performance of Zigbee vs nRF24l01+, see [TMRh20s Blog](https://tmrh20.blogspot.com/2019/05/comparative-performance-analysis.html) for a detailed overview.

## Node Naming

- Leaf node: A node at the outer edge of the network with no children. ZigBee calls it
  an End Device node.
- Relay node: A node which has both parents and children, and relays messages from one
  to the other. ZigBee calls it a Router.
- Base node. The top of the tree node with no parents, only children. Typically this node
  will bridge to another kind of network like Ethernet. ZigBee calls it a Co-ordinator node.
