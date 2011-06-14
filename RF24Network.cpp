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

#define SERIAL_DEBUG
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

uint64_t pipe_address( uint16_t node, uint8_t pipe );

/******************************************************************/

RF24Network::RF24Network( RF24& _radio ): radio(_radio), next_frame(frame_queue), bidirectional(true)
{
}

/******************************************************************/

void RF24Network::begin(uint8_t _channel, uint16_t _node_address, rf24_direction_e _direction )
{
  node_address = _node_address;

  if ( _direction == RF24_NET_UNIDIRECTIONAL )
    bidirectional = false;

  // Set up the radio the way we want it to look
  radio.setChannel(_channel);
  radio.setDataRate(RF24_1MBPS);
  radio.setCRCLength(RF24_CRC_16);

  // Setup our address helper cache
  setup_address();
  
  // Open up our listening pipe
  radio.openReadingPipe(0,pipe_address(_node_address,0));
  radio.startListening();

  // Spew debugging state about the radio
  radio.printDetails();
}

/******************************************************************/

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

      // If this was for us, from one of our children, but on our listening
      // pipe, it could mean that we are not listening to them.  If so, open up
      // and listen to their talking pipe

      if ( header.to_node == node_address && pipe_num == 0 && is_descendant(header.from_node) )
      {
	uint8_t pipe = pipe_to_descendant(header.from_node);
	radio.openReadingPipe(pipe,pipe_address(node_address,pipe));

	// Also need to open pipe 1 so the system can get the full 5-byte address of the pipe.
	radio.openReadingPipe(1,pipe_address(node_address,1));
      }
    }
  }
}

/******************************************************************/

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
  {
    IF_SERIAL_DEBUG(printf_P(PSTR("failed\n\r")));
  }

  return result;
}

/******************************************************************/

bool RF24Network::available(void)
{
  // Are there frames on the queue for us?
  return (next_frame > frame_queue);
}

/******************************************************************/

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

/******************************************************************/

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

/******************************************************************/

bool RF24Network::write(uint16_t to_node)
{
  bool ok = false;
  
  // First, stop listening so we can talk.
  radio.stopListening();

  // Where do we send this?  By default, to our parent
  uint16_t send_node = parent_node;
  // On which pipe
  uint8_t send_pipe = parent_pipe;
  
  // If the node is a direct child,
  if ( is_direct_child(to_node) )
  {
    // Send directly
    send_node = to_node;

    // To its listening pipe
    send_pipe = 0;
  }
  // If the node is a child of a child
  // talk on our child's listening pipe,
  // and let the direct child relay it.
  else if ( is_descendant(to_node) )
  {
    send_node = direct_child_route_to(to_node);
    send_pipe = 0;
  }
  
  IF_SERIAL_DEBUG(printf_P(PSTR("%lu: MAC Sending to 0%o via 0%o on pipe %x\n\r"),millis(),to_node,send_node,send_pipe));

  // First, stop listening so we can talk
  radio.stopListening();

  // Put the frame on the pipe
  ok = write_to_pipe( send_node, send_pipe );

  // If we are talking on our talking pipe, it's possible that no one is listening.
  // If this fails, try sending it on our parent's listening pipe.  That will wake
  // it up, and next time it will listen to us.

  if ( !ok && send_node == parent_node )
    ok = write_to_pipe( parent_node, 0 );
  
  // Now, continue listening
  radio.startListening();

  return ok;
}

/******************************************************************/

bool RF24Network::write_to_pipe( uint16_t node, uint8_t pipe )
{
  bool ok = false;
  
  uint64_t out_pipe = pipe_address( node, pipe );
 
  // Open the correct pipe for writing.  
  radio.openWritingPipe(out_pipe);

  // Retry a few times
  short attempts = 5;
  do
  {
    ok = radio.write( frame_buffer, frame_size );
  }
  while ( !ok && --attempts );

  IF_SERIAL_DEBUG(printf_P(PSTR("%lu: MAC Sent on %lx %s\n\r"),millis(),(uint32_t)out_pipe,ok?"ok":"failed"));

  return ok;
}

