/*
 Copyright (C) 2011 James Coliz, Jr. <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#include "RF24Network_config.h"
#include "RF24.h"
#include "RF24Network.h"

#if defined (ENABLE_SLEEP_MODE)
	#include <avr/sleep.h>
	#include <avr/power.h>
	volatile byte sleep_cycles_remaining;
#endif

uint16_t RF24NetworkHeader::next_id = 1;

uint64_t pipe_address( uint16_t node, uint8_t pipe );
#if defined (RF24NetworkMulticast)
uint16_t levelToAddress( uint8_t level );
#endif
bool is_valid_address( uint16_t node );
uint32_t nFails = 0, nOK=0;

/******************************************************************/
#if !defined (DUAL_HEAD_RADIO)
RF24Network::RF24Network( RF24& _radio ): radio(_radio), next_frame(frame_queue)
{
}
#else
RF24Network::RF24Network( RF24& _radio, RF24& _radio1 ): radio(_radio), radio1(_radio1), next_frame(frame_queue)
{
}
#endif
/******************************************************************/

void RF24Network::begin(uint8_t _channel, uint16_t _node_address )
{
  if (! is_valid_address(_node_address) )
    return;

  node_address = _node_address;

  if ( ! radio.isValid() ){
    return;
  }

  // Set up the radio the way we want it to look
  radio.setChannel(_channel);
  radio.setDataRate(RF24_1MBPS);
  radio.setCRCLength(RF24_CRC_16);
  radio.enableDynamicAck();
  // Use different retry periods to reduce data collisions

    uint8_t retryVar = (((node_address % 6)+1) *2) + 3;
  radio.setRetries(retryVar, 5);
  txTimeout = 20;
  routeTimeout = txTimeout+35; // About 2.5ms max delay per node at optimal routing speeds, 10 nodes maximum hop + max retry time for auto-ack + some delay to relieve congestion

  //printf_P(PSTR("Retries: %d, txTimeout: %d"),retryVar,txTimeout);
#if defined (DUAL_HEAD_RADIO)
  radio1.setChannel(_channel);
  radio1.setDataRate(RF24_1MBPS);
  radio1.setCRCLength(RF24_CRC_16);

#endif

  // Setup our address helper cache
  setup_address();

  // Open up all listening pipes
  int i = 6;
  while (i--){
    radio.openReadingPipe(i,pipe_address(_node_address,i));	
  }
  #if defined (RF24NetworkMulticast)
    uint8_t count = 0;
	while(_node_address){
		_node_address/=8;
		count++;
	}
	multicast_level = count;	
  #endif
  radio.startListening();

}

/******************************************************************/
void RF24Network::failures(uint32_t *_fails, uint32_t *_ok){
	*_fails = nFails;
	*_ok = nOK;
}

