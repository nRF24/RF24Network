RF24Network class
~~~~~~~~~~~~~~~~~

.. cpp:class:: RF24Network

    .. doxygenfunction:: RF24Network::RF24Network

    .. seealso::
        Use the :class:`RF24` class to create the radio object.

Basic API
============

.. doxygenfunction:: RF24Network::begin (uint16_t _node_address)

.. seealso:: :external:cpp:func:`RF24::begin()`

.. doxygenfunction:: RF24Network::begin (uint8_t channel, uint16_t _node_address)

.. seealso:: :external:cpp:func:`RF24::begin()`

.. doxygenfunction:: RF24Network::update
.. doxygenfunction:: RF24Network::available
.. doxygenfunction:: RF24Network::read
.. doxygenfunction:: RF24Network::write (RF24NetworkHeader &header, const void *message, uint16_t len)
.. doxygenfunction:: RF24Network::peek (RF24NetworkHeader &header)
.. doxygenfunction:: RF24Network::peek (RF24NetworkHeader &header, void *message, uint16_t maxlen=MAX_PAYLOAD_SIZE)

Advanced API
============

.. doxygenfunction:: RF24Network::failures
.. doxygenfunction:: RF24Network::multicast
.. doxygenfunction:: RF24Network::write (RF24NetworkHeader &header, const void *message, uint16_t len, uint16_t writeDirect)
.. doxygenfunction:: RF24Network::sleepNode
.. doxygenfunction:: RF24Network::parent
.. doxygenfunction:: RF24Network::addressOfPipe
.. doxygenfunction:: RF24Network::is_valid_address

Configuration API
==================

.. doxygenvariable:: RF24Network::multicastRelay
.. doxygenvariable:: RF24Network::txTimeout
.. doxygenvariable:: RF24Network::routeTimeout
.. doxygenfunction:: RF24Network::multicastLevel
.. doxygenfunction:: RF24Network::setup_watchdog

External Applications/Systems
=============================

Interface for External Applications and Systems (RF24Mesh, RF24Ethernet)

.. doxygenvariable:: RF24Network::frame_buffer
.. doxygenvariable:: RF24Network::external_queue
.. doxygenvariable:: RF24Network::frag_ptr
.. doxygenvariable:: RF24Network::returnSysMsgs
.. doxygenvariable:: RF24Network::networkFlags

Protected Members
-----------------
These members are accessible by RF24Network derivatives.

.. doxygenvariable:: RF24Network::_multicast_level
.. doxygenvariable:: RF24Network::node_address
