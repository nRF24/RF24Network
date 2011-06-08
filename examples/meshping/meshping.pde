#include <avr/pgmspace.h>
#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>

// Avoid spurious warnings
#undef PROGMEM 
#define PROGMEM __attribute__(( section(".progmem.data") )) 
#undef PSTR 
#define PSTR(s) (__extension__({static prog_char __c[] PROGMEM = (s); &__c[0];}))

// This is a very simple example, using 2 nodes.  This exact same code will work
// With an unlimited number of nodes connected in a giant mesh.  Increase the ping
// interval with many more nodes!
RF24NodeLine topology[] = 
{
  { 0x0000000000LL, 0x0000000000LL, 0 }, // Node 0: Invalid
  { 0xE7E7E7E701LL, 0xE7E7E7E701LL, 0 }, // Node 1: Base, has no parent
  { 0xE7E7E7E70ELL, 0xE7E7E7E70ELL, 1 }, // Node 2: Leaf, child of #1
};

RF24 radio(8,9);
RF24Network network(radio,topology);

// Node identities
const int role_pin = 7; // Connect to ground on one unit, leave disconnected on the other
uint16_t this_node;
uint16_t other_node;

// The message that we send is just a ulong, containing the time
unsigned long message;

// Delay manager to send pings regularly
const unsigned long interval = 2000; // ms
unsigned long last_time_sent;

void setup(void)
{
  SPI.begin();
  radio.begin();

  // Figure out which node we are
  pinMode(role_pin,INPUT);
  digitalWrite(role_pin,HIGH);
  if ( digitalRead(role_pin) )
  {
    this_node = 1;
    other_node = 2;
  }
  else
  {
    this_node = 2;
    other_node = 1;
  }


  // Bring up the RF network
  network.begin(/*channel*/ 100, /*node address*/ this_node, /*directionality*/ RF24_NET_BIDIRECTIONAL);
}

void loop(void)
{
  // Pump the network regularly
  network.update();

  // Is there anything ready for us?
  if ( network.available() )
  {
    // If so, grab it and print it out
    RF24NetworkHeader header;
    network.read(header,&message,sizeof(message));
    printf_P(PSTR("%lu: RECEIVED %lu from %u\n\r"),millis(),message,header.from_node);
  }

  // Send a ping to the other guy every 'interval' ms
  unsigned long now = millis();
  if ( now - last_time_sent > interval )
  {
    last_time_sent = now;

    printf_P(PSTR("%lu: SENDING %lu..."),millis(),now);
    
    message = now;
    RF24NetworkHeader header(/*to node*/ other_node);
    bool ok = network.write(header,&message,sizeof(message));
    if (ok)
    {
      printf_P(PSTR("ok\n\r"));
    }
    else
    {
      printf_P(PSTR("failed\n\r"));

      // Try sending at a different time next time
      last_time_sent += 100;
    }
  }
}
// vim:ai:cin:sts=2 sw=2 ft=cpp
