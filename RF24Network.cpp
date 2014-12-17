/*
 Copyright (C) 2011 James Coliz, Jr. <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */
#include "RF24Network_config.h"

 #if defined (RF24_LINUX)
  #include <stdlib.h>
  #include <stdio.h>
  #include <errno.h>
  #include <fcntl.h>
  #include <sys/mman.h>
  #include <string.h>
  #include <sys/time.h>
  #include <time.h>
  #include <unistd.h>
  #include <iostream>
  #include <algorithm>
  #include <RF24/RF24.h>
  #include "RF24Network.h"
#else  
  #include "RF24.h"
  #include "RF24Network.h"
#endif

#if defined (ENABLE_SLEEP_MODE) && !defined (RF24_LINUX) && !defined (__ARDUINO_X86__)
	#include <avr/sleep.h>
	#include <avr/power.h>
	volatile byte sleep_cycles_remaining;
#endif



uint16_t RF24NetworkHeader::next_id = 1;
uint32_t RF24Network::nFails = 0;
uint32_t RF24Network::nOK = 0;
uint64_t pipe_address( uint16_t node, uint8_t pipe );
#if defined (RF24NetworkMulticast)
uint16_t levelToAddress( uint8_t level );
#endif
bool is_valid_address( uint16_t node );
//uint32_t nFails = 0, nOK=0;
//uint8_t dynLen = 0;

/******************************************************************/
#if defined (RF24_LINUX)
RF24Network::RF24Network( RF24& _radio ): radio(_radio), frame_size(MAX_FRAME_SIZE)
{
}
#elif !defined (DUAL_HEAD_RADIO)
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
  
   radio.stopListening();
  // Set up the radio the way we want it to look
  radio.setChannel(_channel);
  radio.enableDynamicAck();
  radio.enableDynamicPayloads();

  // Use different retry periods to reduce data collisions
  uint8_t retryVar = (((node_address % 6)+1) *2) + 3;
  radio.setRetries(retryVar, 5);
  txTimeout = 30;
  routeTimeout = txTimeout*9; // Adjust for max delay per node


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
  uint8_t count = 0; uint16_t addy = _node_address;
  while(addy) {
    addy/=8;
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
  uint8_t returnVal = 0;
  
  while ( radio.isValid() && radio.available(&pipe_num) )
  {

      frame_size = radio.getDynamicPayloadSize();
	  if(frame_size < sizeof(RF24NetworkHeader)){delay(10);continue;}	  
	  
      // Dump the payloads until we've gotten everything
      // Fetch the payload, and see if this was the last one.
      //radio.read( frame_buffer, sizeof(frame_buffer) );
	  radio.read( frame_buffer, frame_size );
	  
      // Read the beginning of the frame as the header
      RF24NetworkHeader *header = (RF24NetworkHeader*)(&frame_buffer);
	  
	  #if defined (RF24_LINUX)
	    IF_SERIAL_DEBUG(printf_P("%u: MAC Received on %u %s\n\r",millis(),pipe_num,header->toString()));
        if (frame_size) {
          IF_SERIAL_DEBUG_FRAGMENTATION(printf("%u: FRG Rcv frame size %i\n",millis(),frame_size););
          IF_SERIAL_DEBUG_FRAGMENTATION(printf("%u: FRG Rcv frame ",millis()); const char* charPtr = reinterpret_cast<const char*>(frame_buffer); for (size_t i = 0; i < frame_size; i++) { printf("%02X ", charPtr[i]); }; printf("\n\r"));
        }
	  #else
      IF_SERIAL_DEBUG(printf_P(PSTR("%lu: MAC Received on %u %s\n\r"),millis(),pipe_num,header->toString()));
      IF_SERIAL_DEBUG(const uint16_t* i = reinterpret_cast<const uint16_t*>(frame_buffer + sizeof(RF24NetworkHeader));printf_P(PSTR("%lu: NET message %04x\n\r"),millis(),*i));
      #endif
	  
      // Throw it away if it's not a valid address
      if ( !is_valid_address(header->to_node) ){
		continue;
	  }
	  
	  
	  #if defined (RF24_LINUX)
	    RF24NetworkFrame frame = RF24NetworkFrame(*header,frame_buffer+sizeof(RF24NetworkHeader),frame_size-sizeof(RF24NetworkHeader));  
	  #else
	    RF24NetworkFrame frame = RF24NetworkFrame(*header,frame_size-sizeof(RF24NetworkHeader));
	    frame.message_buffer = frame_buffer+sizeof(RF24NetworkHeader);
	  #endif
	  
	  uint8_t res = header->type;
      
	  // Is this for us?
      if ( header->to_node == node_address   ){
			if(res == NETWORK_PING){
			   returnVal = NETWORK_PING;
			   continue;
			}

		    if(header->type == NETWORK_ADDR_RESPONSE ){	
			    uint16_t requester = frame_buffer[8];// | frame_buffer[9] << 8;
				requester |= frame_buffer[9] << 8;				
				if(requester != node_address){
					header->to_node = requester;
					write(header->to_node,USER_TX_TO_PHYSICAL_ADDRESS);
					delay(50);
					write(header->to_node,USER_TX_TO_PHYSICAL_ADDRESS);
					//printf("Fwd add response to 0%o\n",requester);
					continue;
				}
			}
			if(header->type == NETWORK_REQ_ADDRESS && node_address){
				//printf("Fwd add req to 0\n");
				header->from_node = node_address;
				header->to_node = 0;
				write(header->to_node,TX_NORMAL);
				continue;
			}
			
			if( res >127 ){	
				IF_SERIAL_DEBUG_ROUTING( printf_P(PSTR("%lu MAC: System payload rcvd %d\n"),millis(),res); );
				if( (header->type < 148 || header->type > 150) && header->type != NETWORK_MORE_FRAGMENTS_NACK && header->type != EXTERNAL_DATA_TYPE){
					//printf("Type %d\n",header->type);
					return res;
				}				
			}
			
			#if defined (RF24_LINUX)
				enqueue(frame);
			#else
			if( enqueue(frame) == 2 ){ //External data received			
				#if defined (SERIAL_DEBUG_MINIMAL)
				  Serial.println("ret ext");
				#endif
				return EXTERNAL_DATA_TYPE;				
			}
			#endif
			
	  }else{	  

	  #if defined	(RF24NetworkMulticast)		
			if( header->to_node == 0100){
				if(header->id != lastMultiMessageID || (header->type>=NETWORK_FIRST_FRAGMENT && header->type<=NETWORK_LAST_FRAGMENT)){
				  if(header->type == NETWORK_POLL ){
				    //Serial.println("Send poll");
					header->to_node = header->from_node;
					header->from_node = node_address;			
					delay((node_address%5)*5);
					write(header->to_node,USER_TX_TO_PHYSICAL_ADDRESS);
					continue;
				  }else
				  if(multicastRelay){					
					IF_SERIAL_DEBUG_ROUTING( printf_P(PSTR("MAC: FWD multicast frame from 0%o to level %d\n"),header->from_node,multicast_level+1); );
					write(levelToAddress(multicast_level)<<3,4);
				  }
				#if defined (RF24_Linux)
					enqueue(frame);
				#else
				  if( enqueue(frame) == 2 ){ //External data received			
				  //Serial.println("ret ext");
					return EXTERNAL_DATA_TYPE;
				  }
				#endif
				lastMultiMessageID = header->id;
				}
				else{				
					IF_SERIAL_DEBUG_ROUTING( printf_P(PSTR("MAC: Drop duplicate multicast frame %d from 0%o\n"),header->id,header->from_node); );
				}				
			}else{			
				write(header->to_node,1);	//Send it on, indicate it is a routed payload
			}
		#else
		//if(radio.available()){printf("------FLUSHED DATA --------------");}	
		write(header->to_node,1);	//Send it on, indicate it is a routed payload
		#endif
	  }
	  
  }
  return returnVal;
}