/******************************************************************/
#if 0
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

    // In bi-directional mode only...
    if ( bidirectional )
    {
      // Listen to our parent.  If we have children, we need to do so
      // on pipe 0 to make room for more children
      if ( has_children )
	radio.openReadingPipe(0,topology[node_address].listening_pipe);
      else
	radio.openReadingPipe(1,topology[node_address].listening_pipe);
    }
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

  //if ( bidirectional || has_children )
    radio.startListening();
}
/******************************************************************/

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
#endif
/******************************************************************/

const char* RF24NetworkHeader::toString(void) const
{
  static char buffer[45];
  snprintf(buffer,sizeof(buffer),"id %04x from %04x to %04x type %c time %04x",id,from_node,to_node,type,time);
  return buffer;
}

/******************************************************************/

bool RF24Network::is_direct_child( uint16_t node )
{
  bool result = false;

  // A direct child of ours has the same low numbers as us, and only
  // one higher number.
  //
  // e.g. node 0234 is a direct child of 034, and node 01234 is a
  // descendant but not a direct child

  // First, is it even a descendant?
  if ( is_descendant(node) )
  {
    // Does it only have ONE more level than us?
    uint16_t child_node_mask = ( ~ node_mask ) << 3;
    result = ( node & child_node_mask ) == 0 ;
  }

  return result;
}

/******************************************************************/

bool RF24Network::is_descendant( uint16_t node )
{
  return ( node & node_mask ) == node_address;
}

/******************************************************************/

void RF24Network::setup_address(void)
{
  // First, establish the node_mask
  uint16_t node_mask_check = 0xFFFF;
  while ( node_address & node_mask_check )
    node_mask_check <<= 3;
  
  node_mask = ~ node_mask_check;

  // parent mask is the next level down
  uint16_t parent_mask = node_mask >> 3;

  // parent node is the part IN the mask
  parent_node = node_address & parent_mask;

  // parent pipe is the part OUT of the mask
  uint16_t i = node_address;
  uint16_t m = parent_mask;
  while (m)
  {
    i >>= 3;
    m >>= 3;
  }
  parent_pipe = i;

#ifdef SERIAL_DEBUG
  printf_P(PSTR("setup_address node=0%o mask=0%o parent=0%o pipe=0%o\n\r"),node_address,node_mask,parent_node,parent_pipe);
#endif
}

/******************************************************************/

uint16_t RF24Network::direct_child_route_to( uint16_t node )
{
  // Presumes that this is in fact a child!!

  uint16_t child_mask = ( node_mask << 3 ) | 0B111;
  return node & child_mask ;
}

/******************************************************************/

uint8_t RF24Network::pipe_to_descendant( uint16_t node )
{
  uint16_t i = node;
  uint16_t m = node_mask;
  
  while (m)
  {
    i >>= 3;
    m >>= 3;
  }

  return i & 0B111;
}

/******************************************************************/

uint64_t pipe_address( uint16_t node, uint8_t pipe )
{
  static uint8_t pipe_segment[] = { 0x3c, 0x5a, 0x69, 0x96, 0xa5, 0xc3 };

  uint64_t result;
  uint8_t* out = reinterpret_cast<uint8_t*>(&result);

  out[0] = pipe_segment[pipe];

  uint8_t w; 
  short i = 4;
  short shift = 12;
  while(i--)
  {
    w = ( node >> shift ) & 0xF ; 
    w |= ~w << 4;
    out[i+1] = w;

    shift -= 4;
  }

#ifdef SERIAL_DEBUG

  uint32_t* top = reinterpret_cast<uint32_t*>(out+1);
  printf_P(PSTR("pipe_address(%x,%u)=%lx%x\n\r"),node,pipe,*top,*out);

#endif

  return result;
}

// vim:ai:cin:sts=2 sw=2 ft=cpp
