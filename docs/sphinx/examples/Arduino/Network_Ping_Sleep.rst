Network_Ping_Sleep.ino
==========================

This example demonstrates how nodes on the network utilize sleep mode to conserve power. For example,
the radio itself will draw about 13.5mA in receive mode. In sleep mode, it will use as little as 22ua (.000022mA)
of power when not actively transmitting or receiving data. In addition, the Arduino is powered down as well,
dropping network power consumption dramatically compared to previous capabilities.

.. note::
    Sleeping nodes generate traffic that will wake other nodes up. This may be mitigated with further modifications. Sleep
    payloads are currently always routed to the master node, which will wake up intermediary nodes. Routing nodes can be configured
    to go back to sleep immediately.

The displayed ``millis()`` count will give an indication of how much a node has been sleeping compared to the others, as ``millis()`` will
not increment while a node sleeps.

- Using this sketch, each node will send a ping to every other node in the network every few seconds.
- The RF24Network library will route the message across the mesh to the correct node.

.. literalinclude:: ../../../../examples/Network_Ping_Sleep/Network_Ping_Sleep.ino
    :linenos:
