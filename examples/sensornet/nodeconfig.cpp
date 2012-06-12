/*
 Copyright (C) 2011 James Coliz, Jr. <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#include "RF24Network_config.h"
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include "nodeconfig.h"

// Where in EEPROM is the address stored?
uint8_t* address_at_eeprom_location = (uint8_t*)10;

// What flag value is stored there so we know the value is valid?
const uint8_t valid_eeprom_flag = 0xde;

eeprom_info_t eeprom_info;

const eeprom_info_t& nodeconfig_read(void)
{
  memset(&eeprom_info,0,sizeof(eeprom_info));

  // Look for the token in EEPROM to indicate the following value is
  // a validly set node address 
  if ( eeprom_read_byte(address_at_eeprom_location) == valid_eeprom_flag )
  {
    eeprom_read_block(&eeprom_info,address_at_eeprom_location,sizeof(eeprom_info));
    printf_P(PSTR("ADDRESS: %o\n\r"),eeprom_info.address);
    printf_P(PSTR("ROLE: %S\n\r"),eeprom_info.relay ? PSTR("Relay") : PSTR("Leaf") );
  }
  else
  {
    printf_P(PSTR("*** No valid address found.  Send node address via serial of the form 011<cr>\n\r"));
    while(1)
    {
      nodeconfig_listen();
    }
  }
  
  return eeprom_info;
}

char serialdata[10];
char* nextserialat = serialdata;
const char* maxserial = serialdata + sizeof(serialdata) - 1;

void nodeconfig_listen(void)
{
  //
  // Listen for serial input, which is how we set the address
  //
  if (Serial.available())
  {
    // If the character on serial input is in a valid range...
    char c = Serial.read();
    if ( c >= '0' && c <= '5' )
    {
      *nextserialat++ = c;
      if ( nextserialat == maxserial )
      {
	*nextserialat = 0;
	printf_P(PSTR("\r\n*** Unknown serial command: %s\r\n"),serialdata);
	nextserialat = serialdata;
      }
    }
    else if ( tolower(c) == 'r' )
    {
      eeprom_info.relay = true;
      printf_P(PSTR("ROLE: %S\n\r"),eeprom_info.relay ? PSTR("Relay") : PSTR("Leaf") );
      if ( eeprom_info.flag == valid_eeprom_flag )
	eeprom_update_block(&eeprom_info,address_at_eeprom_location,sizeof(eeprom_info));
      else
	printf_P(PSTR("Please assign an address to commit this role to EEPROM\r\n"));
    }
    else if ( tolower(c) == 'l' )
    {
      eeprom_info.relay = false;
      printf_P(PSTR("ROLE: %S\n\r"),eeprom_info.relay ? PSTR("Relay") : PSTR("Leaf") );
      if ( eeprom_info.flag == valid_eeprom_flag )
	eeprom_update_block(&eeprom_info,address_at_eeprom_location,sizeof(eeprom_info));
      else
	printf_P(PSTR("Please assign an address to commit this role to EEPROM\r\n"));
    }
    else if ( c == 13 )
    {
      // Convert to octal
      char *pc = serialdata;
      uint16_t address = 0;
      while ( pc < nextserialat )
      {
	address <<= 3;
	address |= (*pc++ - '0');
      }

      // It is our address
      eeprom_info.flag = valid_eeprom_flag;
      eeprom_info.address = address;
      eeprom_update_block(&eeprom_info,address_at_eeprom_location,sizeof(eeprom_info));

      // And we are done right now (no easy way to soft reset)
      printf_P(PSTR("\n\rManually set to address 0%o\n\rPress RESET to continue!"),address);
      while(1);
    }
  }
}
// vim:ai:cin:sts=2 sw=2 ft=cpp