#if defined (RF24_LINUX)
/******************************************************************/

uint8_t RF24Network::enqueue(RF24NetworkFrame frame) {
  bool result = false;

  // This is sent to itself
  if (frame.header.from_node == node_address) {
    bool isFragment = ( frame.header.type == NETWORK_FIRST_FRAGMENT || frame.header.type == NETWORK_MORE_FRAGMENTS || frame.header.type == NETWORK_LAST_FRAGMENT || frame.header.type == NETWORK_MORE_FRAGMENTS_NACK);
    if (isFragment) {
      printf("Cannot enqueue multi-payload frames to self\n");
      return false;
    }
    frame_queue.push(frame);
    return true;
  }

  if ((frame.header.reserved > 1 && (frame.header.type == NETWORK_MORE_FRAGMENTS || frame.header.type== NETWORK_FIRST_FRAGMENT || frame.header.type == NETWORK_MORE_FRAGMENTS_NACK)) ) {
    //The received frame contains the a fragmented payload

    //Set the more fragments flag to indicate a fragmented frame
    IF_SERIAL_DEBUG_FRAGMENTATION(printf("%u: FRG fragmented payload of size %i Bytes with fragmentID '%i' received.\n\r",millis(),frame.message_size,frame.header.reserved););

    //Append payload
    appendFragmentToFrame(frame);
    result = true;

  } else if ( frame.header.type == NETWORK_LAST_FRAGMENT) {
    //The last fragment flag indicates that we received the last fragment
    //Workaround/Hack: In this case the header.reserved does not count the number of fragments but is a copy of the type flags specified by the user application.
    
    IF_SERIAL_DEBUG_FRAGMENTATION(printf("%u: FRG Last fragment with size %i Bytes and fragmentID '%i' received.\n\r",millis(),frame.message_size,frame.header.reserved););
    //Append payload
    appendFragmentToFrame(frame);

    IF_SERIAL_DEBUG(printf_P(PSTR("%u: NET Enqueue assembled frame @%x "),millis(),frame_queue.size()));

    //Push the assembled frame in the frame_queue and remove it from cache
	RF24NetworkFrame *f = &(frameFragmentsCache[ std::make_pair(frame.header.from_node,frame.header.id) ]);
	
    // If total_fragments == 0, we are finished
    if ( frame.header.to_node == node_address && f->message_size > 0){//|| (!frame.header.reserved && frameFragmentsCache.count(std::make_pair(frame.header.from_node,frame.header.id)) ) ) {
      frame_queue.push( frameFragmentsCache[ std::make_pair(frame.header.from_node,frame.header.id) ] );	  
	  
    } else {
      IF_SERIAL_DEBUG_MINIMAL( printf("%u: NET Dropped frame missing fragments\n",millis() ););
	  
    }

    frameFragmentsCache.erase( std::make_pair(frame.header.from_node,frame.header.id) );

    result = true;

  } else{//  if (frame.header.type <= MAX_USER_DEFINED_HEADER_TYPE) {
    //This is not a fragmented payload but a whole frame.

    IF_SERIAL_DEBUG(printf_P(PSTR("%u: NET Enqueue @%x "),millis(),frame_queue.size()));
    // Copy the current frame into the frame queue
    frame_queue.push(frame);
    result = true;

  }/* else {
    //Undefined/Unknown header.type received. Drop frame!
    IF_SERIAL_DEBUG_MINIMAL( printf("%u: FRG Received unknown or system header type %d with fragment id %d\n",millis(),frame.header.type, frame.header.reserved); );
    //The frame is not explicitly dropped, but the given object is ignored.
    //FIXME: does this causes problems with memory management?
  }*/

  if (result) {
    IF_SERIAL_DEBUG(printf("ok\n\r"));
  } else {
    IF_SERIAL_DEBUG(printf("failed\n\r"));
  }

  return result;
}

