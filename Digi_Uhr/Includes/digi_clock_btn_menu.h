/*
 * digi_clock_btn_menu.h
 *
 *  Created on: 16.08.2020
 *      Author: andi
 */

#include <stdbool.h>
#include <stdint.h>
#include <msp430.h>

#ifndef INCLUDES_DIGI_CLOCK_BTN_MENU_H_
#define INCLUDES_DIGI_CLOCK_BTN_MENU_H_

void btn_to_event(void);

void clock_event_to_menu(uint8_t menu_stop_flag);

#endif /* INCLUDES_DIGI_CLOCK_BTN_MENU_H_ */
