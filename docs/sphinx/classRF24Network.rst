ESBNetwork class
~~~~~~~~~~~~~~~~~

.. doxygentypedef:: RF24Network

.. cpp:class:: template <class radio_t = RF24> ESBNetwork

    :tparam radio_t: The ``radio`` object's type. Defaults to :class:`RF24` for legacy behavior.
        This new abstraction is really meant for using the nRF52840 SoC as a drop-in replacement
        for the nRF24L01 radio. For more detail, see the
        `nrf_to_nrf Arduino library <https://github.com/TMRh20/nrf_to_nrf>`_.

    .. doxygenfunction:: ESBNetwork::ESBNetwork

    .. seealso::
        Use the :class:`RF24` class to create the radio object.

Basic API
============

.. doxygenfunction:: ESBNetwork::begin (uint16_t _node_address)

.. seealso:: :external:cpp:func:`RF24::begin()`

.. doxygenfunction:: ESBNetwork::begin (uint8_t channel, uint16_t _node_address)

.. seealso:: :external:cpp:func:`RF24::begin()`

.. doxygenfunction:: ESBNetwork::update
.. doxygenfunction:: ESBNetwork::available
.. doxygenfunction:: ESBNetwork::read
.. doxygenfunction:: ESBNetwork::write (RF24NetworkHeader &header, const void *message, uint16_t len)
.. doxygenfunction:: ESBNetwork::peek (RF24NetworkHeader &header)
.. doxygenfunction:: ESBNetwork::peek (RF24NetworkHeader &header, void *message, uint16_t maxlen=MAX_PAYLOAD_SIZE)

Advanced API
============

.. doxygenfunction:: ESBNetwork::failures
.. doxygenfunction:: ESBNetwork::multicast
.. doxygenfunction:: ESBNetwork::write (RF24NetworkHeader &header, const void *message, uint16_t len, uint16_t writeDirect)
.. doxygenfunction:: ESBNetwork::sleepNode
.. doxygenfunction:: ESBNetwork::parent
.. doxygenfunction:: ESBNetwork::addressOfPipe
.. doxygenfunction:: ESBNetwork::is_valid_address

Configuration API
==================

.. doxygenvariable:: ESBNetwork::multicastRelay
.. doxygenvariable:: ESBNetwork::txTimeout
.. doxygenvariable:: ESBNetwork::routeTimeout
.. doxygenfunction:: ESBNetwork::multicastLevel
.. doxygenfunction:: ESBNetwork::setup_watchdog

External Applications/Systems
=============================

Interface for External Applications and Systems (RF24Mesh, RF24Ethernet)

.. doxygenvariable:: ESBNetwork::frame_buffer
.. doxygenvariable:: ESBNetwork::external_queue
.. doxygenvariable:: ESBNetwork::frag_ptr
.. doxygenvariable:: ESBNetwork::returnSysMsgs
.. doxygenvariable:: ESBNetwork::networkFlags

Protected Members
-----------------
These members are accessible by ESBNetwork derivatives.

.. doxygenvariable:: ESBNetwork::_multicast_level
.. doxygenvariable:: ESBNetwork::node_address
