/*
 Copyright (C) 2011 James Coliz, Jr. <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 
 2014 - TMRh20: New sketch included with updated library
 */

/**
 * Example: Network topology, and pinging across a tree/mesh network with sleeping nodes
 *
 * Using this sketch, each node will send a ping to every other node in the network every few seconds. 
 * The RF24Network library will route the message across the mesh to the correct node.
 *
 * This sketch demonstrates the new functionality of nodes sleeping in STANDBY-I mode. In receive mode,
 * the radio will draw about 13.5 mA. In STANDBY-I mode, the radio draws .000022mA, and is able to awake
 * when payloads are received.
 *
 * How it Works:
 * The enhanced sleep mode utilizes the ACK payload functionality, as radios that are in Primary Transmitter
 * mode (PTX) are able to receive ACK payloads while in STANDBY-I mode. 
 * 1. The radio is configured to use Dynamic Payloads and ACK payloads with Auto-Ack enabled 
 * 2. The radio enters PTX mode and attaches an interrupt handler to the radio interrupt input pin (pin 2)
 * 3. The radio uses the Watchdog Timer to awake at set 1 second intervals in this example
 * 4. Every interval, it sends out a 'sleep' payload and goes back to sleep. Incoming payloads will then be treated as ACK payloads, while the radio remains in STANDBY-I mode.
 * 5. If an interrupt is triggered, the radio wakes up
 * 6. When a message is sent to the sleeping node, the interrupt triggers a wake up, the MCU
 * grabs the payload, and switches back to receive mode in case more data is on its way.
 *
 * The node (Arduino) power use can be reduced further by disabling unnessessary systems via the Power Reduction Register(s) (PRR).
 */

#include <avr/pgmspace.h>
#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include "printf.h"
#include <avr/sleep.h>
#include <avr/power.h>

/***********************************************************************
************* Set the Node Address *************************************
/***********************************************************************/

// These are the Octal addresses that will be assigned
const uint16_t node_address_set[10] = { 00, 02, 05, 012, 015, 022, 025, 032, 035, 045 };

// 0 = Master
// 1-2 (02,05)   = Children of Master(00)
// 3,5 (012,022) = Children of (02)
// 4,6 (015,025) = Children of (05)
// 7   (032)     = Child of (02)
// 8,9 (035,045) = Children of (05)

uint8_t NODE_ADDRESS = 1; // Use numbers 0 through 9 to select an address from the array

/***********************************************************************/
/***********************************************************************/


RF24 radio(7,8);                                    // CE & CS pins to use (Using 7,8 on Uno,Nano)
RF24Network network(radio); 

uint16_t this_node;                                  // Our node address

const unsigned long interval = 1000; // ms           // Delay manager to send pings regularly. Because of sleepNode(), this is largely irrelevant.
unsigned long last_time_sent;

const short max_active_nodes = 10;                    // Array of nodes we are aware of
uint16_t active_nodes[max_active_nodes];
short num_active_nodes = 0;
short next_ping_node_index = 0;


bool send_T(uint16_t to);                              // Prototypes for functions to send & handle messages
bool send_N(uint16_t to);
void handle_T(RF24NetworkHeader& header);
void handle_N(RF24NetworkHeader& header);
void add_node(uint16_t node);


//This is for sleep mode. It is not really required, as users could just use the number 0 through 10
typedef enum { wdt_16ms = 0, wdt_32ms, wdt_64ms, wdt_128ms, wdt_250ms, wdt_500ms, wdt_1s, wdt_2s, wdt_4s, wdt_8s } wdt_prescalar_e;

unsigned long awakeTime = 500;                          // How long in ms the radio will stay awake after leaving sleep mode
unsigned long sleepTimer = 0;                           // Used to keep track of how long the system has been awake

void setup(){
  
  Serial.begin(115200);
  printf_begin();
  printf_P(PSTR("\n\rRF24Network/examples/meshping/\n\r"));

  this_node = node_address_set[NODE_ADDRESS];            // Which node are we?
  
  SPI.begin();                                           // Bring up the RF network
  radio.begin();
  radio.setPALevel(RF24_PA_HIGH);
  network.begin(/*channel*/ 100, /*node address*/ this_node );

/******************************** This is the configuration for sleep mode ***********************/
  network.setup_watchdog(wdt_1s);                       //The watchdog timer will wake the MCU and radio every second to send a sleep payload, then go back to sleep
}

