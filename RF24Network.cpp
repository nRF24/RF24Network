/*
 Copyright (C) 2011 James Coliz, Jr. <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */
#include "RF24Network_config.h"

#if defined(RF24_LINUX)
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
    #if !defined(USE_RF24_LIB_SRC)
        #include <RF24/RF24.h>
    #endif
#else
    #include "RF24.h"
    #if defined(ARDUINO_ARCH_NRF52) || defined(ARDUINO_ARCH_NRF52840)
        #include <nrf_to_nrf.h>
    #endif
#endif
#include "RF24Network.h"
#if defined(USE_RF24_LIB_SRC)
    #include <RF24.h>
#endif

#if defined(ENABLE_SLEEP_MODE) && defined(ESP8266)
    #warning "Disabling sleep mode because sleep doesn't work on ESP8266"
    #undef ENABLE_SLEEP_MODE
#endif

#if defined(ENABLE_SLEEP_MODE) && !defined(RF24_LINUX) && !defined(__ARDUINO_X86__)
    #include <avr/sleep.h>
    #include <avr/power.h>
volatile byte sleep_cycles_remaining;
volatile bool wasInterrupted;
#endif
uint16_t RF24NetworkHeader::next_id = 1;

#if defined(RF24_LINUX)
/******************************************************************/
template<class radio_t>
ESBNetwork<radio_t>::ESBNetwork(radio_t& _radio) : radio(_radio), frame_size(MAX_FRAME_SIZE)
{
    networkFlags = 0;
    returnSysMsgs = 0;
    multicastRelay = 0;
}
#else
template<class radio_t>
ESBNetwork<radio_t>::ESBNetwork(radio_t& _radio) : radio(_radio), next_frame(frame_queue)
{
    #if !defined(DISABLE_FRAGMENTATION)
    frag_queue.message_buffer = &frag_queue_message_buffer[0];
    frag_ptr = &frag_queue;
    #endif
    networkFlags = 0;
    returnSysMsgs = 0;
    multicastRelay = 0;
}
#endif
/******************************************************************/

template<class radio_t>
void ESBNetwork<radio_t>::begin(uint8_t _channel, uint16_t _node_address)
{
    if (!is_valid_address(_node_address))
        return;

    node_address = _node_address;

    if (!radio.isValid())
        return;

    // Set up the radio the way we want it to look
    if (_channel != USE_CURRENT_CHANNEL)
        radio.setChannel(_channel);

    //radio.enableDynamicAck();
    radio.setAutoAck(1);
    radio.setAutoAck(0, 0);

#if defined(ENABLE_DYNAMIC_PAYLOADS)
    radio.enableDynamicPayloads();
#endif

    // Use different retry periods to reduce data collisions
    uint8_t retryVar = (((node_address % 6) + 1) * 2) + 3;
    radio.setRetries(retryVar, 5); // max about 85ms per attempt
    txTimeout = 25;
    routeTimeout = txTimeout * 3; // Adjust for max delay per node within a single chain

    // Setup our address helper cache
    setup_address();

    // Open up all listening pipes
    uint8_t i = 6;
    while (i--)
        radio.openReadingPipe(i, pipe_address(_node_address, i));

    radio.startListening();
}

#if defined ENABLE_NETWORK_STATS
/******************************************************************/

template<class radio_t>
void ESBNetwork<radio_t>::failures(uint32_t* _fails, uint32_t* _ok)
{
    *_fails = nFails;
    *_ok = nOK;
}
#endif

/******************************************************************/

template<class radio_t>
uint8_t ESBNetwork<radio_t>::update(void)
{

    uint8_t returnVal = 0;

    uint32_t timeout = millis();

    while (radio.available()) {
        if (millis() - timeout > 1000) {
#if defined FAILURE_HANDLING
            radio.failureDetected = 1;
#endif
            break;
        }
#if defined(ENABLE_DYNAMIC_PAYLOADS) && !defined(XMEGA_D3)
        frame_size = radio.getDynamicPayloadSize();
#else
        frame_size = MAX_FRAME_SIZE;
#endif

        // Fetch the payload, and see if this was the last one.
        radio.read(frame_buffer, frame_size);

        // Read the beginning of the frame as the header
        RF24NetworkHeader* header = (RF24NetworkHeader*)(&frame_buffer);

        // Throw it away if it's not a valid address or too small
        if (frame_size < sizeof(RF24NetworkHeader) || !is_valid_address(header->to_node) || !is_valid_address(header->from_node)) {
            continue;
        }
        //IF_SERIAL_DEBUG(printf_P(PSTR("MAC Received " PRIPSTR
        //                              "\n\r"),
        //                         header->toString()));
#if defined(RF24_LINUX)
        if (frame_size) {
            IF_SERIAL_DEBUG_FRAGMENTATION_L2(printf_P(PSTR("%u: FRG Rcv frame size %i\n"), millis(), frame_size););
            IF_SERIAL_DEBUG_FRAGMENTATION_L2(printf_P(PSTR("%u: FRG Rcv frame "), millis()); const char* charPtr = reinterpret_cast<const char*>(frame_buffer); for (uint16_t i = 0; i < frame_size; i++) { printf_P(PSTR("%02X "), charPtr[i]); }; printf_P(PSTR("\n\r")));
        }
#else
        IF_SERIAL_DEBUG(const uint16_t* i = reinterpret_cast<const uint16_t*>(frame_buffer + sizeof(RF24NetworkHeader)); printf_P(PSTR("NET message %04x\n\r"), *i));
#endif

        returnVal = header->type;
        // Is this for us?
        if (header->to_node == node_address) {
            if (header->type == NETWORK_PING) {
                continue;
            }
            if (header->type == NETWORK_ADDR_RESPONSE) {
                uint16_t requester = NETWORK_DEFAULT_ADDRESS;
                if (requester != node_address) {
                    header->to_node = requester;
                    write(header->to_node, USER_TX_TO_PHYSICAL_ADDRESS);
                    continue;
                }
            }
            if (header->type == NETWORK_REQ_ADDRESS && node_address) {
                header->from_node = node_address;
                header->to_node = 0;
                write(header->to_node, TX_NORMAL);
                continue;
            }
            if ((returnSysMsgs && header->type > MAX_USER_DEFINED_HEADER_TYPE) || header->type == NETWORK_ACK) {
                IF_SERIAL_DEBUG_ROUTING(printf_P(PSTR("MAC System payload rcvd %d\n"), returnVal););
                if (header->type != NETWORK_FIRST_FRAGMENT && header->type != NETWORK_MORE_FRAGMENTS && header->type != EXTERNAL_DATA_TYPE && header->type != NETWORK_LAST_FRAGMENT) {
                    return returnVal;
                }
            }

            if (enqueue(header) == 2) { //External data received
                IF_SERIAL_DEBUG_MINIMAL(printf_P(PSTR("ret ext\n")););
                return EXTERNAL_DATA_TYPE;
            }
        }
        else {
#if defined(RF24NetworkMulticast)

            if (header->to_node == NETWORK_MULTICAST_ADDRESS) {
                if (header->type == NETWORK_POLL) {
                    returnVal = 0;
                    if (!(networkFlags & FLAG_NO_POLL) && node_address != NETWORK_DEFAULT_ADDRESS) {
                        header->to_node = header->from_node;
                        header->from_node = node_address;
    #ifdef SLOW_ADDR_POLL_RESPONSE
                        delay(parent_pipe + SLOW_ADDR_POLL_RESPONSE);
    #else
                        delay(parent_pipe);
    #endif
                        write(header->to_node, USER_TX_TO_PHYSICAL_ADDRESS);
                    }
                    continue;
                }

                uint8_t val = enqueue(header);

                if (multicastRelay) {
                    IF_SERIAL_DEBUG_ROUTING(printf_P(PSTR("MAC FWD multicast frame from 0%o to level %u\n"), header->from_node, _multicast_level + 1););
                    if ((node_address >> 3) != 0) {
                        // for all but the first level of nodes, those not directly connected to the master, we add the total delay per level
                        delayMicroseconds(600 * 4);
                    }
                    delayMicroseconds((node_address % 4) * 600);
                    write(levelToAddress(_multicast_level) << 3, USER_TX_MULTICAST);
                }
                if (val == 2) { //External data received
                    return EXTERNAL_DATA_TYPE;
                }
            }
            else {
                if (node_address != NETWORK_DEFAULT_ADDRESS) {
                    write(header->to_node, TX_ROUTED); //Send it on, indicate it is a routed payload
                    returnVal = 0;
                }
            }
#else  // not defined(RF24NetworkMulticast)
            if (node_address != NETWORK_DEFAULT_ADDRESS) {
                write(header->to_node, TX_ROUTED); //Send it on, indicate it is a routed payload
                returnVal = 0;
            }
#endif // defined(RF24NetworkMulticast)
        }

    } // radio.available()
    return returnVal;
}

