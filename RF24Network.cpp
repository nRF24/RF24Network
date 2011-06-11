/*
 Copyright (C) 2011 James Coliz, Jr. <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#include <avr/pgmspace.h>
#include <WProgram.h>
#include <RF24Network.h>
#include <RF24.h>

#undef SERIAL_DEBUG
#ifdef SERIAL_DEBUG
#define IF_SERIAL_DEBUG(x) (x)
#else
#define IF_SERIAL_DEBUG(x)
#endif

// Avoid spurious warnings
#undef PROGMEM 
#define PROGMEM __attribute__(( section(".progmem.data") )) 
#undef PSTR 
#define PSTR(s) (__extension__({static prog_char __c[] PROGMEM = (s); &__c[0];}))

uint16_t RF24NetworkHeader::next_id = 1;

RF24Network::RF24Network( RF24& _radio, const RF24NodeLine* _topology ): radio(_radio), topology(_topology), next_frame(frame_queue)
{
  // Find out how many nodes are defined
  num_nodes = 0;
  const RF24NodeLine* i = topology;
  while ( (i++)->parent_node != 0xFFFF )
    ++num_nodes;
}

void RF24Network::begin(uint8_t _channel, uint16_t _node_address, rf24_direction_e /*_direction*/ )
{
  if ( _node_address < num_nodes )
    node_address = _node_address;

  open_pipes();

  radio.setChannel(_channel);
  radio.setDataRate(RF24_1MBPS);
  radio.setCRCLength(RF24_CRC_16);
  radio.startListening();
  radio.printDetails();
}

void RF24Network::update(void)
{
  // if there is data ready
  uint8_t pipe_num;
  while ( radio.available(&pipe_num) )
  {
    // Dump the payloads until we've gotten everything
    boolean done = false;
    while (!done)
    {
      // Fetch the payload, and see if this was the last one.
      done = radio.read( frame_buffer, sizeof(frame_buffer) );

      // Read the beginning of the frame as the header
      const RF24NetworkHeader& header = * reinterpret_cast<RF24NetworkHeader*>(frame_buffer);

      IF_SERIAL_DEBUG(printf_P(PSTR("%lu: MAC Received on %u %s\n\r"),millis(),pipe_num,header.toString()));

#ifdef SERIAL_DEBUG
      const uint16_t* i = reinterpret_cast<const uint16_t*>(frame_buffer + sizeof(RF24NetworkHeader));
      printf_P(PSTR("%lu: NET message %04x\n\r"),millis(),*i);
#endif  

      // Is this for us?
      if ( header.to_node == node_address )
	// Add it to the buffer of frames for us
	enqueue();
      else
	// Relay it
	write(header.to_node);
    }
  }
}

bool RF24Network::enqueue(void)
{
  bool result = false;
  
  IF_SERIAL_DEBUG(printf_P(PSTR("%lu: NET Enqueue "),millis()));

  // Copy the current frame into the frame queue
  if ( next_frame <= frame_queue + sizeof(frame_queue) )
  {
    memcpy(next_frame,frame_buffer, frame_size );
    next_frame += frame_size; 

    result = true;
    IF_SERIAL_DEBUG(printf_P(PSTR("ok\n\r")));
  }
  else
    IF_SERIAL_DEBUG(printf_P(PSTR("failed\n\r")));

  return result;
}

bool RF24Network::available(void)
{
  // Are there frames on the queue for us?
  return (next_frame > frame_queue);
}

size_t RF24Network::read(RF24NetworkHeader& header,void* message, size_t maxlen)
{
  size_t bufsize = 0;

  if ( available() )
  {
    // Move the pointer back one in the queue 
    next_frame -= frame_size;
    uint8_t* frame = next_frame;
      
    // How much buffer size should we actually copy?
    bufsize = min(maxlen,frame_size-sizeof(RF24NetworkHeader));

    // Copy the next available frame from the queue into the provided buffer
    memcpy(&header,frame,sizeof(RF24NetworkHeader));
    memcpy(message,frame+sizeof(RF24NetworkHeader),bufsize);
    
    IF_SERIAL_DEBUG(printf_P(PSTR("%lu: NET Received %s\n\r"),millis(),header.toString()));
  }

  return bufsize;
}

