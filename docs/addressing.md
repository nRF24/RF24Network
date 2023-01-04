# Addressing Format: Understanding Addressing and Topology

@tableofcontents

<!-- markdownlint-disable MD033 MD032 -->
An overview of addressing in RF24Network

## Overview

The nrf24 radio modules typically use a 40-bit address format, requiring 5-bytes of storage space per address, and allowing a wide
array of addresses to be utilized. In addition, the radios are limited to direct communication with 6 other nodes while using the
Enhanced-Shock-Burst (ESB) functionality of the radios.

RF24Network uses a simple method of data compression to store the addresses using only 2 bytes, in a format designed to represent the
network topology in an intuitive way.
See the [Topology and Overview](md_docs_tuning.html) page for more info regarding topology.

## Decimal, Octal and Binary formats

Say we want to designate a logical address to a node, using a tree topology as defined by the
manufacturer. In the simplest format, we could assign the first node the address of 1, the second
2, and so on. Since a single node can only connect to 6 other nodes (1 parent and 5 children)
subnets need to be created if using more than 6 nodes. In this case, the

- children of node 1 could simply be designated as 11, 21, 31, 41, and 51
- children of node 2 could be designated as 12, 22, 32, 42, and 52

The above example is exactly how RF24Network manages the addresses, but they are represented in Octal format.

### Decimal, Octal and Binary

| Decimal | Octal |  Binary  |
| :-----: | :---: | :------: |
|    1    |  01   | 00000001 |
|   11    |  013  | 00001011 |
|    9    |  011  | 00001001 |
|   73    | 0111  | 01001001 |
|   111   | 0157  | 01101111 |

Since the numbers 0-7 can be represented in exactly three bits, each digit is represented by
exactly 3 bits when viewed in octal format. This allows a very simple method of managing addresses
via masking and bit shifting.

## Displaying Addresses

When using Arduino devices, octal addresses can be printed in the following manner:

```cpp
uint16_t address = 0111;
Serial.println(address, OCT);
```

Printf can also be used, if enabled, or if using linux/RPi

```cpp
uint16_t address = 0111;
printf("0%o\n", address);
```

@see
- [This cplusplus.com tutorial](http://www.cplusplus.com/doc/hex/) for more information number bases.
- The [Topology and Overview page](md_docs_tuning.html) for more information regarding network topology.