#if defined(RF24_LINUX)
/******************************************************************/

template<class radio_t>
uint8_t ESBNetwork<radio_t>::enqueue(RF24NetworkHeader* header)
{
    uint8_t result = false;

    RF24NetworkFrame frame = RF24NetworkFrame(*header, frame_buffer + sizeof(RF24NetworkHeader), frame_size - sizeof(RF24NetworkHeader));

    bool isFragment = (frame.header.type == NETWORK_FIRST_FRAGMENT || frame.header.type == NETWORK_MORE_FRAGMENTS || frame.header.type == NETWORK_LAST_FRAGMENT);

    // This is sent to itself
    if (frame.header.from_node == node_address) {
        if (isFragment) {
            printf_P(PSTR("Cannot enqueue multi-payload frames to self\n"));
            result = false;
        }
        else if (frame.header.id > 0) {
            frame_queue.push(frame);
            result = true;
        }
    }
    else if (isFragment) {
        //The received frame contains the a fragmented payload
        //Set the more fragments flag to indicate a fragmented frame
        IF_SERIAL_DEBUG_FRAGMENTATION_L2(printf_P(PSTR("%u: FRG Payload type %d of size %i Bytes with fragmentID '%i' received.\n\r"), millis(), frame.header.type, frame.message_size, frame.header.reserved););
        //Append payload
        result = appendFragmentToFrame(frame);

        //The header.reserved contains the actual header.type on the last fragment
        if (result && frame.header.type == NETWORK_LAST_FRAGMENT) {
            IF_SERIAL_DEBUG_FRAGMENTATION(printf_P(PSTR("%u: FRG Last fragment received\n"), millis()));
            IF_SERIAL_DEBUG(printf_P(PSTR("%u: NET Enqueue assembled frame @ %u\n"), millis(), frame_queue.size()));

            RF24NetworkFrame* f = &(frameFragmentsCache[frame.header.from_node]);

            result = f->header.type == EXTERNAL_DATA_TYPE ? 2 : 1;

            if (f->header.id > 0 && f->message_size > 0) {
                //Load external payloads into a separate queue on linux
                if (result == 2) {
                    external_queue.push(frameFragmentsCache[frame.header.from_node]);
                }
                else {
                    frame_queue.push(frameFragmentsCache[frame.header.from_node]);
                }
            }
            frameFragmentsCache.erase(frame.header.from_node);
        }
    }
    else {
        //if (frame.header.type <= MAX_USER_DEFINED_HEADER_TYPE) {
        //This is not a fragmented payload but a whole frame.

        IF_SERIAL_DEBUG(printf_P(PSTR("%u: NET Enqueue @ %u\n"), millis(), frame_queue.size()));
        // Copy the current frame into the frame queue
        result = frame.header.type == EXTERNAL_DATA_TYPE ? 2 : 1;
        //Load external payloads into a separate queue on linux
        if (result == 2) {
            external_queue.push(frame);
        }
        else {
            frame_queue.push(frame);
        }

    } /* else {
    //Undefined/Unknown header.type received. Drop frame!
    IF_SERIAL_DEBUG_MINIMAL( printf("%u: FRG Received unknown or system header type %d with fragment id %d\n",millis(),frame.header.type, frame.header.reserved); );
    //The frame is not explicitly dropped, but the given object is ignored.
    //FIXME: does this causes problems with memory management?
    }*/

    if (result) {
        //IF_SERIAL_DEBUG(printf_P(PSTR("ok\n\r")));
    }
    else {
        IF_SERIAL_DEBUG(printf_P(PSTR("failed\n\r")));
    }

    return result;
}

/******************************************************************/