uint8_t RF24Network::update(void)
{
  // if there is data ready
  uint8_t pipe_num;
  while ( radio.isValid() && radio.available())//&pipe_num) )
  {

    // Dump the payloads until we've gotten everything

    //while (radio.available())
    //{
      // Fetch the payload, and see if this was the last one.
      radio.read( frame_buffer, sizeof(frame_buffer) );

      // Read the beginning of the frame as the header
      const RF24NetworkHeader& header = * reinterpret_cast<RF24NetworkHeader*>(frame_buffer);

      IF_SERIAL_DEBUG(printf_P(PSTR("%lu: MAC Received on %u %s\n\r"),millis(),pipe_num,header.toString()));
      IF_SERIAL_DEBUG(const uint16_t* i = reinterpret_cast<const uint16_t*>(frame_buffer + sizeof(RF24NetworkHeader));printf_P(PSTR("%lu: NET message %04x\n\r"),millis(),*i));

      // Throw it away if it's not a valid address
      if ( !is_valid_address(header.to_node) ){
		continue;
	  }
	  
	  uint8_t res = header.type;
      // Is this for us?
      if ( header.to_node == node_address   ){
			if(res == NETWORK_ACK){	// If received a routing payload, (Network ACK) discard it, and indicate what it was.
				#ifdef SERIAL_DEBUG_ROUTING
					printf_P(PSTR("MAC: Network ACK Rcvd \n"));
				#endif
				return NETWORK_ACK;
			}								     // Add it to the buffer of frames for us
		
			enqueue();
		
		
	  
	  }else{	  
	  
	  #if defined	(RF24NetworkMulticast)		
			if( header.to_node == 0100){
				if(header.id != lastMultiMessageID){
					if(multicastRelay){					
						#ifdef SERIAL_DEBUG_ROUTING
							printf_P(PSTR("MAC: FWD multicast frame from 0%o to level %d\n"),header.from_node,multicast_level+1);
						#endif
						write(levelToAddress(multicast_level)<<3,4);
					}
				enqueue();				
				lastMultiMessageID = header.id;
				}
				#ifdef SERIAL_DEBUG_ROUTING
				else{				
					printf_P(PSTR("MAC: Drop duplicate multicast frame %d from 0%o\n"),header.id,header.from_node);
				}
				#endif
			}else{
				write(header.to_node,1);	//Send it on, indicate it is a routed payload
			}
		#else
		//if(radio.available()){printf("------FLUSHED DATA --------------");}
		write(header.to_node,1);	//Send it on, indicate it is a routed payload	
		#endif
	  }
	  
	  
      // NOT NEEDED anymore.  Now all reading pipes are open to start.
#if 0
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
#endif
    //}
  }
  
  return 0;
}

/******************************************************************/

bool RF24Network::enqueue(void)
{
  bool result = false;

  IF_SERIAL_DEBUG(printf_P(PSTR("%lu: NET Enqueue @%x "),millis(),next_frame-frame_queue));

  // Copy the current frame into the frame queue
  if ( next_frame < frame_queue + sizeof(frame_queue) )
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

uint16_t RF24Network::parent() const
{
  if ( node_address == 0 )
    return -1;
  else
    return parent_node;
}

/******************************************************************/

void RF24Network::peek(RF24NetworkHeader& header)
{
  if ( available() )
  {
    // Copy the next available frame from the queue into the provided buffer
    memcpy(&header,next_frame-frame_size,sizeof(RF24NetworkHeader));
  }
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

    memcpy(&header,frame,sizeof(RF24NetworkHeader));

    if (maxlen > 0)
    {
      // How much buffer size should we actually copy?
      bufsize = min(maxlen,frame_size-sizeof(RF24NetworkHeader));

      // Copy the next available frame from the queue into the provided buffer
      memcpy(message,frame+sizeof(RF24NetworkHeader),bufsize);
    }

    IF_SERIAL_DEBUG(printf_P(PSTR("%lu: NET Received %s\n\r"),millis(),header.toString()));
  }

  return bufsize;
}

#if defined RF24NetworkMulticast
/******************************************************************/
bool RF24Network::multicast(RF24NetworkHeader& header,const void* message, size_t len, uint8_t level){
	// Fill out the header
  header.to_node = 0100;
  header.from_node = node_address;
  
  // Build the full frame to send
  memcpy(frame_buffer,&header,sizeof(RF24NetworkHeader));
  if (len)
    memcpy(frame_buffer + sizeof(RF24NetworkHeader),message,min(frame_size-sizeof(RF24NetworkHeader),len));

  IF_SERIAL_DEBUG(printf_P(PSTR("%lu: NET Sending %s\n\r"),millis(),header.toString()));
  if (len)
  {
    IF_SERIAL_DEBUG(const uint16_t* i = reinterpret_cast<const uint16_t*>(message);printf_P(PSTR("%lu: NET message %04x\n\r"),millis(),*i));
  }  
    
	return write(levelToAddress(level),4);
  
}
#endif