/******************************************************************/

void RF24Network::appendFragmentToFrame(RF24NetworkFrame frame) {

  
  if(frameFragmentsCache.size() >= 7){ frameFragmentsCache.erase(frameFragmentsCache.begin()); }
  if (frameFragmentsCache.count(std::make_pair(frame.header.from_node,frame.header.id)) == 0 ) {
    // This is the first of many fragments.

    if (frame.header.type == NETWORK_FIRST_FRAGMENT || ( frame.header.type != NETWORK_MORE_FRAGMENTS_NACK && frame.header.type != NETWORK_MORE_FRAGMENTS && frame.header.type != NETWORK_LAST_FRAGMENT)) {
      // Decrement the stored total_fragments counter
      frame.header.reserved = frame.header.reserved-1;
      // Cache the fragment
      frameFragmentsCache[ std::make_pair(frame.header.from_node,frame.header.id) ] = frame;
    } else {
        IF_SERIAL_DEBUG_MINIMAL(printf("%u: FRG Dropping all fragments for frame with header id:%d, missing first fragment(s).\n",millis(),frame.header.id););
    }

  } else {
    //We have at least received one fragments.
    //Append payload

    RF24NetworkFrame *f = &(frameFragmentsCache[ std::make_pair(frame.header.from_node,frame.header.id) ]);

    if (frame.header.type == NETWORK_LAST_FRAGMENT) {
      //Workaround/Hack: The user application specified header.type is sent with the last fragment in the reserved field
      frame.header.type = frame.header.reserved;
      frame.header.reserved = 1;
    }

    //Error checking for missed fragments and payload size
    if (frame.header.reserved != f->header.reserved) {
        if (frame.header.reserved > f->header.reserved) {
            IF_SERIAL_DEBUG_MINIMAL(printf("%u: FRG Duplicate or out of sequence frame %d, expected %d. Cleared.\n",millis(),frame.header.reserved,f->header.reserved););
            //frameFragmentsCache.erase( std::make_pair(frame.header.from_node,frame.header.id) );
        } else {
            frameFragmentsCache.erase( std::make_pair(frame.header.from_node,frame.header.id) );
            IF_SERIAL_DEBUG_MINIMAL(printf("%u: FRG Out of sequence frame %d, expected %d. Cleared.\n",millis(),frame.header.reserved,f->header.reserved););
        }
        return;
    }

    //Check if the total payload exceeds the MAX_PAYLOAD_SIZE.
    //If so, drop the assembled frame.
    if (frame.message_size + f->message_size > MAX_PAYLOAD_SIZE) {
        frameFragmentsCache.erase( std::make_pair(frame.header.from_node,frame.header.id) );
        IF_SERIAL_DEBUG_MINIMAL(printf("%u: FRM Payload of %d exceeds MAX_PAYLOAD_SIZE %d Bytes. Frame dropped\n",millis(),frame.message_size + f->message_size,MAX_PAYLOAD_SIZE););
        return;
    }

    //Append the received fragment to the cached frame
    memcpy(f->message_buffer+f->message_size, frame.message_buffer, frame.message_size);
    f->message_size += frame.message_size;  //Increment message size
    f->header = frame.header; //Update header
    f->header.reserved--;  //Update Total fragments
  }
}

/******************************************************************/
/******************************************************************/

#else // Not defined RF24_Linux:

/******************************************************************/
/******************************************************************/