template<class radio_t>
bool ESBNetwork<radio_t>::appendFragmentToFrame(RF24NetworkFrame frame)
{

    // This is the first of 2 or more fragments.
    if (frame.header.type == NETWORK_FIRST_FRAGMENT) {
        if (frameFragmentsCache.count(frame.header.from_node) != 0) {
            RF24NetworkFrame* f = &(frameFragmentsCache[frame.header.from_node]);
            //Already rcvd first frag
            if (f->header.id == frame.header.id) {
                return false;
            }
        }

        frameFragmentsCache[frame.header.from_node] = frame;
        return true;
    }
    else if (frame.header.type == NETWORK_MORE_FRAGMENTS) {

        if (frameFragmentsCache.count(frame.header.from_node) < 1) {
            return false;
        }
        RF24NetworkFrame* f = &(frameFragmentsCache[frame.header.from_node]);

        if (f->message_size + frame.message_size > MAX_PAYLOAD_SIZE) {
            IF_SERIAL_DEBUG_FRAGMENTATION(printf_P(PSTR("%u: FRG Frame of size %d plus enqueued frame of size %d exceeds max payload size \n"), millis(), frame.message_size, f->message_size););
            return false;
        }

        if (f->header.reserved - 1 == frame.header.reserved && f->header.id == frame.header.id) {
            // Cache the fragment
            memcpy(f->message_buffer + f->message_size, frame.message_buffer, frame.message_size);
            f->message_size += frame.message_size; //Increment message size
            f->header = frame.header;              //Update header
            return true;
        }
        else {
            IF_SERIAL_DEBUG_FRAGMENTATION(printf_P(PSTR("%u: FRG Dropping fragment for frame with header id:%d, out of order fragment(s).\n"), millis(), frame.header.id););
            return false;
        }
    }
    else if (frame.header.type == NETWORK_LAST_FRAGMENT) {

        //We have received the last fragment
        if (frameFragmentsCache.count(frame.header.from_node) < 1) {
            return false;
        }
        //Create pointer to the cached frame
        RF24NetworkFrame* f = &(frameFragmentsCache[frame.header.from_node]);

        if (f->message_size + frame.message_size > MAX_PAYLOAD_SIZE) {
            IF_SERIAL_DEBUG_FRAGMENTATION(printf_P(PSTR("%u: FRG Frame of size %d plus enqueued frame of size %d exceeds max payload size \n"), millis(), frame.message_size, f->message_size););
            return false;
        }
        //Error checking for missed fragments and payload size
        if (f->header.reserved - 1 != 1 || f->header.id != frame.header.id) {
            IF_SERIAL_DEBUG_FRAGMENTATION(printf_P(PSTR("%u: FRG Duplicate or out of sequence frame %d, expected %d. Cleared.\n\r"), millis(), frame.header.reserved, f->header.reserved););
            //frameFragmentsCache.erase( std::make_pair(frame.header.id,frame.header.from_node) );
            return false;
        }
        //The user specified header.type is sent with the last fragment in the reserved field
        frame.header.type = frame.header.reserved;
        frame.header.reserved = 1;

        //Append the received fragment to the cached frame
        memcpy(f->message_buffer + f->message_size, frame.message_buffer, frame.message_size);
        f->message_size += frame.message_size; //Increment message size
        f->header = frame.header;              //Update header
        return true;
    }
    return false;
}

/******************************************************************/

#else // Not defined RF24_Linux

/******************************************************************/

template<class radio_t>
uint8_t ESBNetwork<radio_t>::enqueue(RF24NetworkHeader* header)
{
    bool result = false;
    uint16_t message_size = frame_size - sizeof(RF24NetworkHeader);

    IF_SERIAL_DEBUG(printf_P(PSTR("NET Enqueue @%x\n"), next_frame - frame_queue));

    #if !defined(DISABLE_FRAGMENTATION)

    bool isFragment = header->type == NETWORK_FIRST_FRAGMENT || header->type == NETWORK_MORE_FRAGMENTS || header->type == NETWORK_LAST_FRAGMENT;

    if (isFragment) {

        if (header->type == NETWORK_FIRST_FRAGMENT) {

            memcpy((char*)(&frag_queue), &frame_buffer, sizeof(RF24NetworkHeader));
            memcpy(frag_queue.message_buffer, frame_buffer + sizeof(RF24NetworkHeader), message_size);

            IF_SERIAL_DEBUG_FRAGMENTATION(printf_P(PSTR("queue first, total frags %d\n\r"), header->reserved););
            //Store the total size of the stored frame in message_size
            frag_queue.message_size = message_size;
            --frag_queue.header.reserved;
            IF_SERIAL_DEBUG_FRAGMENTATION_L2(for (int i = 0; i < frag_queue.message_size; i++) { printf_P(PSTR("%02x"), frag_queue.message_buffer[i]); });
            return true;
        }
        // else if not first fragment
        else if (header->type == NETWORK_LAST_FRAGMENT || header->type == NETWORK_MORE_FRAGMENTS) {

            if (frag_queue.message_size + message_size > MAX_PAYLOAD_SIZE) {
        #if defined(SERIAL_DEBUG_FRAGMENTATION) || defined(SERIAL_DEBUG_MINIMAL)
                printf_P(PSTR("Drop frag %d Size exceeds max\n\r"), header->reserved);
        #endif
                frag_queue.header.reserved = 0;
                return false;
            }
            if (frag_queue.header.reserved == 0 || (header->type != NETWORK_LAST_FRAGMENT && header->reserved != frag_queue.header.reserved) || frag_queue.header.id != header->id) {
        #if defined(SERIAL_DEBUG_FRAGMENTATION) || defined(SERIAL_DEBUG_MINIMAL)
                printf_P(PSTR("Drop frag %d Out of order\n\r"), header->reserved);
        #endif
                return false;
            }

            memcpy(frag_queue.message_buffer + frag_queue.message_size, frame_buffer + sizeof(RF24NetworkHeader), message_size);
            frag_queue.message_size += message_size;

            if (header->type != NETWORK_LAST_FRAGMENT) {
                --frag_queue.header.reserved;
                return true;
            }
            frag_queue.header.reserved = 0;
            frag_queue.header.type = header->reserved;

            IF_SERIAL_DEBUG_FRAGMENTATION(printf_P(PSTR("fq 3: %d\n"), frag_queue.message_size););
            IF_SERIAL_DEBUG_FRAGMENTATION_L2(for (int i = 0; i < frag_queue.message_size; i++) { printf_P(PSTR("%02X"), frag_queue.message_buffer[i]); });

            // Frame assembly complete, copy to main buffer if OK
            if (frag_queue.header.type == EXTERNAL_DATA_TYPE) {
                return 2;
            }
        #if defined(DISABLE_USER_PAYLOADS)
            return 0;
        #endif

            if ((uint16_t)(MAX_PAYLOAD_SIZE) - (next_frame - frame_queue) >= frag_queue.message_size) {
                memcpy(next_frame, &frag_queue, 10);
                memcpy(next_frame + 10, frag_queue.message_buffer, frag_queue.message_size);
                next_frame += (10 + frag_queue.message_size);
        #if !defined(ARDUINO_ARCH_AVR)
                if (uint8_t padding = (frag_queue.message_size + 10) % 4) {
                    next_frame += 4 - padding;
                }
        #endif
                IF_SERIAL_DEBUG_FRAGMENTATION(printf_P(PSTR("enq size %d\n"), frag_queue.message_size););
                return true;
            }
            IF_SERIAL_DEBUG_FRAGMENTATION(printf_P(PSTR("Drop frag payload, queue full\n")););
            return false;

        } //If more or last fragments
    }
    else //else is not a fragment
    #endif // End fragmentation enabled

    // Copy the current frame into the frame queue
    #if !defined(DISABLE_FRAGMENTATION)
        if (header->type == EXTERNAL_DATA_TYPE)
    {
        memcpy((char*)(&frag_queue), &frame_buffer, 8);
        frag_queue.message_buffer = frame_buffer + sizeof(RF24NetworkHeader);
        frag_queue.message_size = message_size;
        return 2;
    }
    #endif
    #if defined(DISABLE_USER_PAYLOADS)
    return 0;
}
    #else // !defined(DISABLE_USER_PAYLOADS)
    if (message_size + (next_frame - frame_queue) <= MAIN_BUFFER_SIZE) {
        memcpy(next_frame, &frame_buffer, 8);
        memcpy(next_frame + 8, &message_size, 2);
        memcpy(next_frame + 10, frame_buffer + 8, message_size);

        //IF_SERIAL_DEBUG_FRAGMENTATION( for(int i=0; i<message_size;i++){ Serial.print(next_frame[i],HEX); Serial.print(" : "); } Serial.println(""); );

        next_frame += (message_size + 10);
        #if !defined(ARDUINO_ARCH_AVR)
        if (uint8_t padding = (message_size + 10) % 4) {
            next_frame += 4 - padding;
        }
        #endif
        //IF_SERIAL_DEBUG_FRAGMENTATION( Serial.print("Enq "); Serial.println(next_frame-frame_queue); );//printf_P(PSTR("enq %d\n"),next_frame-frame_queue); );

        result = true;
    }
    else {
        result = false;
        IF_SERIAL_DEBUG(printf_P(PSTR("NET **Drop Payload** Buffer Full")));
    }
    return result;
}
    #endif //USER_PAYLOADS_ENABLED

#endif //End not defined RF24_Linux
/******************************************************************/

template<class radio_t>
bool ESBNetwork<radio_t>::available(void)
{
#if defined(RF24_LINUX)
    return (!frame_queue.empty());
#else
    // Are there frames on the queue for us?
    return (next_frame > frame_queue);
#endif
}

/******************************************************************/

template<class radio_t>
uint16_t ESBNetwork<radio_t>::parent() const
{
    if (node_address == 0)
        return -1;
    else
        return parent_node;
}

/******************************************************************/

template<class radio_t>
uint16_t ESBNetwork<radio_t>::peek(RF24NetworkHeader& header)
{
    if (available()) {
#if defined(RF24_LINUX)
        RF24NetworkFrame frame = frame_queue.front();
        memcpy(&header, &frame.header, sizeof(RF24NetworkHeader));
        return frame.message_size;
#else
        RF24NetworkFrame* frame = (RF24NetworkFrame*)(frame_queue);
        memcpy(&header, &frame->header, sizeof(RF24NetworkHeader));
        uint16_t msg_size;
        memcpy(&msg_size, frame_queue + 8, 2);
        return msg_size;
#endif
    }
    return 0;
}

/******************************************************************/

template<class radio_t>
void ESBNetwork<radio_t>::peek(RF24NetworkHeader& header, void* message, uint16_t maxlen)
{
    if (available()) {
#if defined(RF24_LINUX)
        RF24NetworkFrame frame = frame_queue.front();
        memcpy(&header, &(frame.header), sizeof(RF24NetworkHeader));
        if (maxlen > 0) {
            maxlen = rf24_min(frame.message_size, maxlen);
            memcpy(message, frame.message_buffer, maxlen);
        }
#else
        memcpy(&header, frame_queue, 8); //Copy the header
        if (maxlen > 0) {
            uint16_t bufsize = 0;
            memcpy(&bufsize, frame_queue + 8, 2);
            maxlen = rf24_min(bufsize, maxlen);
            memcpy(message, frame_queue + 10, maxlen); //Copy the message
        }
#endif
    }
}

/******************************************************************/