/******************************************************************/
bool RF24Network::write(RF24NetworkHeader& header,const void* message, size_t len){
	return _write(header,message,len,070);
}
/******************************************************************/
bool RF24Network::write(RF24NetworkHeader& header,const void* message, size_t len, uint16_t writeDirect){
	return _write(header,message,len,writeDirect);
}
/******************************************************************/

bool RF24Network::_write(RF24NetworkHeader& header,const void* message, size_t len, uint16_t writeDirect)
{
  // Fill out the header
  header.from_node = node_address;
  
  // Build the full frame to send
  memcpy(frame_buffer,&header,sizeof(RF24NetworkHeader));
  if (len)
    memcpy(frame_buffer + sizeof(RF24NetworkHeader),message,min(frame_size-sizeof(RF24NetworkHeader),len));

  IF_SERIAL_DEBUG(printf_P(PSTR("%lu: NET Sending %s\n\r"),millis(),header.toString()));
  if (len)
  {
    IF_SERIAL_DEBUG(const uint16_t* i = reinterpret_cast<const uint16_t*>(message);printf_P(PSTR("%lu: NET message %04x\n\r"),millis(),*i));
  }


  // If the user is trying to send it to himself
  if ( header.to_node == node_address ){
    // Just queue it in the received queue
    return enqueue();
  }else{
    // Otherwise send it out over the air	
	if(writeDirect != 070){
		if(header.to_node == writeDirect){
			return write(writeDirect,2);
	    }else{
			return write(writeDirect,3);
		}		
	}else{
	   return write(header.to_node,0); 
	}
  }
}

/******************************************************************/

bool RF24Network::write(uint16_t to_node, uint8_t directTo)  // Direct To: 0 = First Payload, standard routing, 1=routed payload, 2=directRoute to host, 3=directRoute to Route
{
  bool ok = false;
  bool multicast = 0; // Radio ACK requested = 0
  
  // Throw it away if it's not a valid address
  if ( !is_valid_address(to_node) )
    return false;


  // Where do we send this?  By default, to our parent
  uint16_t send_node = parent_node; 

  // On which pipe
  uint8_t send_pipe = parent_pipe %5;
  
  if(directTo == 4){
		send_node = to_node;
		send_pipe = 0;
  }
  
  else if(directTo>1){    
		send_node = to_node;			
  }
     
  // If the node is a direct child,
  else if ( is_direct_child(to_node) )
  {
    // Send directly
    send_node = to_node;

    // To its listening pipe
    send_pipe = 5;
  }
  // If the node is a child of a child
  // talk on our child's listening pipe,
  // and let the direct child relay it.
  else if ( is_descendant(to_node) )
  {
    send_node = direct_child_route_to(to_node);
    send_pipe = 5;
  }
  

  if( ( send_node != to_node) || frame_buffer[6] == NETWORK_ACK || directTo == 3 || directTo == 4){
		multicast = 1;
  }
  

  IF_SERIAL_DEBUG(printf_P(PSTR("%lu: MAC Sending to 0%o via 0%o on pipe %x\n\r"),millis(),to_node,send_node,send_pipe));
  
  #if !defined (DUAL_HEAD_RADIO)
  radio.stopListening();
  #endif
  
  ok=write_to_pipe(send_node, send_pipe, multicast);
  
  #ifdef SERIAL_DEBUG_ROUTING
  if(!ok){	printf_P(PSTR("%lu: MAC Send fail to 0%o via 0%o on pipe %x\n\r"),millis(),to_node,send_node,send_pipe); }
  #endif
 
	if( directTo == 1 && ok && !multicast ){			
			frame_buffer[6] = NETWORK_ACK;								            // Set the payload type to NETWORK_ACK			
			frame_buffer[2] = frame_buffer[0]; frame_buffer[3] = frame_buffer[1];   // Change the 'to' address to the 'from' address
			write(frame_buffer[0] | (frame_buffer[1] << 8),1);						// Send it back as a routed message			
			#if defined (SERIAL_DEBUG_ROUTING)
				//if(!test){
				printf_P(PSTR("MAC: Route OK to 0%o ACK sent to 0%o\n"),to_node,frame_buffer[0] | (frame_buffer[1] << 8));
				//}else{
				//printf("ACK send fail");
				//}
			
			#endif
		}
   
  
 // if(!ok){ printf_P(PSTR("%lu: MAC No Ack from 0%o via 0%o on pipe %x\n\r"),millis(),to_node,send_node,send_pipe); }
 
		
  //printf_P(PSTR("%lu: MAC Sending to 0%o via 0%o on pipe %x\n\r"),millis(),to_node,send_node,send_pipe);
      // NOT NEEDED anymore.  Now all reading pipes are open to start.
#if 0
  // If we are talking on our talking pipe, it's possible that no one is listening.
  // If this fails, try sending it on our parent's listening pipe.  That will wake
  // it up, and next time it will listen to us.

  //if ( !ok && send_node == parent_node )
  //  ok = write_to_pipe( parent_node, 0 );
#endif

#if !defined (DUAL_HEAD_RADIO)
  // Now, continue listening
  radio.startListening();
#endif

	if( (send_node != to_node && directTo==0) || directTo == 3 ){
		uint32_t reply_time = millis(); 
		while( update() != NETWORK_ACK){
			if(millis() - reply_time > routeTimeout){  
				#ifdef SERIAL_DEBUG_ROUTING
					printf_P(PSTR("%lu: MAC Network ACK fail from 0%o on pipe %x\n\r"),millis(),to_node,send_pipe); 
				#endif	
				ok=0;
				break;					
			}
		}		
    }
	
  if(ok == true){
			nOK++;
  }else{	nFails++;
			//printf("Fail to %o",to_node);
  }
  return ok;
}