bool RF24Network::write(RF24NetworkHeader& header,const void* message, size_t len)
{
  // Fill out the header
  header.from_node = node_address;

  // Build the full frame to send
  memcpy(frame_buffer,&header,sizeof(RF24NetworkHeader));
  memcpy(frame_buffer + sizeof(RF24NetworkHeader),message,min(frame_size-sizeof(RF24NetworkHeader),len));

  IF_SERIAL_DEBUG(printf_P(PSTR("%lu: NET Sending %s\n\r"),millis(),header.toString()));

#ifdef SERIAL_DEBUG
  const uint16_t* i = reinterpret_cast<const uint16_t*>(message);
  printf_P(PSTR("%lu: NET message %04x\n\r"),millis(),*i);
#endif  

  // If the user is trying to send it to himself
  if ( header.to_node == node_address )
    // Just queue it in the received queue
    return enqueue();
  else
    // Otherwise send it out over the air
    return write(header.to_node);
}

bool RF24Network::write(uint16_t to_node)
{
  bool ok = false;
  
  // First, stop listening so we can talk.
  radio.stopListening();

  // Which pipe should we use to get the message to the "to_node"?
  // We need to find a node who is OUR CHILD that either IS the to_node
  // or has the to_node as one of ITS children.  Failing that, we'll just
  // send it back to the parent to deal with.
  uint8_t out_node = find_node(node_address,to_node);

  // If we get '0' as a node, there is a problem
  if ( ! out_node )
  {
    IF_SERIAL_DEBUG(printf_P(PSTR("%lu: NET Cannot send to node %u, discarded\n\r"),millis(),to_node));
    return ok;
  }

  // First, stop listening so we can talk
  radio.stopListening();

  // If this node is our child, we talk on it's listening pipe.
  uint64_t out_pipe;
  if ( topology[out_node].parent_node == node_address )
    out_pipe = topology[out_node].listening_pipe;
  
  // Otherwise, it's our parent so we talk on OUR talking pipe
  else
    out_pipe = topology[node_address].talking_pipe;
  
  // Open the correct pipe for writing.  
  radio.openWritingPipe(out_pipe);

  // Retry a few times
  short attempts = 5;
  do
  {
    ok = radio.write( frame_buffer, frame_size );
  }
  while ( !ok && --attempts );

  // Now, continue listening
  radio.startListening();
  
  IF_SERIAL_DEBUG(printf_P(PSTR("%lu: MAC Sent on %x %s\n\r"),millis(),(uint16_t)out_pipe,ok?"ok":"failed"));

  return ok;
}

void RF24Network::open_pipes(void)
{
  // In order to open the right pipes, we need to know whether the node has parents
  // and/or children
  bool has_parent = ( topology[node_address].parent_node != 0 );
  bool has_children = false;

  // If there are any nodes in the topology table which consider this
  // a parent, then we do have children 
  int i = num_nodes;
  while (i-- && !has_children)
    if ( topology[i].parent_node == node_address )
      has_children = true; 

  // Open pipes for parent 
  if ( has_parent )
  {
    // Writing pipe to speak to our parent
    radio.openWritingPipe(topology[node_address].talking_pipe);

    // Listen to our parent.  If we have children, we need to do so
    // on pipe 0 to make room for more children
    if ( has_children )
      radio.openReadingPipe(0,topology[node_address].listening_pipe);
    else
      radio.openReadingPipe(1,topology[node_address].listening_pipe);
  }

  // Listen on children's talking pipes
  if ( has_children )
  {
    // First child listening pipe is #1
    uint8_t current_pipe = 1;
  
    // The topology table tells us who our children are
    int i = num_nodes;
    while (i--)
      if ( topology[i].parent_node == node_address )
	radio.openReadingPipe(current_pipe++,topology[i].talking_pipe);
  }

}

/**
 * Find where to send a message to reach the target node
 *
 * Given the @p target_node, find the child or parent of
 * the @p current_node which will relay messages for the target.
 *
 * This is needed in a multi-hop system where the @p current_node
 * is not adjacent to the @p target_node in the topology
 */
uint16_t RF24Network::find_node( uint16_t current_node, uint16_t target_node )
{
  uint16_t out_node = target_node;
  bool found_target = false;
  while ( ! found_target )
  {
    if ( topology[out_node].parent_node == current_node )
    {
      found_target = true; 
    }
    else
    {
      out_node = topology[out_node].parent_node;

      // If we've made it all the way back to the base without finding
      // common lineage with the to_node, we will just send it to our parent
      if ( out_node == 0 || out_node == 0xFFFF )
      {
	out_node = topology[current_node].parent_node;
	found_target = true;
      }
    }
  }
  return out_node;
}

const char* RF24NetworkHeader::toString(void) const
{
  static char buffer[28];
  snprintf(buffer,sizeof(buffer),"id %04x from %04x to %04x",id,from_node,to_node);
  return buffer;
}

// vim:ai:cin:sts=2 sw=2 ft=cpp
