Reserved System Message Types
=============================

The network will determine whether to automatically acknowledge payloads based on
their general :cpp:var:`~RF24NetworkHeader::type`.

- **User types** (1 - 127) 1 - 64 will NOT be acknowledged
- **System types** (128 - 255) 192 - 255 will NOT be acknowledged

System types can also contain message data.

.. doxygengroup:: DEFINED_TYPES
    :members:
    :content-only:
