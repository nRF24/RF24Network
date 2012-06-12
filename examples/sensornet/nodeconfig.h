/*
 Copyright (C) 2011 James Coliz, Jr. <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#ifndef __NODECONFIG_H__
#define __NODECONFIG_H__

// Additional info
struct eeprom_info_t
{
  uint8_t flag;
  uint16_t address;
  bool relay:1;
};

const eeprom_info_t& nodeconfig_read(void);
void nodeconfig_listen(void);

#endif // __NODECONFIG_H__