void loop(){
    
  network.update();                                      // Pump the network regularly

   while ( network.available() )  {                      // Is there anything ready for us?
     
    RF24NetworkHeader header;                            // If so, take a look at it
    network.peek(header);

    
      switch (header.type){                              // Dispatch the message to the correct handler.
        case 'T': handle_T(header); break;
        case 'N': handle_N(header); break;
        
   /************* SLEEP MODE *********/
  // Note: A 'sleep' header has been defined, and should only need to be ignored if a node is routing traffic to itself
  // The header is defined as:  RF24NetworkHeader sleepHeader(/*to node*/ 00, /*type*/ 'S' /*Sleep*/);    
         case 'S': /*This is a sleep payload, do nothing*/ break;
        
        default:  printf_P(PSTR("*** WARNING *** Unknown message type %c\n\r"),header.type);
                  network.read(header,0,0);
                  break;
      };
    }
    
/***************************** CALLING THE NEW SLEEP FUNCTION ************************/    

  if(millis() - sleepTimer > awakeTime&& NODE_ADDRESS){  // Want to make sure the Arduino stays awake for a little while when data comes in. Do NOT sleep if master node.
     Serial.println("Sleep");
     sleepTimer = millis();                           // Reset the timer value
     delay(100);                                      // Give the Serial print some time to finish up
     radio.stopListening();                           // Switch to PTX mode. Payloads will be seen as ACK payloads, and the radio will wake up
     network.sleepNode(8,0);                          // Sleep the node for 8 cycles of 1second intervals
     Serial.println("Awake"); 
  }
  
   //Examples:
   // network.sleepNode( cycles, interrupt-pin );
   // network.sleepNode(0,0);         // The WDT is configured in this example to sleep in cycles of 1 second. This will sleep 1 second, or until a payload is received 
   // network.sleepNode(1,255);       // Sleep this node for 1 second. Do not wake up until then, even if a payload is received ( no interrupt ) Payloads will be lost.
    
                                /****  end sleep section ***/  
  
  
  unsigned long now = millis();                         // Send a ping to the next node every 'interval' ms
  if ( now - last_time_sent >= interval ){
    last_time_sent = now;


    uint16_t to = 00;                                   // Who should we send to? By default, send to base
    
    
    if ( num_active_nodes ){                            // Or if we have active nodes,
        to = active_nodes[next_ping_node_index++];      // Send to the next active node
        if ( next_ping_node_index > num_active_nodes ){ // Have we rolled over?
	    next_ping_node_index = 0;                   // Next time start at the beginning
	    to = 00;                                    // This time, send to node 00.
        }
    }

    bool ok;

    
    if ( this_node > 00 || to == 00 ){                    // Normal nodes send a 'T' ping
        ok = send_T(to);   
    }else{                                                // Base node sends the current active nodes out
        ok = send_N(to);
    }
    
    if (ok){                                              // Notify us of the result
        printf_P(PSTR("%lu: APP Send ok\n\r"),millis());
    }else{
        printf_P(PSTR("%lu: APP Send failed\n\r"),millis());
        last_time_sent -= 100;                            // Try sending at a different time next time
    }
  }
}

/**
 * Send a 'T' message, the current time
 */
bool send_T(uint16_t to)
{
  RF24NetworkHeader header(/*to node*/ to, /*type*/ 'T' /*Time*/);
  
  // The 'T' message that we send is just a ulong, containing the time
  unsigned long message = millis();
  printf_P(PSTR("---------------------------------\n\r"));
  printf_P(PSTR("%lu: APP Sending %lu to 0%o...\n\r"),millis(),message,to);
  return network.write(header,&message,sizeof(unsigned long));
}

/**
 * Send an 'N' message, the active node list
 */
bool send_N(uint16_t to)
{
  RF24NetworkHeader header(/*to node*/ to, /*type*/ 'N' /*Time*/);
  
  printf_P(PSTR("---------------------------------\n\r"));
  printf_P(PSTR("%lu: APP Sending active nodes to 0%o...\n\r"),millis(),to);
  return network.write(header,active_nodes,sizeof(active_nodes));
}

/**
 * Handle a 'T' message
 * Add the node to the list of active nodes
 */
void handle_T(RF24NetworkHeader& header){

  unsigned long message;                                                                      // The 'T' message is just a ulong, containing the time
  network.read(header,&message,sizeof(unsigned long));
  printf_P(PSTR("%lu: APP Received %lu from 0%o\n\r"),millis(),message,header.from_node);


  if ( header.from_node != this_node || header.from_node > 00 )                                // If this message is from ourselves or the base, don't bother adding it to the active nodes.
    add_node(header.from_node);
}

/**
 * Handle an 'N' message, the active node list
 */
void handle_N(RF24NetworkHeader& header)
{
  static uint16_t incoming_nodes[max_active_nodes];

  network.read(header,&incoming_nodes,sizeof(incoming_nodes));
  printf_P(PSTR("%lu: APP Received nodes from 0%o\n\r"),millis(),header.from_node);

  int i = 0;
  while ( i < max_active_nodes && incoming_nodes[i] > 00 )
    add_node(incoming_nodes[i++]);
}

/**
 * Add a particular node to the current list of active nodes
 */
void add_node(uint16_t node){
  
  short i = num_active_nodes;                                    // Do we already know about this node?
  while (i--)  {
    if ( active_nodes[i] == node )
        break;
  }
  
  if ( i == -1 && num_active_nodes < max_active_nodes ){         // If not, add it to the table
      active_nodes[num_active_nodes++] = node; 
      printf_P(PSTR("%lu: APP Added 0%o to list of active nodes.\n\r"),millis(),node);
  }
}