template<class radio_t>
uint16_t ESBNetwork<radio_t>::read(RF24NetworkHeader& header, void* message, uint16_t maxlen)
{
    uint16_t bufsize = 0;

    // This function assumes that there is a frame in the queue.
    // Call `available()` before calling `read()`.
    //if (!available()) { return bufsize; }

#if defined(RF24_LINUX)
    RF24NetworkFrame frame = frame_queue.front();

    // How much buffer size should we actually copy?
    bufsize = rf24_min(frame.message_size, maxlen);
    memcpy(&header, &(frame.header), sizeof(RF24NetworkHeader));
    memcpy(message, frame.message_buffer, bufsize);

    IF_SERIAL_DEBUG(printf_P(PSTR("%u: FRG message size %i\n"), millis(), frame.message_size););
    IF_SERIAL_DEBUG(printf_P(PSTR("%u: FRG message "), millis()); const char* charPtr = reinterpret_cast<const char*>(message); for (uint16_t i = 0; i < bufsize; i++) { printf_P(PSTR("%02X "), charPtr[i]); }; printf(PSTR("\n\r")));

    IF_SERIAL_DEBUG(printf_P(PSTR("%u: NET read " PRIPSTR
                                  "\n\r"),
                             millis(), header.toString()));

    frame_queue.pop();

#else // !defined(RF24_LINUX)

    memcpy(&header, frame_queue, 8);
    memcpy(&bufsize, frame_queue + 8, 2);

    if (maxlen > 0) {
        maxlen = rf24_min(maxlen, bufsize);
        memcpy(message, frame_queue + 10, maxlen);
        IF_SERIAL_DEBUG(printf_P(PSTR("NET message size %d\n"), bufsize););

        IF_SERIAL_DEBUG(uint16_t len = maxlen; printf_P(PSTR("NET message ")); const uint8_t* charPtr = reinterpret_cast<const uint8_t*>(message); while (len--) { printf_P(PSTR("%02x "), charPtr[len]); } printf_P(PSTR("\n\r")));
    }
    next_frame -= bufsize + 10;
    uint8_t padding = 0;
    #if !defined(ARDUINO_ARCH_AVR)
    if ((padding = (bufsize + 10) % 4)) {
        padding = 4 - padding;
        next_frame -= padding;
    }
    #endif // !defined(ARDUINO_ARCH_AVR)
    memmove(frame_queue, frame_queue + bufsize + 10 + padding, sizeof(frame_queue) - bufsize);
    //IF_SERIAL_DEBUG(printf_P(PSTR("%u: NET Received %s\n\r"), millis(), header.toString()));

#endif // !defined(RF24_LINUX)
    return bufsize;
}

#if defined RF24NetworkMulticast
/******************************************************************/

template<class radio_t>
bool ESBNetwork<radio_t>::multicast(RF24NetworkHeader& header, const void* message, uint16_t len, uint8_t level)
{
    // Fill out the header
    header.to_node = NETWORK_MULTICAST_ADDRESS;
    header.from_node = node_address;
    return write(header, message, len, levelToAddress(level > 4 ? _multicast_level : level));
}
#endif

/******************************************************************/

template<class radio_t>
bool ESBNetwork<radio_t>::write(RF24NetworkHeader& header, const void* message, uint16_t len)
{
    return write(header, message, len, NETWORK_AUTO_ROUTING);
}

/******************************************************************/

template<class radio_t>
bool ESBNetwork<radio_t>::write(RF24NetworkHeader& header, const void* message, uint16_t len, uint16_t writeDirect)
{

#if defined(DISABLE_FRAGMENTATION)
    frame_size = rf24_min(len + sizeof(RF24NetworkHeader), MAX_FRAME_SIZE);
    return _write(header, message, rf24_min(len, max_frame_payload_size), writeDirect);
#else // !defined(DISABLE_FRAGMENTATION)
    if (len <= max_frame_payload_size) {
        //Normal Write (Un-Fragmented)
        frame_size = len + sizeof(RF24NetworkHeader);
        return _write(header, message, len, writeDirect);
    }
    //Check payload size

    if (len > MAX_PAYLOAD_SIZE) {
        IF_SERIAL_DEBUG(printf_P(PSTR("NET write message failed. Given 'len' %d is bigger than the MAX Payload size %i\n\r"), len, MAX_PAYLOAD_SIZE););
        return false;
    }

    //Divide the message payload into chunks of max_frame_payload_size
    uint8_t fragment_id = (len % max_frame_payload_size != 0) + ((len) / max_frame_payload_size); //the number of fragments to send = ceil(len/max_frame_payload_size)

    uint8_t msgCount = 0;

    IF_SERIAL_DEBUG_FRAGMENTATION(printf_P(PSTR("FRG Total message fragments %d\n\r"), fragment_id););

    if (header.to_node != NETWORK_MULTICAST_ADDRESS) {
        networkFlags |= FLAG_FAST_FRAG;
        radio.stopListening();
    }

    uint8_t retriesPerFrag = 0;
    uint8_t type = header.type;
    bool ok = 0;

    while (fragment_id > 0) {

        //Copy and fill out the header
        //RF24NetworkHeader fragmentHeader = header;
        header.reserved = fragment_id;

        if (fragment_id == 1) {
            header.type = NETWORK_LAST_FRAGMENT; //Set the last fragment flag to indicate the last fragment
            header.reserved = type;              //The reserved field is used to transmit the header type
        }
        else if (msgCount == 0) {
            header.type = NETWORK_FIRST_FRAGMENT;
        }
        else {
            header.type = NETWORK_MORE_FRAGMENTS; //Set the more fragments flag to indicate a fragmented frame
        }

        uint16_t offset = msgCount * max_frame_payload_size;
        uint16_t fragmentLen = rf24_min((uint16_t)(len - offset), max_frame_payload_size);

        //Try to send the payload chunk with the copied header
        frame_size = sizeof(RF24NetworkHeader) + fragmentLen;
        ok = _write(header, ((char*)message) + offset, fragmentLen, writeDirect);

        if (!ok) {
            delay(2);
            ++retriesPerFrag;
        }
        else {
            retriesPerFrag = 0;
            fragment_id--;
            msgCount++;
        }

        //if(writeDirect != NETWORK_AUTO_ROUTING){ delay(2); } //Delay 2ms between sending multicast payloads

        if (!ok && retriesPerFrag >= 3) {
            IF_SERIAL_DEBUG_FRAGMENTATION(printf_P(PSTR("FRG TX with fragmentID '%d' failed after %d fragments. Abort.\n\r"), fragment_id, msgCount));
            break;
        }

        // Message was successful sent
        IF_SERIAL_DEBUG_FRAGMENTATION_L2(printf_P(PSTR("FRG message transmission with fragmentID '%d' successful.\n\r"), fragment_id));
    }
    header.type = type;
    if (networkFlags & FLAG_FAST_FRAG) {
        ok = radio.txStandBy(txTimeout);
        radio.startListening();
        radio.setAutoAck(0, 0);
    }
    networkFlags &= ~FLAG_FAST_FRAG;

    // Return true if all the chunks where sent successfully
    IF_SERIAL_DEBUG_FRAGMENTATION(printf_P(PSTR("FRG total message fragments sent %i.\r\n"), msgCount););

    if (!ok || fragment_id > 0) {
        return false;
    }
    return true;

#endif //Fragmentation enabled
}

