ESB_Network class
~~~~~~~~~~~~~~~~~

.. doxygentypedef:: RF24Network

.. cpp:class:: template <class ESB_Radio = RF24> ESB_Network

    :tparam ESB_Radio: The ``radio`` object's type. Defaults to :class:`RF24` for legacy behavior.
        This new abstraction is really meant for using the nRF52840 SoC as a drop-in replacement
        for the nRF24L01 radio. For more detail, see the
        `nrf_to_nrf Arduino library <https://github.com/TMRh20/nrf_to_nrf>`_.

    .. doxygenfunction:: ESB_Network::ESB_Network

    .. seealso::
        Use the :class:`RF24` class to create the radio object.

Basic API
============

.. doxygenfunction:: ESB_Network::begin (uint16_t _node_address)

.. seealso:: :external:cpp:func:`RF24::begin()`

.. doxygenfunction:: ESB_Network::begin (uint8_t channel, uint16_t _node_address)

.. seealso:: :external:cpp:func:`RF24::begin()`

.. doxygenfunction:: ESB_Network::update
.. doxygenfunction:: ESB_Network::available
.. doxygenfunction:: ESB_Network::read
.. doxygenfunction:: ESB_Network::write (RF24NetworkHeader &header, const void *message, uint16_t len)
.. doxygenfunction:: ESB_Network::peek (RF24NetworkHeader &header)
.. doxygenfunction:: ESB_Network::peek (RF24NetworkHeader &header, void *message, uint16_t maxlen=MAX_PAYLOAD_SIZE)

Advanced API
============

.. doxygenfunction:: ESB_Network::failures
.. doxygenfunction:: ESB_Network::multicast
.. doxygenfunction:: ESB_Network::write (RF24NetworkHeader &header, const void *message, uint16_t len, uint16_t writeDirect)
.. doxygenfunction:: ESB_Network::sleepNode
.. doxygenfunction:: ESB_Network::parent
.. doxygenfunction:: ESB_Network::addressOfPipe
.. doxygenfunction:: ESB_Network::is_valid_address

Configuration API
==================

.. doxygenvariable:: ESB_Network::multicastRelay
.. doxygenvariable:: ESB_Network::txTimeout
.. doxygenvariable:: ESB_Network::routeTimeout
.. doxygenfunction:: ESB_Network::multicastLevel
.. doxygenfunction:: ESB_Network::setup_watchdog

External Applications/Systems
=============================

Interface for External Applications and Systems (RF24Mesh, RF24Ethernet)

.. doxygenvariable:: ESB_Network::frame_buffer
.. doxygenvariable:: ESB_Network::external_queue
.. doxygenvariable:: ESB_Network::frag_ptr
.. doxygenvariable:: ESB_Network::returnSysMsgs
.. doxygenvariable:: ESB_Network::networkFlags

Protected Members
-----------------
These members are accessible by ESB_Network derivatives.

.. doxygenvariable:: ESB_Network::_multicast_level
.. doxygenvariable:: ESB_Network::node_address
