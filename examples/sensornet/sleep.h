
/*
 Copyright (C) 2011 James Coliz, Jr. <maniacbug@ymail.com>
 
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/**
 * @file sleep.h 
 *
 * Sleep helpers, public interface 
 */
 
#ifndef SLEEP_H
#define SLEEP_H

typedef enum { wdt_16ms = 0, wdt_32ms, wdt_64ms, wdt_128ms, wdt_250ms, wdt_500ms, wdt_1s, wdt_2s, wdt_4s, wdt_8s } wdt_prescalar_e;

void sleep_begin(wdt_prescalar_e prescalar,short cycles);
void sleep_now(void);

#endif // SLEEP_H
// vim:ai:cin:sts=2 sw=2 ft=cpp