/******************************************************************/

template<class radio_t>
bool ESBNetwork<radio_t>::_write(RF24NetworkHeader& header, const void* message, uint16_t len, uint16_t writeDirect)
{
    // Fill out the header
    header.from_node = node_address;

    // Build the full frame to send
    memcpy(frame_buffer, &header, sizeof(RF24NetworkHeader));

    //IF_SERIAL_DEBUG(printf_P(PSTR("NET Sending " PRIPSTR
    //                              "\n\r"),
    //                         header.toString()));
    if (len) {
#if defined(RF24_LINUX)
        memcpy(frame_buffer + sizeof(RF24NetworkHeader), message, rf24_min(frame_size - sizeof(RF24NetworkHeader), len));
        IF_SERIAL_DEBUG(printf_P(PSTR("%u: FRG frame size %i\n"), millis(), frame_size););
        IF_SERIAL_DEBUG(printf_P(PSTR("%u: FRG frame "), millis()); const char* charPtr = reinterpret_cast<const char*>(frame_buffer); for (uint16_t i = 0; i < frame_size; i++) { printf_P(PSTR("%02X "), charPtr[i]); }; printf_P(PSTR("\n\r")));
#else

        memcpy(frame_buffer + sizeof(RF24NetworkHeader), message, len);

        IF_SERIAL_DEBUG(uint16_t tmpLen = len; printf_P(PSTR("NET message ")); const uint8_t* charPtr = reinterpret_cast<const uint8_t*>(message); while (tmpLen--) { printf_P(PSTR("%02x "), charPtr[tmpLen]); } printf_P(PSTR("\n\r")));
#endif
    }

    // If the user is trying to send it to himself
    /*if (header.to_node == node_address) {
    #if defined (RF24_LINUX)
        RF24NetworkFrame frame = RF24NetworkFrame(header, message, rf24_min(MAX_FRAME_SIZE - sizeof(RF24NetworkHeader), len));
    #else
        RF24NetworkFrame frame(header, len);
    #endif
        // Just queue it in the received queue
        return enqueue(frame);
    }*/
    // Otherwise send it out over the air

    if (writeDirect != NETWORK_AUTO_ROUTING) {
        uint8_t sendType = USER_TX_TO_LOGICAL_ADDRESS; // Payload is multicast to the first node, and routed normally to the next

        if (header.to_node == NETWORK_MULTICAST_ADDRESS)
            sendType = USER_TX_MULTICAST;
        if (header.to_node == writeDirect)
            sendType = USER_TX_TO_PHYSICAL_ADDRESS; // Payload is multicast to the first node, which is the recipient

        return write(writeDirect, sendType);
    }
    return write(header.to_node, TX_NORMAL);
}

/******************************************************************/

template<class radio_t>
bool ESBNetwork<radio_t>::write(uint16_t to_node, uint8_t sendType)
{
    bool ok = false;
    bool isAckType = false;
    if (frame_buffer[6] > 64 && frame_buffer[6] < 192)
        isAckType = true;

    /*if( ( (frame_buffer[7] % 2) && frame_buffer[6] == NETWORK_MORE_FRAGMENTS) ){
	isAckType = 0;
    }*/

    // Throw it away if it's not a valid address
    if (!is_valid_address(to_node))
        return false;

    //Load info into our conversion structure, and get the converted address info
    logicalToPhysicalStruct conversion = {to_node, sendType, 0};
    logicalToPhysicalAddress(&conversion);

    IF_SERIAL_DEBUG(printf_P(PSTR("MAC Sending to 0%o via 0%o on pipe %x\n\r"), to_node, conversion.send_node, conversion.send_pipe));
    /**Write it*/
    if (sendType == TX_ROUTED && conversion.send_node == to_node && isAckType) {
        delay(2);
    }
    ok = write_to_pipe(conversion.send_node, conversion.send_pipe, conversion.multicast);

    if (!ok) {
        IF_SERIAL_DEBUG_ROUTING(printf_P(PSTR("MAC Send fail to 0%o via 0%o on pipe %x\n\r"), to_node, conversion.send_node, conversion.send_pipe););
    }

    if (sendType == TX_ROUTED && ok && conversion.send_node == to_node && isAckType) {
        // NETWORK_ACK messages are only sent by the last node in the route to a target node.
        // ie: Node 00 sends to node 011, node 01 will send the network ack to 00 upon delivery.
        // Any node receiving a NETWORK_ACK message will route it as a normal message.

        RF24NetworkHeader* header = (RF24NetworkHeader*)&frame_buffer;
        header->type = NETWORK_ACK;          // Set the payload type to NETWORK_ACK
        header->to_node = header->from_node; // Change the 'to' address to the 'from' address

        conversion.send_node = header->from_node;
        conversion.send_pipe = TX_ROUTED;
        conversion.multicast = 0;
        logicalToPhysicalAddress(&conversion);

        //Write the data using the resulting physical address
        frame_size = sizeof(RF24NetworkHeader);
        write_to_pipe(conversion.send_node, conversion.send_pipe, conversion.multicast);

        // dynLen=0;
        IF_SERIAL_DEBUG_ROUTING(printf_P(PSTR("MAC Route OK to 0%o ACK sent to 0%o\n"), to_node, header->from_node););
    }

    if (ok && conversion.send_node != to_node && (sendType == TX_NORMAL || sendType == USER_TX_TO_LOGICAL_ADDRESS) && isAckType) {
        // Now, continue listening
        if (networkFlags & FLAG_FAST_FRAG) {
            radio.txStandBy(txTimeout);
            networkFlags &= ~FLAG_FAST_FRAG;
            radio.setAutoAck(0, 0);
        }
        radio.startListening();
        uint32_t reply_time = millis();

        while (update() != NETWORK_ACK) {
#if defined(RF24_LINUX)
            delayMicroseconds(900);
#endif
            if (millis() - reply_time > routeTimeout) {
                IF_SERIAL_DEBUG_ROUTING(printf_P(PSTR("MAC Network ACK fail from 0%o via 0%o on pipe %x\n\r"), to_node, conversion.send_node, conversion.send_pipe););
                ok = false;
                break;
            }
        }
    }
    if (!(networkFlags & FLAG_FAST_FRAG)) {
        // Now, continue listening
        radio.startListening();
    }

#if defined ENABLE_NETWORK_STATS
    if (ok == true) {
        ++nOK;
    }
    else {
        ++nFails;
    }
#endif
    return ok;
}

