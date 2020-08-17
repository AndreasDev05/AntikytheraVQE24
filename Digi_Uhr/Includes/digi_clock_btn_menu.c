/*
 * digi_clock_btn_menu.c
 *
 *  Created on: 16.08.2020
 *      Author: andi
 */

#include <stdbool.h>
#include <stdint.h>
#include <msp430.h>
#include <msp430f5529.h>
#include "digi_clock.h"
#include "digi_clock_btn_menu.h"

void btn_to_event(void)
{
    extern volatile uint8_t btn_is_int_set;
    extern uint8_t btn_in_work;
    extern uint16_t btn_counter[6];
    extern uint8_t btn_event[6];
    uint8_t invert_BUTTON_IN;
    /*
     * the order of btn_in_work and btn_is_int_set is imported
     * it released the debouncing of buttons
     */
    if (btn_in_work)
    {
        invert_BUTTON_IN = ~ BUTTON_IN;
        if (btn_in_work & BUTTON_1)
        {
            if (invert_BUTTON_IN & BUTTON_1)
            {
                btn_counter[0]++;
                if ((!(btn_event[0] & BTN_PRESS_INTER_FIRST)) && (btn_counter[0] >= 20))
                {
                    btn_event[0] |= BTN_PRESS_INTER_FIRST | BTN_PRESS_INTER;
                }
                if ((btn_event[0] & BTN_PRESS_INTER_FIRST) && (!(btn_counter[0] & 7)))
                btn_event[0] |= BTN_PRESS_INTER;
            }
            else // Push button released
            {
                if (btn_counter[0] > 1)
                {
                    if ((btn_counter[0] > 3) & (btn_counter[0] < 15))
                    {
                        btn_event[0] |= BTN_PRESS_SHORT;
                    }
                    if ((btn_counter[0] >= 15) & (btn_counter[0] < 300))
                    {
                        btn_event[0] |= BTN_PRESS_LONG;
                    }
                    btn_counter[0] = 0;
                    btn_in_work &= ~BUTTON_1;
                    btn_event[0] &= ~BTN_PRESS_INTER_FIRST;
                }
            }
        }
    }
    if (btn_is_int_set)
    {
        if (btn_is_int_set & BUTTON_1)
        {
            if (!(btn_in_work & BUTTON_1)) btn_in_work |= BUTTON_1;
            btn_is_int_set &= ~BUTTON_1;
        }
        if (btn_is_int_set & BUTTON_2)
        {
            btn_in_work |= BUTTON_2;
            btn_is_int_set &= ~BUTTON_2;
        }
        if (btn_is_int_set & BUTTON_3)
        {
            btn_in_work |= BUTTON_3;
            btn_is_int_set &= ~BUTTON_3;
        }
        if (btn_is_int_set & BUTTON_4)
        {
            btn_in_work |= BUTTON_4;
            btn_is_int_set &= ~BUTTON_4;
        }
        if (btn_is_int_set & BUTTON_5)
        {
            btn_in_work |= BUTTON_5;
            btn_is_int_set &= ~BUTTON_5;
        }
        if (btn_is_int_set & BUTTON_6)
        {
            btn_in_work |= BUTTON_6;
            btn_is_int_set &= ~BUTTON_6;
        }

    }
}

/*
 * Button-events
 */
void clock_event_to_menue(void)
{
    extern uint8_t btn_event[6];

    if (btn_event[0] & BTN_PRESS_SHORT)
    {
//               SIGNALS_OUT ^= LED_OSCI_FAULT;
        btn_event[0] &= ~BTN_PRESS_SHORT;
    }
    if (btn_event[0] & BTN_PRESS_LONG)
    {
//               SIGNALS_OUT ^= LED_OSCI_FAULT;
        btn_event[0] &= ~BTN_PRESS_LONG;
    }
    if (btn_event[0] & BTN_PRESS_INTER)
    {
        SIGNALS_OUT ^= LED_OSCI_FAULT;
        btn_event[0] &= ~BTN_PRESS_INTER;
    } //          SIGNALS_OUT ^= LED_SEC;              // toggle P4.0
}
