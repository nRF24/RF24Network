
/*
 Copyright (C) 2011 James Coliz, Jr. <maniacbug@ymail.com>
 
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/**
 * @file sleep.cpp 
 *
 * Sleep helpers, definitions
 */

#include <WProgram.h>
#include <avr/sleep.h>
#include "sleep.h" 

volatile short sleep_cycles_remaining;
short sleep_cycles_per_transmission; 

void setup_watchdog(wdt_prescalar_e prescalar);
void do_sleep(void);

//
// Sleep module wrapper
//

void sleep_begin(wdt_prescalar_e prescalar,short cycles)
{
  sleep_cycles_remaining = cycles;
  sleep_cycles_per_transmission = cycles;

  setup_watchdog(prescalar);
}

void sleep_now(void)
{
  while( sleep_cycles_remaining )
    do_sleep();
  
  sleep_cycles_remaining = sleep_cycles_per_transmission;
}

//
// Universal Sleep helpers 
//

void setup_watchdog(wdt_prescalar_e prescalar_in) 
{
    uint8_t prescalar = min(9,(uint8_t)prescalar_in);
    uint8_t wdtcsr = prescalar & 7;
    if ( prescalar & 8 )
	wdtcsr |= _BV(WDP3);

    MCUSR &= ~_BV(WDRF);
    WDTCSR = _BV(WDCE) | _BV(WDE);
    WDTCSR = _BV(WDCE) | wdtcsr | _BV(WDIE);
}

ISR(WDT_vect) {
  --sleep_cycles_remaining;
}

void do_sleep(void)
{
    set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
    sleep_enable();

    sleep_mode();                        // System sleeps here

    sleep_disable();                     // System continues execution here when watchdog timed out 
}

// vim:ai:cin:sts=2 sw=2 ft=cpp