/******************************************************************/

template<class radio_t>
void ESBNetwork<radio_t>::logicalToPhysicalAddress(logicalToPhysicalStruct* conversionInfo)
{

    //Create pointers so this makes sense.. kind of
    //We take in the to_node(logical) now, at the end of the function, output the send_node(physical) address, etc.
    //back to the original memory address that held the logical information.
    uint16_t* to_node = &conversionInfo->send_node;
    uint8_t* directTo = &conversionInfo->send_pipe;
    bool* multicast = &conversionInfo->multicast;

    // Where do we send this?  By default, to our parent
    uint16_t pre_conversion_send_node = parent_node;

    // On which pipe
    uint8_t pre_conversion_send_pipe = parent_pipe;

    if (*directTo > TX_ROUTED) {
        pre_conversion_send_node = *to_node;
        *multicast = 1;
        //if(*directTo == USER_TX_MULTICAST || *directTo == USER_TX_TO_PHYSICAL_ADDRESS){
        pre_conversion_send_pipe = 0;
        //}
    }
    else if (is_descendant(*to_node)) {
        pre_conversion_send_pipe = 5; // Send to its listening pipe
        // If the node is a direct child,
        if (is_direct_child(*to_node)) {
            // Send directly
            pre_conversion_send_node = *to_node;
        }
        // If the node is a child of a child
        // talk on our child's listening pipe,
        // and let the direct child relay it.
        else {
            pre_conversion_send_node = direct_child_route_to(*to_node);
        }
    }

    *to_node = pre_conversion_send_node;
    *directTo = pre_conversion_send_pipe;
}

/********************************************************/

template<class radio_t>
bool ESBNetwork<radio_t>::write_to_pipe(uint16_t node, uint8_t pipe, bool multicast)
{
    bool ok = false;

    // Open the correct pipe for writing.
    // First, stop listening so we can talk
    if (!(networkFlags & FLAG_FAST_FRAG)) {
        radio.stopListening();
    }

    if (!(networkFlags & FLAG_FAST_FRAG) || (frame_buffer[6] == NETWORK_FIRST_FRAGMENT && networkFlags & FLAG_FAST_FRAG)) {
        radio.setAutoAck(0, !multicast);
        radio.openWritingPipe(pipe_address(node, pipe));
    }

    ok = radio.writeFast(frame_buffer, frame_size, 0);

    if (!(networkFlags & FLAG_FAST_FRAG)) {
        ok = radio.txStandBy(txTimeout);
        radio.setAutoAck(0, 0);
    }

    /*
    #if defined (__arm__) || defined (RF24_LINUX)
    IF_SERIAL_DEBUG(printf_P(PSTR("%u: MAC Sent on %x %s\n\r"), millis(), (uint32_t)out_pipe, ok ? PSTR("ok") : PSTR("failed")));
    #else
    IF_SERIAL_DEBUG(printf_P(PSTR("%u: MAC Sent on %lx %S\n\r"), millis(), (uint32_t)out_pipe, ok ? PSTR("ok") : PSTR("failed")));
    #endif
    */
    return ok;
}

/******************************************************************/

const char* RF24NetworkHeader::toString(void) const
{
    static char buffer[45];
    //snprintf_P(buffer,sizeof(buffer),PSTR("id %04x from 0%o to 0%o type %c"),id,from_node,to_node,type);
    sprintf_P(buffer, PSTR("id %u from 0%o to 0%o type %d"), id, from_node, to_node, type);
    return buffer;
}

/******************************************************************/

template<class radio_t>
bool ESBNetwork<radio_t>::is_direct_child(uint16_t node)
{
    // A direct child of ours has the same low numbers as us, and only
    // one higher number.
    //
    // e.g. node 0234 is a direct child of 034, and node 01234 is a
    // descendant but not a direct child

    // Does it only have ONE more level than us?
    uint16_t child_node_mask = (~node_mask) << 3;
    return (node & child_node_mask) == 0;
}

/******************************************************************/

template<class radio_t>
bool ESBNetwork<radio_t>::is_descendant(uint16_t node)
{
    return (node & node_mask) == node_address;
}

/******************************************************************/

template<class radio_t>
void ESBNetwork<radio_t>::setup_address(void)
{
    // First, establish the node_mask
    uint16_t node_mask_check = 0xFFFF;
#if defined(RF24NetworkMulticast)
    uint8_t count = 0;
#endif

    while (node_address & node_mask_check) {
        node_mask_check <<= 3;
#if defined(RF24NetworkMulticast)
        count++;
    }
    _multicast_level = count;
#else
    }
#endif

    node_mask = ~node_mask_check;

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

    IF_SERIAL_DEBUG_MINIMAL(printf_P(PSTR("setup_address node=0%o mask=0%o parent=0%o pipe=0%o\n\r"), node_address, node_mask, parent_node, parent_pipe););
    //  IF_SERIAL_DEBUG_MINIMAL(Serial.print(F("setup_address node=")));
    //  IF_SERIAL_DEBUG_MINIMAL(Serial.print(node_address,OCT));
    //  IF_SERIAL_DEBUG_MINIMAL(Serial.print(F(" parent=")));
    //  IF_SERIAL_DEBUG_MINIMAL(Serial.println(parent_node,OCT));
}

/******************************************************************/

template<class radio_t>
uint16_t ESBNetwork<radio_t>::addressOfPipe(uint16_t node, uint8_t pipeNo)
{
    //Say this node is 013 (1011), mask is 077 or (00111111)
    //Say we want to use pipe 3 (11)
    //6 bits in node mask, so shift pipeNo 6 times left and | into address
    uint16_t m = node_mask >> 3;
    uint8_t i = 0;

    while (m) {  //While there are bits left in the node mask
        m >>= 1; //Shift to the right
        i++;     //Count the # of increments
    }
    return node | (pipeNo << i);
}

/******************************************************************/

template<class radio_t>
uint16_t ESBNetwork<radio_t>::direct_child_route_to(uint16_t node)
{
    // Presumes that this is in fact a child!!
    uint16_t child_mask = (node_mask << 3) | 0x07;
    return node & child_mask;
}

