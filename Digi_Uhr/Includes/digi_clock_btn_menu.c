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
    uint8_t invert_BUTTON_IN,btn_i,btn_j;
    /*
     * the order of btn_in_work and btn_is_int_set is imported
     * it released the debouncing of buttons
     */
    if (btn_in_work)
    {
// TODO Bei der FOR-Schleife muss mal die Performance gemessen werden
        invert_BUTTON_IN = ~ BUTTON_IN;
        btn_j = BUTTON_6;
        btn_j <<= 1;
        btn_i = 6;
//        for (btn_i=0; btn_i <= 5; btn_i++)
        do
        {
            btn_j >>= 1;
            btn_i--;
            if (btn_in_work & btn_j)
            {
                if (invert_BUTTON_IN & btn_j)
                {
                    btn_counter[btn_i]++;
                    if ((!(btn_event[btn_i] & BTN_PRESS_INTER_FIRST)) && (btn_counter[btn_i] >= 20))
                    {
                        btn_event[btn_i] |= BTN_PRESS_INTER_FIRST | BTN_PRESS_INTER;
                    }
                    if ((btn_event[btn_i] & BTN_PRESS_INTER_FIRST) && (!(btn_counter[btn_i] & 7)))
                    btn_event[btn_i] |= BTN_PRESS_INTER;
                }
                else // Push button released
                {
                    if (btn_counter[btn_i] > 1)
                    {
                        if ((btn_counter[btn_i] > 3) & (btn_counter[btn_i] < 15))
                        {
                            btn_event[btn_i] |= BTN_PRESS_SHORT;
                        }
                        if ((btn_counter[btn_i] >= 15) & (btn_counter[btn_i] < 300))
                        {
                            btn_event[btn_i] |= BTN_PRESS_LONG;
                        }
                        btn_counter[btn_i] = 0;
                        btn_in_work &= ~btn_j;
                        btn_event[btn_i] &= ~BTN_PRESS_INTER_FIRST;
                    }
                }
            }
        }
        while (btn_i > 0);
    }

/*
 * the order of btn_in_work and btn_is_int_set is imported
 * it released the debouncing of buttons
 */
    if (btn_is_int_set)
    {
        if (btn_is_int_set & BUTTON_1)
        {
            if (!(btn_in_work & BUTTON_1)) btn_in_work |= BUTTON_1;
            btn_is_int_set &= ~BUTTON_1;
        }
        if (btn_is_int_set & BUTTON_2)
        {
            if (!(btn_in_work & BUTTON_2)) btn_in_work |= BUTTON_2;
            btn_is_int_set &= ~BUTTON_2;
        }
        if (btn_is_int_set & BUTTON_3)
        {
            if (!(btn_in_work & BUTTON_3)) btn_in_work |= BUTTON_3;
            btn_is_int_set &= ~BUTTON_3;
        }
        if (btn_is_int_set & BUTTON_4)
        {
            if (!(btn_in_work & BUTTON_4)) btn_in_work |= BUTTON_4;
            btn_is_int_set &= ~BUTTON_4;
        }
        if (btn_is_int_set & BUTTON_5)
        {
            if (!(btn_in_work & BUTTON_5)) btn_in_work |= BUTTON_5;
            btn_is_int_set &= ~BUTTON_5;
        }
        if (btn_is_int_set & BUTTON_6)
        {
            if (!(btn_in_work & BUTTON_6)) btn_in_work |= BUTTON_6;
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

    if (btn_event[0])
    {
        if (btn_event[0] & BTN_PRESS_SHORT)
        {
            btn_event[0] &= ~BTN_PRESS_SHORT;
        }
        if (btn_event[0] & BTN_PRESS_LONG)
        {
            btn_event[0] &= ~BTN_PRESS_LONG;
        }
        if (btn_event[0] & BTN_PRESS_INTER)
        {
            SIGNALS_OUT ^= LED_OSCI_FAULT;
            btn_event[0] &= ~BTN_PRESS_INTER;
        }
    }
// button 2 events
    if (btn_event[1])
    {
        if (btn_event[1] & BTN_PRESS_SHORT)
        {
            btn_event[1] &= ~BTN_PRESS_SHORT;
        }
        if (btn_event[1] & BTN_PRESS_LONG)
        {
            btn_event[1] &= ~BTN_PRESS_LONG;
        }
        if (btn_event[1] & BTN_PRESS_INTER)
        {
            btn_event[1] &= ~BTN_PRESS_INTER;
        }
    }
// button 3 events
    if (btn_event[2])
    {
        if (btn_event[2] & BTN_PRESS_SHORT)
        {
            btn_event[2] &= ~BTN_PRESS_SHORT;
        }
        if (btn_event[2] & BTN_PRESS_LONG)
        {
            btn_event[2] &= ~BTN_PRESS_LONG;
        }
        if (btn_event[2] & BTN_PRESS_INTER)
        {
            btn_event[2] &= ~BTN_PRESS_INTER;
        }
    }
// button 4 events
    if (btn_event[3])
    {
        if (btn_event[3] & BTN_PRESS_SHORT)
        {
            btn_event[3] &= ~BTN_PRESS_SHORT;
        }
        if (btn_event[3] & BTN_PRESS_LONG)
        {
            btn_event[3] &= ~BTN_PRESS_LONG;
        }
        if (btn_event[3] & BTN_PRESS_INTER)
        {
            btn_event[3] &= ~BTN_PRESS_INTER;
        }
    }
// button 5 events
    if (btn_event[4])
    {
        if (btn_event[4] & BTN_PRESS_SHORT)
        {
            btn_event[4] &= ~BTN_PRESS_SHORT;
        }
        if (btn_event[4] & BTN_PRESS_LONG)
        {
            btn_event[4] &= ~BTN_PRESS_LONG;
        }
        if (btn_event[4] & BTN_PRESS_INTER)
        {
            btn_event[4] &= ~BTN_PRESS_INTER;
        }
    }
// button 6 events
    if (btn_event[5])
    {
        if (btn_event[5] & BTN_PRESS_SHORT)
        {
            btn_event[5] &= ~BTN_PRESS_SHORT;
        }
        if (btn_event[5] & BTN_PRESS_LONG)
        {
            btn_event[5] &= ~BTN_PRESS_LONG;
        }
        if (btn_event[5] & BTN_PRESS_INTER)
        {
            btn_event[5] &= ~BTN_PRESS_INTER;
        }
    }
}
