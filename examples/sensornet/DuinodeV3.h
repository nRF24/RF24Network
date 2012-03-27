#ifndef __DUINODE_V3_H__
#define __DUINODE_V3_H__

/**
 * @file DuinodeV3.h
 *
 * Contains hardware definitions for RF Duinode V3 (2V4)
 */

#define PINS_DEFINED 1 
#define __PLATFORM__ "RF Duinode V3/V4 (2V4)"

const int rf_irq = 0;

const int red_led_pin = 3;
const int yel_led_pin = 4;
const int grn_led_pin = 5;
const int button_1_pin = 6;

const int rf_ce_pin = 8;
const int rf_csn_pin = 7;

const int temp_pin = 2; // analog
const int voltage_pin = 3; // analog

// 1.1V internal reference after 1M/470k divider, in 8-bit fixed point
const unsigned voltage_reference = 0x371;

#endif // __DUINODE_V3_H__
// vim:cin:ai:sts=2 sw=2 ft=cpp