/******************************************************************/

template<class radio_t>
bool ESBNetwork<radio_t>::is_valid_address(uint16_t node)
{
    bool result = true;
    if (node == NETWORK_MULTICAST_ADDRESS || node == 010) {
        return result;
    }
    uint8_t count = 0;
#if defined(SERIAL_DEBUG_MINIMAL)
    uint16_t origNode = node;
#endif
    while (node) {
        uint8_t digit = node & 0x07;
        if (digit < 1 || digit > 5) {
            result = false;
            IF_SERIAL_DEBUG_MINIMAL(printf_P(PSTR("*** WARNING *** Invalid address 0%o\n\r"), origNode););
            break;
        }
        node >>= 3;
        count++;
    }

    if (count > 4) {
        IF_SERIAL_DEBUG_MINIMAL(printf_P(PSTR("*** WARNING *** Invalid address 0%o\n\r"), origNode););
        return false;
    }
    return result;
}

#if defined(RF24NetworkMulticast)
/******************************************************************/

template<class radio_t>
void ESBNetwork<radio_t>::multicastLevel(uint8_t level)
{
    _multicast_level = level;
    radio.stopListening();
    radio.openReadingPipe(0, pipe_address(levelToAddress(level), 0));
    radio.startListening();
}

/******************************************************************/

template<class radio_t>
uint16_t ESBNetwork<radio_t>::levelToAddress(uint8_t level)
{

    uint16_t levelAddr = 1;
    if (level)
        levelAddr = levelAddr << ((level - 1) * 3);
    else
        return 0;

    return levelAddr;
}

#endif // !defined(RF24NetworkMulticast)
/******************************************************************/

template<class radio_t>
uint64_t ESBNetwork<radio_t>::pipe_address(uint16_t node, uint8_t pipe)
{

    static uint8_t address_translation[] = {0xc3, 0x3c, 0x33, 0xce, 0x3e, 0xe3, 0xec};
    uint64_t result = 0xCCCCCCCCCCLL;
    uint8_t* out = reinterpret_cast<uint8_t*>(&result);

    // Translate the address to use our optimally chosen radio address bytes
    uint8_t count = 1;
    uint16_t dec = node;

    while (dec) {
#if defined(RF24NetworkMulticast)
        if (pipe != 0 || !node)
#endif
            out[count] = address_translation[(dec % 8)]; // Convert our decimal values to octal, translate them to address bytes, and set our address

        dec /= 8;
        count++;
    }

#if defined(RF24NetworkMulticast)
    if (pipe != 0 || !node)
#endif
        out[0] = address_translation[pipe];
#if defined(RF24NetworkMulticast)
    else
        out[1] = address_translation[count - 1];
#endif
    IF_SERIAL_DEBUG(uint32_t* top = reinterpret_cast<uint32_t*>(out + 1); printf_P(PSTR("NET Pipe %i on node 0%o has address %x%x\n\r"), pipe, node, *top, *out));

    return result;
}

/************************ Sleep Mode ******************************************/

#if defined ENABLE_SLEEP_MODE

    #if !defined(__arm__) && !defined(__ARDUINO_X86__)

void wakeUp()
{
    wasInterrupted = true;
    sleep_cycles_remaining = 0;
}

ISR(WDT_vect)
{
    --sleep_cycles_remaining;
}

template<class radio_t>
bool ESBNetwork<radio_t>::sleepNode(unsigned int cycles, int interruptPin, uint8_t INTERRUPT_MODE)
{
    sleep_cycles_remaining = cycles;
    set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
    sleep_enable();
    if (interruptPin != 255) {
        wasInterrupted = false; //Reset Flag
        //LOW,CHANGE, FALLING, RISING correspond with the values 0,1,2,3 respectively
        attachInterrupt(interruptPin, wakeUp, INTERRUPT_MODE);
        //if(INTERRUPT_MODE==0) attachInterrupt(interruptPin,wakeUp, LOW);
        //if(INTERRUPT_MODE==1) attachInterrupt(interruptPin,wakeUp, RISING);
        //if(INTERRUPT_MODE==2) attachInterrupt(interruptPin,wakeUp, FALLING);
        //if(INTERRUPT_MODE==3) attachInterrupt(interruptPin,wakeUp, CHANGE);
    }

        #if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    WDTCR |= _BV(WDIE);
        #else
    WDTCSR |= _BV(WDIE);
        #endif

    while (sleep_cycles_remaining) {
        sleep_mode(); // System sleeps here
    }                 // The WDT_vect interrupt wakes the MCU from here
    sleep_disable();  // System continues execution here when watchdog timed out
    detachInterrupt(interruptPin);

        #if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    WDTCR &= ~_BV(WDIE);
        #else
    WDTCSR &= ~_BV(WDIE);
        #endif
    return !wasInterrupted;
}

template<class radio_t>
void ESBNetwork<radio_t>::setup_watchdog(uint8_t prescalar)
{

    uint8_t wdtcsr = prescalar & 7;
    if (prescalar & 8)
        wdtcsr |= _BV(WDP3);
    MCUSR &= ~_BV(WDRF); // Clear the WD System Reset Flag

        #if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    WDTCR = _BV(WDCE) | _BV(WDE);           // Write the WD Change enable bit to enable changing the prescaler and enable system reset
    WDTCR = _BV(WDCE) | wdtcsr | _BV(WDIE); // Write the prescalar bits (how long to sleep, enable the interrupt to wake the MCU
        #else
    WDTCSR = _BV(WDCE) | _BV(WDE);           // Write the WD Change enable bit to enable changing the prescaler and enable system reset
    WDTCSR = _BV(WDCE) | wdtcsr | _BV(WDIE); // Write the prescalar bits (how long to sleep, enable the interrupt to wake the MCU
        #endif
}

    #endif // not ATTiny
#endif     // Enable sleep mode

// ensure the compiler is aware of the possible datatype for the template class
template class ESBNetwork<RF24>;
#if defined(ARDUINO_ARCH_NRF52) || defined(ARDUINO_ARCH_NRF52840) || defined(ARDUINO_ARCH_NRF52833)
template class ESBNetwork<nrf_to_nrf>;
#endif