/******************************************************************/


bool RF24Network::write_to_pipe( uint16_t node, uint8_t pipe, bool multicast )
{
  bool ok = false;
 
  uint64_t out_pipe = pipe_address( node, pipe );

#if !defined (DUAL_HEAD_RADIO)
 // Open the correct pipe for writing.

  // First, stop listening so we can talk
  
  radio.openWritingPipe(out_pipe);  
  radio.writeFast(frame_buffer, frame_size,multicast);
  ok = radio.txStandBy(txTimeout);    
 
#else
  radio1.openWritingPipe(out_pipe);
  radio1.writeFast(frame_buffer, frame_size);
  ok = radio1.txStandBy(txTimeout,multicast);

#endif

  #ifdef __arm__
  IF_SERIAL_DEBUG(printf_P(PSTR("%lu: MAC Sent on %lx %s\n\r"),millis(),(uint32_t)out_pipe,ok?PSTR("ok"):PSTR("failed")));
  #else
  IF_SERIAL_DEBUG(printf_P(PSTR("%lu: MAC Sent on %lx %S\n\r"),millis(),(uint32_t)out_pipe,ok?PSTR("ok"):PSTR("failed")));
  #endif
  
  /*for(int i=0; i<5; i++){
	uint8_t tmp = out_pipe;
	out_pipe=out_pipe >> 8;
	Serial.println(tmp,HEX);
  }*/

  return ok;
}

/******************************************************************/

