# Advanced Configuration

@tableofcontents

<!-- markdownlint-disable MD033 -->
RF24Network offers many features, some of which can be configured by editing the RF24Network_config.h file

| Configuration Option            | Description                                                                                                                                                                                                            |
| ------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `#define RF24NetworkMulticast`  | This option allows nodes to send and receive multicast payloads.<br>Nodes with multicast enabled can also be configured to relay multicast payloads on to further multicast levels.<br>See RF24Network::multicastRelay |
| `#define DISABLE_FRAGMENTATION` | Fragmentation is enabled by default, and uses an additional 144 bytes of memory.                                                                                                                                       |
| `#define MAX_PAYLOAD_SIZE 144`  | The maximum size of payloads defaults to 144 bytes. If used with RF24toTUN and two Raspberry Pi, set this to 1500                                                                                                      |
| `#define DISABLE_USER_PAYLOADS` | This option will disable user-caching of payloads entirely. Use with RF24Ethernet to reduce memory usage. (TCP/IP is an external data type, and not cached)                                                            |
| `#define ENABLE_SLEEP_MODE`     | Uncomment this option to enable sleep mode for AVR devices. (ATTiny,Uno, etc)                                                                                                                                          |
| `#define ENABLE_NETWORK_STATS`  | Enable counting of all successful or failed transmissions, routed or sent directly                                                                                                                                     |
