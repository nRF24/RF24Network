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
  
  #if !defined (DISABLE_FRAGMENTATION)
  //*frag_queue.message_buffer = &frag_queue_message_buffer[0];
  #endif
  radio.stopListening();
  // Set up the radio the way we want it to look
  radio.setChannel(_channel);
  //radio.setDataRate(RF24_1MBPS);
  //radio.setCRCLength(RF24_CRC_16);
  radio.enableDynamicAck();
  radio.enableDynamicPayloads();
  // Use different retry periods to reduce data collisions

    uint8_t retryVar = (((node_address % 6)+1) *2) + 3;
  radio.setRetries(retryVar, 5);
  txTimeout = 30;
  routeTimeout = txTimeout*9; // Adjust for max delay per node

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
  while(_node_address) {
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
  uint8_t returnVal = 0;
  
  while ( radio.isValid() && radio.available())//&pipe_num) )
  {

  
      frame_size = radio.getDynamicPayloadSize();
	  if(!frame_size){delay(5);continue;}
	  
      // Dump the payloads until we've gotten everything
      // Fetch the payload, and see if this was the last one.
      //radio.read( frame_buffer, sizeof(frame_buffer) );
	  radio.read( frame_buffer, frame_size );

      // Read the beginning of the frame as the header
      RF24NetworkHeader& header = * reinterpret_cast<RF24NetworkHeader*>(frame_buffer);
      IF_SERIAL_DEBUG(printf_P(PSTR("%lu: MAC Received on %u %s\n\r"),millis(),pipe_num,header.toString()));
      IF_SERIAL_DEBUG(const uint16_t* i = reinterpret_cast<const uint16_t*>(frame_buffer + sizeof(RF24NetworkHeader));printf_P(PSTR("%lu: NET message %04x\n\r"),millis(),*i));

      // Throw it away if it's not a valid address
      if ( !is_valid_address(header.to_node) ){
		continue;
	  }
	  
	  RF24NetworkFrame frame = RF24NetworkFrame(header,frame_size-sizeof(RF24NetworkHeader),0);
	  frame.message_buffer = frame_buffer;
	  
	  uint8_t res = header.type;
      // Is this for us?
      if ( header.to_node == node_address   ){
			
			if(res == NETWORK_PING){
			   returnVal = NETWORK_PING;
			   continue;
			}

		    if(header.type == NETWORK_ADDR_RESPONSE ){	
			    uint16_t requester = frame_buffer[8];// | frame_buffer[9] << 8;
				requester |= frame_buffer[9] << 8;				
				if(requester != node_address){
					header.to_node = requester;
					write(header.to_node,USER_TX_TO_PHYSICAL_ADDRESS);
					delay(50);
					write(header.to_node,USER_TX_TO_PHYSICAL_ADDRESS);
					printf("Fwd add response to 0%o\n",requester);
					continue;
				}
			}
			if(header.type == NETWORK_REQ_ADDRESS && node_address){
				printf("Fwd add req to 0\n");
				header.from_node = node_address;
				header.to_node = 0;
				write(header.to_node,TX_NORMAL);
				continue;
			}
			
			if( res >127 ){	
				IF_SERIAL_DEBUG_ROUTING( printf_P(PSTR("%lu MAC: System payload rcvd %d\n"),millis(),res); );
				if( (header.type < 148 || header.type > 150) && header.type != NETWORK_MORE_FRAGMENTS_NACK){
					//printf("Type %d\n",header.type);
					return res;
				}				
			}
			
			if( enqueue(frame) == 2 ){ //External data received			
				#if defined (SERIAL_DEBUG_MINIMAL)
				//Serial.println("ret ext");
				#endif
				return EXTERNAL_DATA_TYPE;
				
			}
			
	  }else{	  
	  
	  #if defined	(RF24NetworkMulticast)		
			if( header.to_node == 0100){
				if(header.id != lastMultiMessageID || (header.type>=NETWORK_FIRST_FRAGMENT && header.type<=NETWORK_LAST_FRAGMENT)){
					if(multicastRelay){					
						IF_SERIAL_DEBUG_ROUTING( printf_P(PSTR("MAC: FWD multicast frame from 0%o to level %d\n"),header.from_node,multicast_level+1); );
						write(levelToAddress(multicast_level)<<3,4);
					}

				if(header.type == NETWORK_POLL ){
				    //Serial.println("Send poll");
					header.to_node = header.from_node;
					header.from_node = node_address;			
					delay((node_address%5)*5);
					write(header.to_node,USER_TX_TO_PHYSICAL_ADDRESS);
					continue;
				}
				
				if( enqueue(frame) == 2 ){ //External data received			
				//Serial.println("ret ext");
					return EXTERNAL_DATA_TYPE;
				}				
				lastMultiMessageID = header.id;
				}
				else{				
					IF_SERIAL_DEBUG_ROUTING( printf_P(PSTR("MAC: Drop duplicate multicast frame %d from 0%o\n"),header.id,header.from_node); );
				}				
			}else{			
				write(header.to_node,1);	//Send it on, indicate it is a routed payload
			}
		#else
		//if(radio.available()){printf("------FLUSHED DATA --------------");}	
		write(header.to_node,1);	//Send it on, indicate it is a routed payload
		#endif
	  }
	  
  }
  return returnVal;
}

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
			printf("Frag frame with %d frags exceeds MAX_PAYLOAD_SIZE\n",frame.header.reserved);
