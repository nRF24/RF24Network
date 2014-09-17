/*
 Copyright (C) 2011 James Coliz, Jr. <maniacbug@ymail.com>
 Copyright (C) 2014 Rei <devel@reixd.net>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

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
#include "RF24Network_config.h"
#include <RF24/RF24.h>
#include "RF24Network.h"

uint16_t RF24NetworkHeader::next_id = 1;

uint64_t pipe_address( uint16_t node, uint8_t pipe );
#if defined (RF24NetworkMulticast)
  uint16_t levelToAddress( uint8_t level );
#endif
bool is_valid_address( uint16_t node );
uint32_t nFails = 0, nOK=0;

/******************************************************************/

RF24Network::RF24Network( RF24& _radio ): radio(_radio), frame_size(MAX_FRAME_SIZE), lastMultiMessageID(0), noListen(1), lastWriteTime(0)
{}

/******************************************************************/

void RF24Network::begin(uint8_t _channel, uint16_t _node_address ) {
  if (! is_valid_address(_node_address) ) {
    return;
  }

  node_address = _node_address;

  if ( ! radio.isValid() ) {
    return;
  }

  // Set up the radio the way we want it to look
  radio.setChannel(_channel);
  radio.setDataRate(RF24_1MBPS);
  radio.setCRCLength(RF24_CRC_16);
  radio.enableDynamicAck();
  radio.enableDynamicPayloads();

  uint8_t retryVar = (((node_address % 6)+1) *2) + 3;
  radio.setRetries(retryVar, 5);
  txTimeout = 30;
  routeTimeout = txTimeout*9;

  // Setup our address helper cache
  setup_address();

  // Open up all listening pipes
  int i = 6;
  while (i--) {
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
void RF24Network::failures(uint32_t *_fails, uint32_t *_ok) {
  *_fails = nFails;
  *_ok = nOK;
}

uint8_t RF24Network::update(void) {
  // if there is data ready
  uint8_t returnVal = 0;

#if defined (SERIAL_DEBUG)
  while ( radio.isValid() && radio.available(&pipe_num) )
#else
  while ( radio.isValid() && radio.available() )
#endif
  {
    // Fetch the payload, and get the size
    size_t len = radio.getDynamicPayloadSize();
    if (len == 0) {
      delay(2);
      printf("Bad Dyn Payload\n");
      continue;
    }
    radio.read( frame_buffer, len );

    //Do we have a valid length for a frame?
    //We need at least a frame with header a no payload (payload_size equals 0).
    if (len < sizeof(RF24NetworkHeader)) {
      continue;
    }

    // Read the beginning of the frame as the header
    RF24NetworkHeader header;
    memcpy(&header,frame_buffer,sizeof(RF24NetworkHeader));

    IF_SERIAL_DEBUG(printf_P("%u: MAC Received on %u %s\n\r",millis(),pipe_num,header.toString()));
    if (len) {
      IF_SERIAL_DEBUG_FRAGMENTATION(printf("%u: FRG Rcv frame size %i\n",millis(),len););
      IF_SERIAL_DEBUG_FRAGMENTATION(printf("%u: FRG Rcv frame ",millis()); const char* charPtr = reinterpret_cast<const char*>(frame_buffer); for (size_t i = 0; i < len; i++) { printf("%02X ", charPtr[i]); }; printf("\n\r"));
    }

    // Throw it away if it's not a valid address
    if ( !is_valid_address(header.to_node) ) {
      continue;
    }

    IF_SERIAL_DEBUG(printf_P("%u: MAC Valid payload from %i with size %i received.\n\r",millis(),header.from_node,len));

    // Build the full frame
    RF24NetworkFrame frame = RF24NetworkFrame(header,frame_buffer+sizeof(RF24NetworkHeader),len-sizeof(RF24NetworkHeader));

    // Is this for us?
    if ( header.to_node == node_address ) {
      if (header.type == NETWORK_ACK) {
        IF_SERIAL_DEBUG_ROUTING(printf_P(PSTR("RT: Network ACK Rcvd\n")););
        returnVal = NETWORK_ACK;
        continue;
      }

      enqueue(frame);

      if (radio.rxFifoFull()) {
        printf(" fifo full\n");
      }

    } else { //Frame if not for us

      #if defined (RF24NetworkMulticast)
        if ( header.to_node == 0100) {

          if (header.id != lastMultiMessageID) {
            if (multicastRelay) {
              IF_SERIAL_DEBUG_ROUTING(printf_P(PSTR("RT: FWD multicast frame from 0%o to level %d\n"),header.from_node,multicast_level+1););
              write(levelToAddress(multicast_level)<<3,4);
            }

            lastMultiMessageID = header.id;
            enqueue(frame);

          } else {
            //Duplicate received
            IF_SERIAL_DEBUG_ROUTING(printf_P(PSTR("RT: Drop duplicate multicast frame %u from 0%o\n"),header.id,header.from_node););
          }

        } else {
          write(header.to_node,1);     //Send it on, indicate it is a routed payload
        }
      #else //Not Multicast
        write(header.to_node,1);    //Send it on, indicate it is a routed payload
      #endif
    }

  } // END while radio.isValid() && radio.available()

  return returnVal;
}

/******************************************************************/

bool RF24Network::enqueue(RF24NetworkFrame frame) {
  bool result = false;

  // This is sent to itself
  if (frame.header.from_node == node_address) {
    bool isFragment = ( frame.header.type == NETWORK_FIRST_FRAGMENT || frame.header.type == NETWORK_MORE_FRAGMENTS || frame.header.type == NETWORK_LAST_FRAGMENT );
    if (isFragment) {
      printf("Cannot enqueue multi-payload frames to self\n");
      return false;
    }
    frame_queue.push(frame);
    return true;
  }

  if ((frame.header.fragment_id > 1 && (frame.header.type == NETWORK_MORE_FRAGMENTS || frame.header.type== NETWORK_FIRST_FRAGMENT)) ) {
    //The received frame contains the a fragmented payload

    //Set the more fragments flag to indicate a fragmented frame
    IF_SERIAL_DEBUG_FRAGMENTATION(printf("%u: FRG fragmented payload of size %i Bytes with fragmentID '%i' received.\n\r",millis(),frame.message_size,frame.header.fragment_id););

    //Append payload
    appendFragmentToFrame(frame);
    result = true;

  } else if ( frame.header.type == NETWORK_LAST_FRAGMENT) {
    //The last fragment flag indicates that we received the last fragment
    //Workaround/Hack: In this case the header.fragment_id does not count the number of fragments but is a copy of the type flags specified by the user application.

    IF_SERIAL_DEBUG_FRAGMENTATION(printf("%u: FRG Last fragment with size %i Bytes and fragmentID '%i' received.\n\r",millis(),frame.message_size,frame.header.fragment_id););
    //Append payload
    appendFragmentToFrame(frame);

    IF_SERIAL_DEBUG(printf_P(PSTR("%u: NET Enqueue assembled frame @%x "),millis(),frame_queue.size()));

    //Push the assembled frame in the frame_queue and remove it from cache
    // If total_fragments == 0, we are finished
    if ( frame.header.to_node == node_address || (!frame.header.fragment_id && frameFragmentsCache.count(std::make_pair(frame.header.from_node,frame.header.id)) ) ) {
      frame_queue.push( frameFragmentsCache[ std::make_pair(frame.header.from_node,frame.header.id) ] );
    } else {
      IF_SERIAL_DEBUG_MINIMAL( printf("%u: NET Dropped frame missing %d fragments\n",millis(),frame.total_fragments ););
    }

    frameFragmentsCache.erase( std::make_pair(frame.header.from_node,frame.header.id) );

    result = true;

  } else if (frame.header.type <= MAX_USER_DEFINED_HEADER_TYPE) {
    //This is not a fragmented payload but a whole frame.

    IF_SERIAL_DEBUG(printf_P(PSTR("%u: NET Enqueue @%x "),millis(),frame_queue.size()));
    // Copy the current frame into the frame queue
    frame_queue.push(frame);
    result = true;

  } else {
    //Undefined/Unknown header.type received. Drop frame!
    IF_SERIAL_DEBUG_MINIMAL( printf("%u: FRG Received unknown or system header type %d with fragment id %d\n",millis(),frame.header.type, frame.header.fragment_id); );
    //The frame is not explicitly dropped, but the given object is ignored.
    //FIXME: does this causes problems with memory management?
  }

  if (result) {
    IF_SERIAL_DEBUG(printf("ok\n\r"));
  } else {
    IF_SERIAL_DEBUG(printf("failed\n\r"));
  }

  return result;
}

/******************************************************************/

void RF24Network::appendFragmentToFrame(RF24NetworkFrame frame) {

  bool isFragment = ( frame.header.type == NETWORK_FIRST_FRAGMENT || frame.header.type == NETWORK_MORE_FRAGMENTS || frame.header.type == NETWORK_LAST_FRAGMENT );

  if (!isFragment) {
    //The received payload is not a fragment.
    frameFragmentsCache[ std::make_pair(frame.header.from_node,frame.header.id) ] = frame;
  } else

  if (frameFragmentsCache.count(std::make_pair(frame.header.from_node,frame.header.id)) == 0 ) {
    // This is the first of many fragments.

    if (frame.header.type == NETWORK_FIRST_FRAGMENT || ( frame.header.type != NETWORK_MORE_FRAGMENTS && frame.header.type != NETWORK_LAST_FRAGMENT)) {
      // Decrement the stored total_fragments counter
      frame.header.fragment_id = frame.header.fragment_id-1;
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
      //Workaround/Hack: The user application specified header.type is sent with the last fragment in the fragment_id field
      frame.header.type = frame.header.fragment_id;
      frame.header.fragment_id = 1;
    }

    //Error checking for missed fragments and payload size
    if (frame.header.fragment_id != f->header.fragment_id) {
        if (frame.header.fragment_id > f->header.fragment_id) {
            IF_SERIAL_DEBUG_MINIMAL(printf("%u: FRG Duplicate or out of sequence frame %d, expected %d. Cleared.\n",millis(),frame.header.fragment_id,f->header.fragment_id););
            frameFragmentsCache.erase( std::make_pair(frame.header.from_node,frame.header.id) );
        } else {
            frameFragmentsCache.erase( std::make_pair(frame.header.from_node,frame.header.id) );
            IF_SERIAL_DEBUG_MINIMAL(printf("%u: FRG Out of sequence frame %d, expected %d. Cleared.\n",millis(),frame.header.fragment_id,f->header.fragment_id););
        }
        return;
    }

    //Check if the total payload exceeds the MAX_PAYLOAD_SIZE.
    //If so, drop the assembled frame.
    if (frame.message_size + f->message_size > MAX_PAYLOAD_SIZE) {
        frameFragmentsCache.erase( std::make_pair(frame.header.from_node,frame.header.id) );
        IF_SERIAL_DEBUG_MINIMAL(printf("%u: FRM Payload of %l exceeds MAX_PAYLOAD_SIZE %l Bytes. Frame dropped\n",millis(),frame.message_size + f->message_size,MAX_PAYLOAD_SIZE););
        return;
    }

    //Append the received fragment to the cached frame
    memcpy(f->message_buffer+f->message_size, frame.message_buffer, frame.message_size);
    f->message_size += frame.message_size;  //Increment message size
    f->header = frame.header; //Update header
    f->header.fragment_id--;  //Update Total fragments

  }
}

/******************************************************************/

bool RF24Network::available(void) {
  // Are there frames on the queue for us?
  return (!frame_queue.empty());
}

/******************************************************************/

uint16_t RF24Network::parent() const {
  //Return the parent node of this node. If we are the parent (master) node, return -1.
  if ( node_address == 0 ) {
    return -1;
  } else {
    return parent_node;
  }
}

/******************************************************************/

void RF24Network::peek(RF24NetworkHeader& header) {
  //Get an insight in the next frame in the queue.
  if ( available() ) {
    RF24NetworkFrame frame = frame_queue.front();
    memcpy(&header,&frame,sizeof(RF24NetworkHeader));
  }
}

/******************************************************************/

size_t RF24Network::read(RF24NetworkHeader& header,void* message, size_t maxlen) {
  //Get the next queued frame.

  size_t bufsize = 0;

  if ( available() ) {
    RF24NetworkFrame frame = frame_queue.front();

    // How much buffer size should we actually copy?
    bufsize = std::min(frame.message_size,maxlen);

    memcpy(&header,&(frame.header),sizeof(RF24NetworkHeader));
    memcpy(message,frame.message_buffer,bufsize);

    IF_SERIAL_DEBUG_FRAGMENTATION(printf("%u: FRG message size %i\n",millis(),frame.message_size););
    IF_SERIAL_DEBUG_FRAGMENTATION(printf("%u: FRG message ",millis()); const char* charPtr = reinterpret_cast<const char*>(message); for (size_t i = 0; i < bufsize; i++) { printf("%02X ", charPtr[i]); }; printf("\n\r"));
    IF_SERIAL_DEBUG(printf_P(PSTR("%u: NET read %s\n\r"),millis(),header.toString()));

    frame_queue.pop();
  }

  return bufsize;
}

/******************************************************************/

#if defined (RF24NetworkMulticast)

bool RF24Network::multicast(RF24NetworkHeader& header,const void* message, size_t len, uint8_t level) {

  // Fill out the header
  header.to_node = 0100;
  header.from_node = node_address;

  // Build the full frame to send
  RF24NetworkFrame frame = RF24NetworkFrame(header,message,std::min(sizeof(message),len));

  IF_SERIAL_DEBUG(printf_P(PSTR("%u: NET Sending %s\n\r"),millis(),header.toString()));
  if (len) {
    IF_SERIAL_DEBUG(const uint16_t* i = reinterpret_cast<const uint16_t*>(message);printf_P(PSTR("%u: NET message %04x\n\r"),millis(),*i));
  }

  uint16_t levelAddr = 1;
  levelAddr = levelAddr << ((level-1) * 3);

  return write(levelAddr,4);

}
#endif  //RF24NetworkMulticast

/******************************************************************/

bool RF24Network::write(RF24NetworkHeader& header,const void* message, size_t len) {
    return write(header,message,len,070);
}

/******************************************************************/

bool RF24Network::write(RF24NetworkHeader& header,const void* message, size_t len, uint16_t writeDirect) {

  bool txSuccess = true;

  //Check payload size
  if (len > MAX_PAYLOAD_SIZE) {
    IF_SERIAL_DEBUG(printf("%u: NET write message failed. Given 'len' is bigger than the MAX Payload size %i\n\r",millis(),MAX_PAYLOAD_SIZE););
    return false;
  }

  //If the message payload is too big, whe cannot generate enough fragments
  //and enumerate them
  if (len > 255*max_frame_payload_size) {
    txSuccess = false;
    return txSuccess;
  }

  //The payload is smaller than MAX_PAYLOAD_SIZE and we are able to enumerate the fragments (due to variable sizes).
  // --> We cann transmit the message.

  // First, stop listening so we can talk.
  radio.stopListening();

  //Normal Write (Un-Fragmented)
  //The len of the payload if less or equal than the max_frame_payload_size,
  //therefore we send this payload in a single frame. Fragmentation not needed.
  if (len <= max_frame_payload_size) {
    txSuccess = _write(header,message,len,writeDirect);
    radio.startListening();
    return txSuccess;
  }

  noListen = 1;

  //Fragmented Write
  //The len payload is definitely bigger than the max_frame_payload_size,
  //therefore a fragmentation IS neded.

  //Divide the message payload into chuncks of max_frame_payload_size
  uint8_t fragment_id = 1 + ((len - 1) / max_frame_payload_size);  //the number of fragments to send = ceil(len/max_frame_payload_size)
  uint8_t msgCount = 0;

  IF_SERIAL_DEBUG_FRAGMENTATION(printf("%u: FRG Total message fragments %i\n\r",millis(),fragment_id););

  //Iterate over the payload chuncks
  //  Assemble a new message, copy and fill out the header
  //  Try to send this message
  //  If it fails
  //    then break
  //    return result as false
  while (fragment_id > 0) {

    //Copy and fill out the header
    RF24NetworkHeader fragmentHeader = header;
    fragmentHeader.fragment_id = fragment_id;

    if (fragment_id == 1) {
      fragmentHeader.type = NETWORK_LAST_FRAGMENT;  //Set the last fragment flag to indicate the last fragment
      fragmentHeader.fragment_id = header.type; //Workaroung/Hack: to transmit the user application defined header.type, save this variable in the header.fragment_id.
    } else {
      if (msgCount == 0) {
        fragmentHeader.type = NETWORK_FIRST_FRAGMENT;
      }else{
        fragmentHeader.type = NETWORK_MORE_FRAGMENTS; //Set the more fragments flag to indicate a fragmented frame
      }
    }

    size_t offset = msgCount*max_frame_payload_size;
    size_t fragmentLen = std::min(len-offset,max_frame_payload_size);

    IF_SERIAL_DEBUG_FRAGMENTATION(printf("%u: FRG try to transmit fragmented payload of size %i Bytes with fragmentID '%i'\n\r",millis(),fragmentLen,fragment_id););

    //Try to send the payload chunk with the copied header
    bool ok = _write(fragmentHeader,message+offset,fragmentLen,writeDirect);

    if (!noListen) {
      //Hack to allow the radio to be able to process the tx bytes
      delayMicroseconds(50);
    }

    if (!ok) {
        IF_SERIAL_DEBUG_FRAGMENTATION(printf("%u: FRG message transmission with fragmentID '%i' failed. Abort.\n\r",millis(),fragment_id););
        txSuccess = false;
        break;
    }

    //Message was successful sent
    IF_SERIAL_DEBUG_FRAGMENTATION(printf("%u: FRG message transmission with fragmentID '%i' sucessfull.\n\r",millis(),fragment_id););

    //Check and modify counters
    fragment_id--;
    msgCount++;
  }

  noListen = 0;

  // Now, continue listening
  radio.startListening();

  int frag_delay = uint8_t(len/48);
  delay( std::min(frag_delay,20));

  //Return true if all the chuncks where sent successfuly
  //else return false
  IF_SERIAL_DEBUG(printf("%u: NET total message fragments sent %i. txSuccess ",millis(),msgCount); printf("%s\n\r", txSuccess ? "YES" : "NO"););
  return txSuccess;
}

/******************************************************************/

bool RF24Network::_write(RF24NetworkHeader& header,const void* message, size_t len, uint16_t writeDirect) {

  // Fill out the header
  header.from_node = node_address;

  // Build the full frame to send
  memcpy(frame_buffer,&header,sizeof(RF24NetworkHeader));
  frame_size = sizeof(RF24NetworkHeader); //Set the current frame size

  if (len) {
    memcpy(frame_buffer + sizeof(RF24NetworkHeader),message,std::min(MAX_FRAME_SIZE-sizeof(RF24NetworkHeader),len));
    frame_size += len; //Set the current frame size
  }

  IF_SERIAL_DEBUG(printf_P(PSTR("%u: NET Sending %s\n\r"),millis(),header.toString()));

  if (frame_size) {
    IF_SERIAL_DEBUG_FRAGMENTATION(printf("%u: FRG frame size %i\n",millis(),frame_size););
    IF_SERIAL_DEBUG_FRAGMENTATION(printf("%u: FRG frame ",millis()); const char* charPtr = reinterpret_cast<const char*>(frame_buffer); for (size_t i = 0; i < frame_size; i++) { printf("%02X ", charPtr[i]); }; printf("\n\r"));
  }

  // If the user is trying to send it to himself
  if ( header.to_node == node_address ) {
    // Build the frame to send
    RF24NetworkFrame frame = RF24NetworkFrame(header,message,std::min(MAX_FRAME_SIZE-sizeof(RF24NetworkHeader),len));
    // Just queue it in the received queue
    return enqueue(frame);

  } else {

    //Transmit to the specified destination
    if (writeDirect != 070) {
        if (header.to_node == writeDirect) {
            return write(writeDirect,2);
        } else {
            return write(writeDirect,3);
        }
    } else {
        // Otherwise send it out over the air
        return write(header.to_node,0);
    }
  }

}

/******************************************************************/

bool RF24Network::write(uint16_t to_node, uint8_t directTo) {

  bool multicast = 0; // Radio ACK requested = 0
  const uint16_t fromAddress = frame_buffer[0] | (frame_buffer[1] << 8);
  const uint16_t logicalAddress = frame_buffer[2] | (frame_buffer[3] << 8);

  // Throw it away if it's not a valid address
  if ( !is_valid_address(to_node) ) {
    return false;
  }

  if (to_node != parent_node && !is_direct_child(to_node) ) {
     noListen=0;
  }

  // Where do we send this?  By default, to our parent
  uint16_t send_node = parent_node;
  // On which pipe
  uint8_t send_pipe = parent_pipe%5;

  //Select the send_pipe based on directTo or
  //if this node is a direct child or a descendant
 if (directTo > 1) {
    send_node = to_node;
    multicast = 1;
    if (directTo == 4) {
        send_pipe=0;
    }
  } else if ( is_direct_child(to_node) ) { // If the node is a direct child,

    // Send directly
    send_node = to_node;

    // To its listening pipe
    send_pipe = 5;

  } else if ( is_descendant(to_node) ) {
    // If the node is a child of a child
    // talk on our child's listening pipe,
    // and let the direct child relay it.

    send_node = direct_child_route_to(to_node);
    send_pipe = 5;
  }


  IF_SERIAL_DEBUG(printf_P(PSTR("%u: MAC Sending to 0%o via 0%o on pipe %x\n\r"),millis(),logicalAddress,send_node,send_pipe));

  // Put the frame on the pipe
  bool ok = write_to_pipe( send_node, send_pipe, multicast );

  #if defined (SERIAL_DEBUG_ROUTING) || defined(SERIAL_DEBUG)
    if (!ok) {
      printf_P(PSTR("%u: MAC Send fail to 0%o from 0%o via 0%o on pipe %x\n\r"),millis(),logicalAddress,fromAddress,send_node,send_pipe);
    }
  #endif


  if ( directTo == 1 && ok && send_node == to_node && frame_buffer[6] != NETWORK_ACK && fromAddress != node_address ) {

    frame_buffer[6] = NETWORK_ACK;
    frame_buffer[2] = frame_buffer[0]; frame_buffer[3] = frame_buffer[1];
    write(fromAddress,1);

    IF_SERIAL_DEBUG_ROUTING(printf("RT Route OK to 0%o ACK sent to 0%o\n",to_node,fromAddress););
  }

  if (!noListen ) {
    radio.startListening();
  }

  if ( ok && (send_node != logicalAddress) && (directTo==0 || directTo == 3 )) {

    if (!noListen) {
      radio.startListening();
    }

    uint32_t reply_time = millis();

    //Wait for NETWORK_ACK
    while( update() != NETWORK_ACK) {
      if (millis() - reply_time > routeTimeout) {
        ok = 0;

        IF_SERIAL_DEBUG_ROUTING(printf_P(PSTR("%u: RT Network ACK fail from 0%o via 0%o on pipe %x\n\r"),millis(),logicalAddress,send_node,send_pipe););
        break;
      }
    }

    if (!noListen) {
      radio.stopListening();
    }

  }

  //Increment the number of correctly trasmitted frames or the number of fail transmissions
  if (ok == true) {
    nOK++;
  } else {
    nFails++;
  }

  return ok;
}

/******************************************************************/



bool RF24Network::write_to_pipe( uint16_t node, uint8_t pipe, bool multicast ) {

  bool ok = false;

  uint64_t out_pipe = pipe_address( node, pipe );

  if (!noListen) {
    radio.stopListening();
  }

  // Open the correct pipe for writing.
  radio.openWritingPipe(out_pipe);

  // Retry a few times
  radio.writeFast(frame_buffer, frame_size, multicast);

  ok = radio.txStandBy(txTimeout);

  IF_SERIAL_DEBUG(printf_P(PSTR("%u: MAC Sent on %x %s\n\r"),millis(),(uint32_t)out_pipe,ok?PSTR("ok"):PSTR("failed")));

  return ok;
}

/******************************************************************/

const char* RF24NetworkHeader::toString(void) const {
  //Convert the network header to a C-String.
  static char buffer[45];
  return buffer;
}

/******************************************************************/

bool RF24Network::is_direct_child( uint16_t node ) {

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

bool RF24Network::is_descendant( uint16_t node ) {
  return ( node & node_mask ) == node_address;
}

/******************************************************************/

void RF24Network::setup_address(void) {

  // First, establish the node_mask
  uint16_t node_mask_check = 0xFFFF;
  while ( node_address & node_mask_check ) {
    node_mask_check <<= 3;
  }

  node_mask = ~ node_mask_check;

  // parent mask is the next level down
  uint16_t parent_mask = node_mask >> 3;

  // parent node is the part IN the mask
  parent_node = node_address & parent_mask;

  // parent pipe is the part OUT of the mask
  uint16_t i = node_address;
  uint16_t m = parent_mask;
  while (m) {
    i >>= 3;
    m >>= 3;
  }

  parent_pipe = i;

  IF_SERIAL_DEBUG(printf_P(PSTR("setup_address node=0%o mask=0%o parent=0%o pipe=0%o\n\r"),node_address,node_mask,parent_node,parent_pipe););
}

/******************************************************************/

uint16_t RF24Network::direct_child_route_to( uint16_t node ) {
  // Presumes that this is in fact a child!!
  uint16_t child_mask = ( node_mask << 3 ) | 0B111;
  return node & child_mask ;
}

/******************************************************************/

uint8_t RF24Network::pipe_to_descendant( uint16_t node ) {
  uint16_t i = node;
  uint16_t m = node_mask;

  while (m) {
    i >>= 3;
    m >>= 3;
  }

  return i & 0B111;
}

/******************************************************************/

bool is_valid_address( uint16_t node ) {
  bool result = true;

  while(node) {
    uint8_t digit = node & 0B111;

  #if !defined (RF24NetworkMulticast)
    if (digit < 1 || digit > 5)
  #else
    if (digit < 0 || digit > 5) //Allow our out of range multicast address
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
  void RF24Network::multicastLevel(uint8_t level) {
    multicast_level = level;
    radio.stopListening();
    radio.openReadingPipe(0,pipe_address(levelToAddress(level),0));
    radio.startListening();
  }

  uint16_t levelToAddress(uint8_t level) {
    uint16_t levelAddr = 1;
    levelAddr = levelAddr << ((level-1) * 3);
    return levelAddr;
  }
#endif

/******************************************************************/

uint64_t pipe_address( uint16_t node, uint8_t pipe ) {

  static uint8_t address_translation[] = { 0xc3,0x3c,0x33,0xce,0x3e,0xe3,0xec };
  uint64_t result = 0xCCCCCCCCCCLL;
  uint8_t* out = reinterpret_cast<uint8_t*>(&result);

  // Translate the address to use our optimally chosen radio address bytes
  uint8_t count = 1; uint16_t dec = node;

  #if defined (RF24NetworkMulticast)
    if (pipe != 0 || !node) {
  #endif
    while (dec) {
        out[count]=address_translation[(dec % 8)];      // Convert our decimal values to octal, translate them to address bytes, and set our address
        dec /= 8;
        count++;
    }

    out[0] = address_translation[pipe];     // Set last byte by pipe number

  #if defined (RF24NetworkMulticast)
    } else {
        while(dec) {
            dec/=8;
            count++;
        }
        out[1] = address_translation[count-1];
    }
  #endif

  IF_SERIAL_DEBUG(uint32_t* top = reinterpret_cast<uint32_t*>(out+1);printf_P(PSTR("%u: NET Pipe %i on node 0%o has address %x%x\n\r"),millis(),pipe,node,*top,*out));

  return result;
}


// vim:ai:cin:sts=2 sw=2 ft=cpp
