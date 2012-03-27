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

uint16_t nodeconfig_read(void)
{
  uint16_t result = 0;

  // Look for the token in EEPROM to indicate the following value is
  // a validly set node address 
  if ( eeprom_read_byte(address_at_eeprom_location) == valid_eeprom_flag )
  {
    // Read the address from EEPROM
    result = eeprom_read_byte(address_at_eeprom_location+1) | ((uint16_t)eeprom_read_byte(address_at_eeprom_location+2) << 8 );
    printf_P(PSTR("ADDRESS: %o\n\r"),result);
  }
  else
  {
    printf_P(PSTR("*** No valid address found.  Send node address via serial of the form 011<cr>\n\r"));
    while(1)
    {
      nodeconfig_listen();
    }
  }
  
  return result;
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
      eeprom_write_byte(address_at_eeprom_location,valid_eeprom_flag);
      eeprom_write_byte(address_at_eeprom_location+1,address & 0xff);
      eeprom_write_byte(address_at_eeprom_location+2,address >> 8);

      // And we are done right now (no easy way to soft reset)
      printf_P(PSTR("\n\rManually set to address 0%o\n\rPress RESET to continue!"),address);
      while(1);
    }
  }
}
// vim:ai:cin:sts=2 sw=2 ft=cpp