uint8_t RF24Network::enqueue(RF24NetworkFrame frame)
{
  bool result = false;
  
  IF_SERIAL_DEBUG(printf_P(PSTR("%lu: NET Enqueue @%x "),millis(),next_frame-frame_queue));

#if !defined ( DISABLE_FRAGMENTATION ) 

  bool isFragment = frame.header.type >=148 && frame.header.type <=150 ? true : false;

  if(isFragment){

	frag_queue.message_buffer=&frag_queue_message_buffer[0];
	
	if(frame.header.type == NETWORK_FIRST_FRAGMENT){
	    if(frame.header.reserved *24 > MAX_PAYLOAD_SIZE){
		// TUN:
		//if(frame.header.reserved *24 > MAX_PAYLOAD_SIZE + 28){
#if defined (SERIAL_DEBUG_FRAGMENTATION) || defined (SERIAL_DEBUG_MINIMAL)		
			printf(PSTR("Frag frame with %d frags exceeds MAX_PAYLOAD_SIZE\n"),frame.header.reserved);
#endif
			frag_queue.header.reserved = 0;
			return 0;
		}
#if defined (SERIAL_DEBUG_FRAGMENTATION)
		printf(PSTR("queue first, total frags %d\n"),frame.header.reserved);
#endif
		memcpy(&frag_queue,&frame,10);
		memcpy(frag_queue.message_buffer,frame_buffer+sizeof(RF24NetworkHeader),frame.message_size);
#if defined (SERIAL_DEBUG_FRAGMENTATION_L2)		
		for(int i=0; i<frag_queue.message_size;i++){
			Serial.println(frag_queue.message_buffer[i],HEX);
		}
#endif		
		//Store the total size of the stored frame in message_size
	    frag_queue.message_size = frame.message_size;
		frag_queue.header.reserved = frag_queue.header.reserved - 1;
		return true;
	}else
	if(frame.header.type == NETWORK_MORE_FRAGMENTS || frame.header.type == NETWORK_MORE_FRAGMENTS_NACK){
	    if(frame.header.reserved != frag_queue.header.reserved){
#if defined (SERIAL_DEBUG_FRAGMENTATION) || defined (SERIAL_DEBUG_MINIMAL)
		printf(PSTR("1 Dropped out of order frame %d expected %d\n"),frame.header.reserved,frag_queue.header.reserved);
#endif
			frag_queue.header.reserved = 0;
			return 0;
		}
		memcpy(frag_queue.message_buffer+frag_queue.message_size,frame_buffer+sizeof(RF24NetworkHeader),frame.message_size);
		frag_queue.message_size += frame.message_size;
		frag_queue.header.reserved--;
		return true;
	}else
	if(frame.header.type == NETWORK_LAST_FRAGMENT){
		
		if(frag_queue.header.reserved != 1 || frag_queue.header.id != frame.header.id){
			#if defined (SERIAL_DEBUG_FRAGMENTATION) || defined (SERIAL_DEBUG_MINIMAL)
			printf(PSTR("2 Dropped out of order frame frag %d header id %d expected last (1) \n"),frame.header.reserved,frame.header.id);
			#endif
			frag_queue.header.reserved = 0;
			return 0;
		}
		frag_queue.header.reserved = 0;

		memcpy(frag_queue.message_buffer+frag_queue.message_size,frame_buffer+sizeof(RF24NetworkHeader),frame.message_size);
				
		frag_queue.message_size += frame.message_size;		
#if defined (SERIAL_DEBUG_FRAGMENTATION)
		printf(PSTR("fq 3: %d\n"),frag_queue.message_size);
#endif
#if defined (SERIAL_DEBUG_FRAGMENTATION_L2)
		for(int i=0; i< frag_queue.message_size;i++){
			Serial.println(frag_queue.message_buffer[i],HEX);
		}
#endif		
	
		//Frame assembly complete, copy to main buffer if OK	
		
		if (  next_frame < frame_queue + sizeof(frame_queue)  && frame.message_size <= MAX_PAYLOAD_SIZE)
		{
			//Set the type on the incoming message header, as well as the received message
			frame.header.type = frame.header.reserved;
			frag_queue.header.type = frame.header.reserved;
		
			if(frame.header.type == EXTERNAL_DATA_TYPE){
				frag_ptr = &frag_queue;
				return 2;
			}else{
			#if defined (DISABLE_USER_PAYLOADS)
				return 0;
			#endif		
				if( frag_queue.message_size <  sizeof(frame_queue)-(next_frame-frame_queue)){
				memcpy(next_frame,&frag_queue,10);
				memcpy(next_frame+10,frag_queue.message_buffer,frag_queue.message_size);
				next_frame += (10+frag_queue.message_size);
				}
			}
		#if defined (SERIAL_DEBUG_FRAGMENTATION)
			printf(PSTR("enq size %d\n"),frag_queue.message_size);
		#endif
		   return true;
		}
  
	}//If last Fragment
  
  }else //else is not a fragment
 #endif 
  
  // Copy the current frame into the frame queue
  if (  next_frame < frame_queue + sizeof(frame_queue) && frame.message_size <= MAX_FRAME_SIZE)
  {

#if !defined( DISABLE_FRAGMENTATION )
	frag_ptr = &frag_queue;

	if(frame.header.type == EXTERNAL_DATA_TYPE){
		memcpy(&frag_queue,&frame,10);
		frag_queue.message_buffer = frame_buffer+sizeof(RF24NetworkHeader);
		frag_queue.message_size = frame.message_size;
		return 2;
	}
#endif		
#if defined (DISABLE_USER_PAYLOADS)
	return 0;
#endif
	memcpy(next_frame,&frame,10);
	memcpy(next_frame+10,frame_buffer+sizeof(RF24NetworkHeader),frame.message_size);
#if defined (SERIAL_DEBUG_FRAGMENTATION)
	for(int i=0; i<frame.message_size+10;i++){
		Serial.print(frame_queue[i],HEX);
		Serial.print(" : ");
	}
	Serial.println("");
#endif
    next_frame += (frame.message_size + 10); 
#if defined (SERIAL_DEBUG_FRAGMENTATION)
	printf(PSTR("enq %d\n"),next_frame-frame_queue);
#endif
    result = true;
    IF_SERIAL_DEBUG(printf_P(PSTR("ok\n\r")));
  }
  else
  {
    IF_SERIAL_DEBUG(printf_P(PSTR("failed\n\r")));
	
  }
  return result;
}