#endif
			frag_queue.header.reserved = 0;
			return 0;
		}
#if defined (SERIAL_DEBUG_FRAGMENTATION)
		printf("queue first, total frags %d\n",frame.header.reserved);
#endif
		memcpy(&frag_queue,&frame,11);
		memcpy(frag_queue.message_buffer,frame_buffer+sizeof(RF24NetworkHeader),frame.message_size);
#if defined (SERIAL_DEBUG_FRAGMENTATION_L2)		
		for(int i=0; i<frag_queue.message_size;i++){
			Serial.println(frag_queue.message_buffer[i],HEX);
		}
#endif
		frag_queue.total_fragments = frag_queue.header.reserved;
		//Store the total size of the stored frame in message_size
	    frag_queue.message_size = frame.message_size;
		frag_queue.header.reserved = frag_queue.total_fragments - 1;

	}else
	if(frame.header.type == NETWORK_MORE_FRAGMENTS || frame.header.type == NETWORK_MORE_FRAGMENTS_NACK){
	    if(frame.header.reserved != frag_queue.header.reserved){
#if defined (SERIAL_DEBUG_FRAGMENTATION)
		printf("1 Dropped out of order frame %d expected %d\n",frame.header.reserved,frag_queue.header.reserved);
#endif
			frag_queue.header.reserved = 0;
			return 0;
		}
		memcpy(frag_queue.message_buffer+frag_queue.message_size,frame_buffer+sizeof(RF24NetworkHeader),frame.message_size);
		frag_queue.message_size += frame.message_size;
		frag_queue.header.reserved--;

	}else
	if(frame.header.type == NETWORK_LAST_FRAGMENT){
		
		if(frag_queue.header.reserved != 1 || frag_queue.header.id != frame.header.id){
			#if defined (SERIAL_DEBUG_FRAGMENTATION)
			printf("2 Dropped out of order frame frag %d header id %d expected last (1) \n",frame.header.reserved,frame.header.id);
			#endif
			frag_queue.header.reserved = 0;
			return 0;
		}
		frag_queue.header.reserved = 0;

		memcpy(frag_queue.message_buffer+frag_queue.message_size,frame_buffer+sizeof(RF24NetworkHeader),frame.message_size);
				
		frag_queue.message_size += frame.message_size;		
#if defined (SERIAL_DEBUG_FRAGMENTATION)
		printf("fq 3: %d\n",frag_queue.message_size);
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
			//memcpy(next_frame,&frag_queue,11+frag_queue.message_size);
			
				if( frag_queue.message_size <  sizeof(frame_queue)-(next_frame-frame_queue)){
				memcpy(next_frame,&frag_queue,11);
//				memcpy(next_frame+11,&frag_queue.message_buffer,frag_queue.message_size);
				memcpy(next_frame+11,frag_queue.message_buffer,frag_queue.message_size);
				next_frame += (11+frag_queue.message_size);
				}
			}
		#if defined (SERIAL_DEBUG_FRAGMENTATION)
			printf("enq size %d\n",frag_queue.message_size);
		#endif
		
		}
  
	}//If last Fragment
  
  }else //else is not a fragment
 #endif 
  // Copy the current frame into the frame queue
  if (  next_frame < frame_queue + sizeof(frame_queue) && frame.message_size <= MAX_FRAME_SIZE)
  {
    //memcpy(next_frame,frame_buffer, frame_size );
#if !defined( DISABLE_FRAGMENTATION )
	frag_ptr = (RF24NetworkFrame*)&next_frame;
#endif	
	if(frame.header.type == EXTERNAL_DATA_TYPE){		
		return 2;
	}
	
//	memcpy(next_frame,&frame,11);
//	memcpy(next_frame+11,frame_buffer+sizeof(RF24NetworkHeader),frame.message_size);
	memcpy(next_frame,&frame,11);
	memcpy(next_frame+11,frame_buffer+sizeof(RF24NetworkHeader),frame.message_size);
#if defined (SERIAL_DEBUG_FRAGMENTATION)
	for(int i=0; i<frame.message_size+11;i++){
		Serial.print(frame_queue[i],HEX);
		Serial.print(" : ");
	}
	Serial.println("");
#endif
    next_frame += (frame.message_size + 11); 
#if defined (SERIAL_DEBUG_FRAGMENTATION)
	printf("enq %d\n",next_frame-frame_queue);
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
uint8_t RF24Network::peekData(){
		
		return frame_queue[0];
}
size_t RF24Network::peek(RF24NetworkHeader& header)
{
  if ( available() )
  {
    // Copy the next available frame from the queue into the provided buffer
    //memcpy(&header,next_frame-frame_size,sizeof(RF24NetworkHeader));
	memcpy(&header,frame_queue,sizeof(RF24NetworkHeader));
	//next_frame=frame_queue;
	RF24NetworkFrame& frame = * reinterpret_cast<RF24NetworkFrame*>(frame_queue);
	return frame.message_size;
  }
  return 0;
}

/******************************************************************/

size_t RF24Network::read(RF24NetworkHeader& header,void* message, size_t maxlen)
{
  size_t bufsize = 0;

  if ( available() )
  {
    
	memcpy(&header,frame_queue,8);

    if (maxlen > 0)
    {	
		bufsize = frame_queue[8];
		bufsize |= frame_queue[9] << 8;
		maxlen = min(maxlen,bufsize);
		memcpy(message,frame_queue+11,maxlen);
	    IF_SERIAL_DEBUG(printf("%lu: NET message size %d\n",millis(),bufsize););

	size_t len = bufsize;
	IF_SERIAL_DEBUG(printf_P(PSTR("%lu: NET r message "),millis());const uint8_t* charPtr = reinterpret_cast<const uint8_t*>(message);while(len--){ printf("%02x ",charPtr[len]);} printf_P(PSTR("\n\r") ) );      
	  
    }
	bufsize+=11;
	memmove(frame_queue,frame_queue+bufsize,sizeof(frame_queue)- bufsize);
	while(bufsize--){
		next_frame--;
	}

	//IF_SERIAL_DEBUG(printf_P(PSTR("%lu: NET Received %s\n\r"),millis(),header.toString()));
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
    
	return write(levelToAddress(level),USER_TX_MULTICAST);
  
}
#endif

/******************************************************************/
bool RF24Network::write(RF24NetworkHeader& header,const void* message, size_t len){
	return write(header,message,len,070);
}
/******************************************************************/
bool RF24Network::write(RF24NetworkHeader& header,const void* message, size_t len, uint16_t writeDirect){
    frame_size=sizeof(RF24NetworkHeader)+len;
	frame_size=min(frame_size,MAX_FRAME_SIZE);
#if defined (DISABLE_FRAGMENTATION)
	return _write(header,message,len,writeDirect);
#else  
  //bool txSuccess = true;

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
    RF24NetworkHeader *fragmentHeader = &header;
    fragmentHeader->reserved = fragment_id;

    if (fragment_id == 1) {
      fragmentHeader->type = NETWORK_LAST_FRAGMENT;  //Set the last fragment flag to indicate the last fragment
      fragmentHeader->reserved = header.type; //Workaroung/Hack: to transmit the user application defined header.type, save this variable in the header.fragment_id.
    } else {
      if (msgCount == 0) {
        fragmentHeader->type = NETWORK_FIRST_FRAGMENT;
      }else{
        fragmentHeader->type = NETWORK_MORE_FRAGMENTS; //Set the more fragments flag to indicate a fragmented frame
      }
    }
	
    size_t offset = msgCount*max_frame_payload_size;
    size_t fragmentLen = min(len-offset,max_frame_payload_size);

   // IF_SERIAL_DEBUG_FRAGMENTATION(printf("%lu: FRG try to transmit fragmented payload of size %d Bytes with fragmentID '%d'\n\r",millis(),fragmentLen,fragment_id););

    //Try to send the payload chunk with the copied header
	//printf("frz %d\n",frame_size);
    frame_size = sizeof(RF24NetworkHeader)+fragmentLen;
	bool ok = _write(*fragmentHeader,((char *)message)+offset,fragmentLen,writeDirect);
	
	//fragmentHeader->id++;
	
    uint8_t counter = 0;
	/*while(!ok){
	  fragmentHeader.id++;
	  ok = _write(fragmentHeader,((char *)message)+offset,fragmentLen,writeDirect);
	  counter++;
	  if(counter >= 3){ break; }	  
	}*/
    //if (!noListen) {      
     // delayMicroseconds(50);
    //}

    if (!ok) {
        IF_SERIAL_DEBUG_FRAGMENTATION(printf("%lu: FRG message transmission with fragmentID '%d' failed. Abort.\n\r",millis(),fragment_id););
        return false;
        break;
    }

    //Message was successful sent
    IF_SERIAL_DEBUG_FRAGMENTATION(printf("%lu: FRG message transmission with fragmentID '%d' sucessfull.\n\r",millis(),fragment_id););

    //Check and modify counters
    fragment_id--;
    msgCount++;
  }

  //noListen = 0;

  // Now, continue listening
  //radio.startListening();

  int frag_delay = uint8_t(len/48);
  delay( min(frag_delay,20));

  //Return true if all the chunks where sent successfully
  //else return false
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
  
  IF_SERIAL_DEBUG(printf_P(PSTR("%lu: NET Sending %s\n\r"),millis(),header.toString()));
  if (len){
    
    memcpy(frame_buffer + sizeof(RF24NetworkHeader),message,min(frame_size-sizeof(RF24NetworkHeader),len));
	
	IF_SERIAL_DEBUG(size_t tmpLen = len;printf_P(PSTR("%lu: NET message "),millis());const uint8_t* charPtr = reinterpret_cast<const uint8_t*>(message);while(tmpLen--){ printf("%02x ",charPtr[tmpLen]);} printf_P(PSTR("\n\r") ) );

  }

  // If the user is trying to send it to himself
  if ( header.to_node == node_address ){

    RF24NetworkFrame frame = RF24NetworkFrame(header,len,0);
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
  //const uint16_t fromAddress = frame_buffer[0] | (frame_buffer[1] << 8);
  
  // Throw it away if it's not a valid address
  if ( !is_valid_address(to_node) )
    return false;  
  
  //Load info into our conversion structure, and get the converted address info
  logicalToPhysicalStruct conversion = { to_node,directTo,0};
  logicalToPhysicalAddress(&conversion);

  IF_SERIAL_DEBUG(printf_P(PSTR("%lu: MAC Sending to 0%o via 0%o on pipe %x\n\r"),millis(),to_node,conversion.send_node,conversion.send_pipe));
  
  /**Write it*/
  ok=write_to_pipe(conversion.send_node, conversion.send_pipe, conversion.multicast);  	
  
  
  if(!ok){	IF_SERIAL_DEBUG_ROUTING( printf_P(PSTR("%lu: MAC Send fail to 0%o via 0%o on pipe %x\n\r"),millis(),to_node,conversion.send_node,conversion.send_pipe);); }
 
	if( directTo == TX_ROUTED && ok && conversion.send_node == to_node && frame_buffer[6] > 64 && frame_buffer[6] < 192){
			
			RF24NetworkHeader& header = * reinterpret_cast<RF24NetworkHeader*>(frame_buffer);
			header.type = NETWORK_ACK;				    // Set the payload type to NETWORK_ACK			
			header.to_node = header.from_node;          // Change the 'to' address to the 'from' address			
			//dynLen=8;
			//conversion={header.from_node,TX_ROUTED,0};
			conversion.send_node = header.from_node;
			conversion.send_pipe = TX_ROUTED;
			conversion.multicast = 0;
			logicalToPhysicalAddress(&conversion);
			
			//Write the data using the resulting physical address
			write_to_pipe(conversion.send_node, conversion.send_pipe, conversion.multicast);
			
			//dynLen=0;
			IF_SERIAL_DEBUG_ROUTING( printf_P(PSTR("%lu MAC: Route OK to 0%o ACK sent to 0%o\n"),millis(),to_node,header.to_node); );
	}
  //dynLen=0;  	
 // if(!ok){ printf_P(PSTR("%lu: MAC No Ack from 0%o via 0%o on pipe %x\n\r"),millis(),to_node,send_node,send_pipe); }
 
#if !defined (DUAL_HEAD_RADIO)
  // Now, continue listening
  radio.startListening();
#endif

	if( ok && conversion.send_node != to_node && (directTo==0 || directTo==3) && frame_buffer[6] > 64 && frame_buffer[6] < 192){
		uint32_t reply_time = millis(); 
		while( update() != NETWORK_ACK){
			if(millis() - reply_time > routeTimeout){  
				IF_SERIAL_DEBUG_ROUTING( printf_P(PSTR("%lu: MAC Network ACK fail from 0%o via 0%o on pipe %x\n\r"),millis(),to_node,conversion.send_node,conversion.send_pipe); );				
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
  radio.writeFast(&frame_buffer, frame_size,multicast);
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
  sprintf_P(buffer,PSTR("id %d from 0%o to 0%o type %d"),id,from_node,to_node,type);
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