const char* RF24NetworkHeader::toString(void) const
{
  static char buffer[45];
  //snprintf_P(buffer,sizeof(buffer),PSTR("id %04x from 0%o to 0%o type %c"),id,from_node,to_node,type);
  sprintf_P(buffer,PSTR("id %04x from 0%o to 0%o type %c"),id,from_node,to_node,type);
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
  //parent_pipe = i-1;

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

bool is_valid_address( uint16_t node )
{
  bool result = true;

  while(node)
  {
    uint8_t digit = node & 0B111;
	#if !defined (RF24NetworkMulticast)
    if (digit < 1 || digit > 5)
	#else
	if (digit < 0 || digit > 5)	//Allow our out of range multicast address
	#endif
    {
      result = false;
      printf_P(PSTR("*** WARNING *** Invalid address 0%o\n\r"),node);
      break;
    }
    node >>= 3;
  }

  return result;
}

/******************************************************************/
#if defined (RF24NetworkMulticast)
void RF24Network::multicastLevel(uint8_t level){
  multicast_level = level;
  radio.stopListening();  
  radio.openReadingPipe(0,pipe_address(levelToAddress(level),0));
  radio.startListening();
  }
  
uint16_t levelToAddress(uint8_t level){
	uint16_t levelAddr = 1;
	levelAddr = levelAddr << ((level-1) * 3);
	return levelAddr;
}  
#endif
/******************************************************************/

uint64_t pipe_address( uint16_t node, uint8_t pipe )
{
  
  static uint8_t address_translation[] = { 0xc3,0x3c,0x33,0xce,0x3e,0xe3,0xec };
  uint64_t result = 0xCCCCCCCCCCLL;
  uint8_t* out = reinterpret_cast<uint8_t*>(&result);
  
  // Translate the address to use our optimally chosen radio address bytes
	uint8_t count = 1; uint16_t dec = node;
   #if defined (RF24NetworkMulticast)
	if(pipe != 0 || !node){
   #endif
	while(dec){		
		out[count]=address_translation[(dec % 8)];		// Convert our decimal values to octal, translate them to address bytes, and set our address
		dec /= 8;	
		count++;
	}
 		
	out[0] = address_translation[pipe];		// Set last byte by pipe number
   #if defined (RF24NetworkMulticast)
	}else{
		while(dec){
			dec/=8;
			count++;
		}
		out[1] = address_translation[count-1];	
	}
  
  #endif
  
  IF_SERIAL_DEBUG(uint32_t* top = reinterpret_cast<uint32_t*>(out+1);printf_P(PSTR("%lu: NET Pipe %i on node 0%o has address %lx%x\n\r"),millis(),pipe,node,*top,*out));
	
  return result;
}


/************************ Sleep Mode ******************************************/


#if defined ENABLE_SLEEP_MODE

#if !defined(__arm__)

void wakeUp(){
  sleep_disable();
  sleep_cycles_remaining = 0;
}

ISR(WDT_vect){
  --sleep_cycles_remaining;

}


void RF24Network::sleepNode( unsigned int cycles, int interruptPin ){

  sleep_cycles_remaining = cycles;
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  sleep_enable();
  if(interruptPin != 255){
  	attachInterrupt(interruptPin,wakeUp, LOW);
  }    
  #if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
  WDTCR |= _BV(WDIE);
  #else
  WDTCSR |= _BV(WDIE);
  #endif
  while(sleep_cycles_remaining){
    sleep_mode();                        // System sleeps here
  }                                     // The WDT_vect interrupt wakes the MCU from here
  sleep_disable();                     // System continues execution here when watchdog timed out
  detachInterrupt(interruptPin);
  #if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
	WDTCR &= ~_BV(WDIE);
  #else
	WDTCSR &= ~_BV(WDIE);
  #endif
}

void RF24Network::setup_watchdog(uint8_t prescalar){

  uint8_t wdtcsr = prescalar & 7;
  if ( prescalar & 8 )
    wdtcsr |= _BV(WDP3);
  MCUSR &= ~_BV(WDRF);                      // Clear the WD System Reset Flag

  #if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
  WDTCR = _BV(WDCE) | _BV(WDE);            // Write the WD Change enable bit to enable changing the prescaler and enable system reset
  WDTCR = _BV(WDCE) | wdtcsr | _BV(WDIE);  // Write the prescalar bits (how long to sleep, enable the interrupt to wake the MCU
  #else
  WDTCSR = _BV(WDCE) | _BV(WDE);            // Write the WD Change enable bit to enable changing the prescaler and enable system reset
  WDTCSR = _BV(WDCE) | wdtcsr | _BV(WDIE);  // Write the prescalar bits (how long to sleep, enable the interrupt to wake the MCU
  #endif
}


#endif // not ATTiny
#endif // Enable sleep mode