#endif //End not defined RF24_Linux
/******************************************************************/

bool RF24Network::available(void)
{
#if defined (RF24_LINUX)
  return (!frame_queue.empty());
#else
  // Are there frames on the queue for us?
  return (next_frame > frame_queue);
#endif
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
/*uint8_t RF24Network::peekData(){
		
		return frame_queue[0];
}*/

size_t RF24Network::peek(RF24NetworkHeader& header)
{
  if ( available() )
  {
  #if defined (RF24_LINUX)
    RF24NetworkFrame frame = frame_queue.front();
    memcpy(&header,&frame,sizeof(RF24NetworkHeader));
    return frame.message_size;
  #else
	memcpy(&header,frame_queue,sizeof(RF24NetworkHeader));
	RF24NetworkFrame& frame = * reinterpret_cast<RF24NetworkFrame*>(frame_queue);
	return frame.message_size;
  #endif
  }
  return 0;
}

/******************************************************************/

size_t RF24Network::read(RF24NetworkHeader& header,void* message, size_t maxlen)
{
  size_t bufsize = 0;

 #if defined (RF24_LINUX)
   if ( available() ) {
    RF24NetworkFrame frame = frame_queue.front();

    // How much buffer size should we actually copy?
    bufsize = std::min(frame.message_size,maxlen);
    memcpy(&header,&(frame.header),sizeof(RF24NetworkHeader));
    memcpy(message,frame.message_buffer,bufsize);

    IF_SERIAL_DEBUG(printf("%u: FRG message size %i\n",millis(),frame.message_size););
    IF_SERIAL_DEBUG(printf("%u: FRG message ",millis()); const char* charPtr = reinterpret_cast<const char*>(message); for (size_t i = 0; i < bufsize; i++) { printf("%02X ", charPtr[i]); }; printf("\n\r"));	
	
    IF_SERIAL_DEBUG(printf_P(PSTR("%u: NET read %s\n\r"),millis(),header.toString()));

    frame_queue.pop();
  }
#else  
  if ( available() )
  {
    
	memcpy(&header,frame_queue,8);

	bufsize = frame_queue[8];
	bufsize |= frame_queue[9] << 8;
	
    if (maxlen > 0)
    {		
		maxlen = min(maxlen,bufsize);
		memcpy(message,frame_queue+10,maxlen);
	    IF_SERIAL_DEBUG(printf("%lu: NET message size %d\n",millis(),bufsize););

	size_t len = bufsize;
	IF_SERIAL_DEBUG(printf_P(PSTR("%lu: NET r message "),millis());const uint8_t* charPtr = reinterpret_cast<const uint8_t*>(message);while(len--){ printf("%02x ",charPtr[len]);} printf_P(PSTR("\n\r") ) );      
	  
    }
	bufsize+=10;
	memmove(frame_queue,frame_queue+bufsize,sizeof(frame_queue)- bufsize);
	next_frame-=bufsize;

	//IF_SERIAL_DEBUG(printf_P(PSTR("%lu: NET Received %s\n\r"),millis(),header.toString()));
  }
#endif
  return bufsize;
}


#if defined RF24NetworkMulticast
/******************************************************************/
bool RF24Network::multicast(RF24NetworkHeader& header,const void* message, size_t len, uint8_t level){
	// Fill out the header
  header.to_node = 0100;
  header.from_node = node_address;
  
  /*// Build the full frame to send
  memcpy(frame_buffer,&header,sizeof(RF24NetworkHeader));
  if (len)
    memcpy(frame_buffer + sizeof(RF24NetworkHeader),message,min(frame_size-sizeof(RF24NetworkHeader),len));

  IF_SERIAL_DEBUG(printf_P(PSTR("%lu: NET Sending %s\n\r"),millis(),header.toString()));
  if (len)
  {
    IF_SERIAL_DEBUG(const uint16_t* i = reinterpret_cast<const uint16_t*>(message);printf_P(PSTR("%lu: NET message %04x\n\r"),millis(),*i));
  }  
    
	return write(levelToAddress(level),USER_TX_MULTICAST);*/
    return write(header, message, len, levelToAddress(level));
}
#endif

/******************************************************************/
bool RF24Network::write(RF24NetworkHeader& header,const void* message, size_t len){
	return write(header,message,len,070);
}
/******************************************************************/
bool RF24Network::write(RF24NetworkHeader& header,const void* message, size_t len, uint16_t writeDirect){
    frame_size=sizeof(RF24NetworkHeader)+len;
	frame_size=rf24_min(frame_size,MAX_FRAME_SIZE);
#if defined (DISABLE_FRAGMENTATION)
	return _write(header,message,len,writeDirect);
#else  

  //Check payload size
  if (len > MAX_PAYLOAD_SIZE) {
    IF_SERIAL_DEBUG(printf("%u: NET write message failed. Given 'len' is bigger than the MAX Payload size %i\n\r",millis(),MAX_PAYLOAD_SIZE););
    return false;
  }
	
  //If the message payload is too big, whe cannot generate enough fragments
  //and enumerate them
  if (len > 255*max_frame_payload_size) {
    return false;
  }

  //Normal Write (Un-Fragmented)
  if (len <= max_frame_payload_size) {
    return _write(header,message,len,writeDirect);
  }  
  
  //Divide the message payload into chuncks of max_frame_payload_size
  uint8_t fragment_id = 1 + ((len - 1) / max_frame_payload_size);  //the number of fragments to send = ceil(len/max_frame_payload_size)
  uint8_t msgCount = 0;

  IF_SERIAL_DEBUG_FRAGMENTATION(printf("%lu: FRG Total message fragments %d\n\r",millis(),fragment_id););
  
  while (fragment_id > 0) {

    //Copy and fill out the header
    RF24NetworkHeader fragmentHeader = header;
    fragmentHeader.reserved = fragment_id;

    if (fragment_id == 1) {
      fragmentHeader.type = NETWORK_LAST_FRAGMENT;  //Set the last fragment flag to indicate the last fragment
      fragmentHeader.reserved = header.type; //Workaroung/Hack: to transmit the user application defined header.type, save this variable in the header.fragment_id.
    } else {
      if (msgCount == 0) {
        fragmentHeader.type = NETWORK_FIRST_FRAGMENT;
      }else{
        fragmentHeader.type = NETWORK_MORE_FRAGMENTS; //Set the more fragments flag to indicate a fragmented frame
      }
    }
	
    size_t offset = msgCount*max_frame_payload_size;
    size_t fragmentLen = rf24_min(len-offset,max_frame_payload_size);

   // IF_SERIAL_DEBUG_FRAGMENTATION(printf("%lu: FRG try to transmit fragmented payload of size %d Bytes with fragmentID '%d'\n\r",millis(),fragmentLen,fragment_id););

    //Try to send the payload chunk with the copied header
    frame_size = sizeof(RF24NetworkHeader)+fragmentLen;
	bool ok = _write(fragmentHeader,((char *)message)+offset,fragmentLen,writeDirect);

    if(writeDirect != 070){ delay(3); } //Delay 5ms between sending multicast payloads
    
	if (!ok) {
        IF_SERIAL_DEBUG_FRAGMENTATION(printf("%lu: FRG message transmission with fragmentID '%d' failed. Abort.\n\r",millis(),fragment_id););
        return false;
    }

    //Message was successful sent
    IF_SERIAL_DEBUG_FRAGMENTATION(printf("%lu: FRG message transmission with fragmentID '%d' sucessfull.\n\r",millis(),fragment_id););

    //Check and modify counters
    fragment_id--;
    msgCount++;
  }

  int frag_delay = uint8_t(len/48);
  delay( rf24_min(frag_delay,20));

  //Return true if all the chunks where sent successfully
  //IF_SERIAL_DEBUG(printf("%u: NET total message fragments sent %i. txSuccess ",millis(),msgCount); printf("%s\n\r", txSuccess ? "YES" : "NO"););
  return true;
  
#endif //Fragmentation enabled
}
/******************************************************************/

bool RF24Network::_write(RF24NetworkHeader& header,const void* message, size_t len, uint16_t writeDirect)
{
  // Fill out the header
  header.from_node = node_address;
  
  // Build the full frame to send
  memcpy(frame_buffer,&header,sizeof(RF24NetworkHeader));
  
  #if defined (RF24_LINUX)
	IF_SERIAL_DEBUG(printf_P(PSTR("%u: NET Sending %s\n\r"),millis(),header.toString()));
  #else
    IF_SERIAL_DEBUG(printf_P(PSTR("%lu: NET Sending %s\n\r"),millis(),header.toString()));
  #endif
  if (len){
    #if defined (RF24_LINUX)
	memcpy(frame_buffer + sizeof(RF24NetworkHeader),message,rf24_min(frame_size-sizeof(RF24NetworkHeader),len));
    IF_SERIAL_DEBUG(printf("%u: FRG frame size %i\n",millis(),frame_size););
    IF_SERIAL_DEBUG(printf("%u: FRG frame ",millis()); const char* charPtr = reinterpret_cast<const char*>(frame_buffer); for (size_t i = 0; i < frame_size; i++) { printf("%02X ", charPtr[i]); }; printf("\n\r"));
	#else
    memcpy(frame_buffer + sizeof(RF24NetworkHeader),message,rf24_min(frame_size-sizeof(RF24NetworkHeader),len));
	
	IF_SERIAL_DEBUG(size_t tmpLen = len;printf_P(PSTR("%lu: NET message "),millis());const uint8_t* charPtr = reinterpret_cast<const uint8_t*>(message);while(tmpLen--){ printf("%02x ",charPtr[tmpLen]);} printf_P(PSTR("\n\r") ) );
    #endif
  }

  // If the user is trying to send it to himself
  if ( header.to_node == node_address ){
	#if defined (RF24_LINUX)
	  RF24NetworkFrame frame = RF24NetworkFrame(header,message,rf24_min(MAX_FRAME_SIZE-sizeof(RF24NetworkHeader),len));	
	#else
      RF24NetworkFrame frame(header,len);
    #endif
	// Just queue it in the received queue
    return enqueue(frame);
  }
    // Otherwise send it out over the air	
	if(writeDirect != 070){
		if(header.to_node == writeDirect){
			return write(writeDirect,USER_TX_TO_PHYSICAL_ADDRESS);
	    }else{
			return write(writeDirect,USER_TX_TO_LOGICAL_ADDRESS);
		}		
	}else{
	   return write(header.to_node,TX_NORMAL); 
	}
  
}

/******************************************************************/

bool RF24Network::write(uint16_t to_node, uint8_t directTo)  // Direct To: 0 = First Payload, standard routing, 1=routed payload, 2=directRoute to host, 3=directRoute to Route
{
  bool ok = false;
  
  // Throw it away if it's not a valid address
  if ( !is_valid_address(to_node) )
    return false;  
  
  //Load info into our conversion structure, and get the converted address info
  logicalToPhysicalStruct conversion = { to_node,directTo,0};
  logicalToPhysicalAddress(&conversion);
  
  #if defined (RF24_LINUX)
  IF_SERIAL_DEBUG(printf_P(PSTR("%u: MAC Sending to 0%o via 0%o on pipe %x\n\r"),millis(),to_node,conversion.send_node,conversion.send_pipe));
  #else
  IF_SERIAL_DEBUG(printf_P(PSTR("%lu: MAC Sending to 0%o via 0%o on pipe %x\n\r"),millis(),to_node,conversion.send_node,conversion.send_pipe));
  #endif
  /**Write it*/
  ok=write_to_pipe(conversion.send_node, conversion.send_pipe, conversion.multicast);  	
  
  
  if(!ok){	
    #if defined (RF24_LINUX)
    IF_SERIAL_DEBUG_ROUTING( printf_P(PSTR("%u: MAC Send fail to 0%o via 0%o on pipe %x\n\r"),millis(),to_node,conversion.send_node,conversion.send_pipe);); }
	#else
	IF_SERIAL_DEBUG_ROUTING( printf_P(PSTR("%lu: MAC Send fail to 0%o via 0%o on pipe %x\n\r"),millis(),to_node,conversion.send_node,conversion.send_pipe);); }
	#endif
 
	if( directTo == TX_ROUTED && ok && conversion.send_node == to_node && frame_buffer[6] > 64 && frame_buffer[6] < 192){
			
			RF24NetworkHeader& header = * reinterpret_cast<RF24NetworkHeader*>(frame_buffer);
			header.type = NETWORK_ACK;				    // Set the payload type to NETWORK_ACK			
			header.to_node = header.from_node;          // Change the 'to' address to the 'from' address			

			conversion.send_node = header.from_node;
			conversion.send_pipe = TX_ROUTED;
			conversion.multicast = 0;
			logicalToPhysicalAddress(&conversion);
			
			//Write the data using the resulting physical address
			write_to_pipe(conversion.send_node, conversion.send_pipe, conversion.multicast);
			
			//dynLen=0;
			#if defined (RF24_LINUX)
				IF_SERIAL_DEBUG_ROUTING( printf_P(PSTR("%u MAC: Route OK to 0%o ACK sent to 0%o\n"),millis(),to_node,header.to_node); );
			#else
			    IF_SERIAL_DEBUG_ROUTING( printf_P(PSTR("%lu MAC: Route OK to 0%o ACK sent to 0%o\n"),millis(),to_node,header.to_node); );
			#endif
	}

 
#if !defined (DUAL_HEAD_RADIO)
  // Now, continue listening
  radio.startListening();
#endif

	if( ok && conversion.send_node != to_node && (directTo==0 || directTo==3) && frame_buffer[6] > 64 && frame_buffer[6] < 192){
		uint32_t reply_time = millis(); 
		while( update() != NETWORK_ACK){
			if(millis() - reply_time > routeTimeout){
				#if defined (RF24_LINUX)
				  IF_SERIAL_DEBUG_ROUTING( printf_P(PSTR("%u: MAC Network ACK fail from 0%o via 0%o on pipe %x\n\r"),millis(),to_node,conversion.send_node,conversion.send_pipe); );
				#else
				  IF_SERIAL_DEBUG_ROUTING( printf_P(PSTR("%lu: MAC Network ACK fail from 0%o via 0%o on pipe %x\n\r"),millis(),to_node,conversion.send_node,conversion.send_pipe); );
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

	// Provided the to_node and directTo option, it will return the resulting node and pipe
bool RF24Network::logicalToPhysicalAddress(logicalToPhysicalStruct *conversionInfo){

  //Create pointers so this makes sense.. kind of
  //We take in the to_node(logical) now, at the end of the function, output the send_node(physical) address, etc.
  //back to the original memory address that held the logical information.
  uint16_t *to_node = &conversionInfo->send_node;
  uint8_t *directTo = &conversionInfo->send_pipe;
  bool *multicast = &conversionInfo->multicast;    
  
  // Where do we send this?  By default, to our parent
  uint16_t pre_conversion_send_node = parent_node; 

  // On which pipe
  uint8_t pre_conversion_send_pipe = parent_pipe %5;
  
 if(*directTo > TX_ROUTED ){    
	pre_conversion_send_node = *to_node;
	*multicast = 1;	
	if(*directTo == USER_TX_MULTICAST){
		pre_conversion_send_pipe=0;
	}	
  }     
  // If the node is a direct child,
  else
  if ( is_direct_child(*to_node) )
  {
    // Send directly
    pre_conversion_send_node = *to_node;
    // To its listening pipe
    pre_conversion_send_pipe = 5;
  }
  // If the node is a child of a child
  // talk on our child's listening pipe,
  // and let the direct child relay it.
  else if ( is_descendant(*to_node) )
  {
    pre_conversion_send_node = direct_child_route_to(*to_node);
    pre_conversion_send_pipe = 5;
  }
  
  *to_node = pre_conversion_send_node;
  *directTo = pre_conversion_send_pipe;
  
  return 1;
  
}

/********************************************************/


bool RF24Network::write_to_pipe( uint16_t node, uint8_t pipe, bool multicast )
{
  bool ok = false;

  uint64_t out_pipe = pipe_address( node, pipe );

#if !defined (DUAL_HEAD_RADIO)
 // Open the correct pipe for writing.
  // First, stop listening so we can talk
  radio.stopListening();
  radio.openWritingPipe(out_pipe);  
  radio.writeFast(frame_buffer, frame_size,multicast);
  ok = radio.txStandBy(txTimeout);

#else
  radio1.openWritingPipe(out_pipe);
  radio1.writeFast(frame_buffer, frame_size);
  ok = radio1.txStandBy(txTimeout,multicast);

#endif

  #if defined (__arm__) || defined (RF24_LINUX)
  IF_SERIAL_DEBUG(printf_P(PSTR("%u: MAC Sent on %x %s\n\r"),millis(),(uint32_t)out_pipe,ok?PSTR("ok"):PSTR("failed")));
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
  sprintf_P(buffer,PSTR("id %u from 0%o to 0%o type %d"),id,from_node,to_node,type);
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


  IF_SERIAL_DEBUG( printf_P(PSTR("setup_address node=0%o mask=0%o parent=0%o pipe=0%o\n\r"),node_address,node_mask,parent_node,parent_pipe););

}

/******************************************************************/
uint16_t RF24Network::addressOfPipe( uint16_t node, uint8_t pipeNo )
{
		//Say this node is 013 (1011), mask is 077 or (00111111)
		//Say we want to use pipe 3 (11)
        //6 bits in node mask, so shift pipeNo 6 times left and | into address		
	uint16_t m = node_mask >> 3;
	uint8_t i=0;
	
	while (m){ 	   //While there are bits left in the node mask
		m>>=1;     //Shift to the right
		i++;       //Count the # of increments
	}
    return node | (pipeNo << i);	
}

/******************************************************************/

uint16_t RF24Network::direct_child_route_to( uint16_t node )
{
  // Presumes that this is in fact a child!!
  uint16_t child_mask = ( node_mask << 3 ) | 0B111;
  return node & child_mask;
  
}

/******************************************************************/
/*
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
}*/

/******************************************************************/

bool RF24Network::is_valid_address( uint16_t node )
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
      IF_SERIAL_DEBUG_MINIMAL(printf_P(PSTR("*** WARNING *** Invalid address 0%o\n\r"),node););
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
	if(level){
		levelAddr = levelAddr << ((level-1) * 3);
	}else{
		return 0;		
	}
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
  
  #if defined (RF24_LINUX)
  IF_SERIAL_DEBUG(uint32_t* top = reinterpret_cast<uint32_t*>(out+1);printf_P(PSTR("%u: NET Pipe %i on node 0%o has address %x%x\n\r"),millis(),pipe,node,*top,*out));
  #else
  IF_SERIAL_DEBUG(uint32_t* top = reinterpret_cast<uint32_t*>(out+1);printf_P(PSTR("%lu: NET Pipe %i on node 0%o has address %lx%x\n\r"),millis(),pipe,node,*top,*out));
  #endif
  
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
