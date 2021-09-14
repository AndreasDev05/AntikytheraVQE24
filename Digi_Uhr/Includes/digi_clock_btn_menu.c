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
#include <digi_clock_alarm_fkt.h>


void btn_to_event(void)
{
    extern volatile uint8_t btn_is_int_set;
    extern uint8_t btn_in_work;
    extern uint16_t btn_counter[6];
    extern uint8_t btn_event[6], btn_adaptation[];
    extern uint8_t alarm_start_flags;
    uint8_t invert_BUTTON_IN,btn_i,btn_j;
    /*
     * the order of btn_in_work and btn_is_int_set is imported
     * it released the debouncing of buttons
     */
    if (btn_in_work)
    {
// TODO Bei der FOR-Schleife muss mal die Performance gemessen werden
        invert_BUTTON_IN = ~ BUTTON_IN;
        btn_j = BUTTON_last;
        btn_j <<= 1;
        btn_i = 6;
//        for (btn_i=0; btn_i <= 5; btn_i++)
        do
        {
            btn_j >>= 1;
            btn_i--;
            if (btn_in_work & btn_j)
            {
                if (invert_BUTTON_IN & btn_adaptation[btn_i])
                {
                    btn_counter[btn_i]++;
//                    if (btn_counter[btn_i] == 15) alarm_start_flags |= ALARM_BEEP;
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
                        if ((btn_counter[btn_i] >= 2) && (btn_counter[btn_i] < 15))
                        {
                            btn_event[btn_i] |= BTN_PRESS_SHORT;
                        }
                        if ((btn_counter[btn_i] >= 15) && (btn_counter[btn_i] < 300))
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
        if (btn_is_int_set & BUTTON_down)
        {
            if (!(btn_in_work & BUTTON_down)) btn_in_work |= BUTTON_down;
            btn_is_int_set &= ~BUTTON_down;
        }
        if (btn_is_int_set & BUTTON_up)
        {
            if (!(btn_in_work & BUTTON_up)) btn_in_work |= BUTTON_up;
            btn_is_int_set &= ~BUTTON_up;
        }
        if (btn_is_int_set & BUTTON_left)
        {
            if (!(btn_in_work & BUTTON_left)) btn_in_work |= BUTTON_left;
            btn_is_int_set &= ~BUTTON_left;
        }
        if (btn_is_int_set & BUTTON_right)
        {
            if (!(btn_in_work & BUTTON_right)) btn_in_work |= BUTTON_right;
            btn_is_int_set &= ~BUTTON_right;
        }
        if (btn_is_int_set & BUTTON_mode)
        {
            if (!(btn_in_work & BUTTON_mode)) btn_in_work |= BUTTON_mode;
            btn_is_int_set &= ~BUTTON_mode;
        }
        if (btn_is_int_set & BUTTON_alarm)
        {
            if (!(btn_in_work & BUTTON_alarm)) btn_in_work |= BUTTON_alarm;
            btn_is_int_set &= ~BUTTON_alarm;
        }

    }
}

/*
 * Button-events
 */
void clock_event_to_menu(uint8_t menu_stop_flag)
{
    extern uint8_t btn_event[6];
    extern uint8_t alarm_start_flags;
    extern uint8_t menu_stop_timer;
    extern enum EVE_con_typ eve_condition;

    static enum MENU_layer menu_layer = time_date;
    static enum MENU_sub_layer menu_sub_layer = menu_sub_level_0;

    if (menu_stop_flag == MENU_STOP_FLAG)
    {
        menu_layer = time_date;
        menu_sub_layer = menu_sub_level_0;
        eve_condition = normal;
    }
    else
        menu_stop_timer = MENU_STOP_COUNT;

    if (btn_event[0])  // down
    {
        if (btn_event[0] & BTN_PRESS_SHORT)
        {
            switch (menu_layer)
            {
            case time_date:
                switch (menu_sub_layer)
                {
                case menu_sub_level_0:
                    menu_layer = alarm_times;
                    eve_condition = view_alarm_time1;
                    break;
                }
                break;
            }
            btn_event[0] &= ~BTN_PRESS_SHORT;
        }
        if (btn_event[0] & BTN_PRESS_LONG)
        {
            btn_event[0] &= ~BTN_PRESS_LONG;
        }
        if (btn_event[0] & BTN_PRESS_INTER)
        {
            btn_event[0] &= ~BTN_PRESS_INTER;
        }
    }
// button 2 events
    if (btn_event[1])  // Up??
    {
        if (btn_event[1] & BTN_PRESS_SHORT)
        {
            switch (menu_layer)
            {
            case time_date:
                switch (menu_sub_layer)
                {
                case menu_sub_level_0:
                    menu_layer = measured_data;
                    eve_condition = view_temp_out;
                    break;
                }
                break;
                case alarm_times:
                    switch (menu_sub_layer)
                    {
                    case menu_sub_level_0:
                        menu_layer = time_date;
                        eve_condition = normal;
                        break;
                    }
                    break;
            }
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
    if (btn_event[2])  // left
    {
        if (btn_event[2] & BTN_PRESS_SHORT)
        {
            switch (menu_layer)
            {
            case time_date:
                switch (menu_sub_layer)
                {
                case menu_sub_level_0:
                    menu_sub_layer = menu_sub_level_m1;
                    eve_condition = view_sec_and_min;
                    break;
                case menu_sub_level_1:
                    menu_sub_layer = menu_sub_level_0;
                    eve_condition = normal;
                    break;
                case menu_sub_level_2:
                    menu_sub_layer = menu_sub_level_1;
                    eve_condition = view_day_of_week;
                    break;
                case menu_sub_level_3:
                    menu_sub_layer = menu_sub_level_2;
                    eve_condition = view_day_and_month;
                    break;
                }
                break;
                case measured_data:
                    switch (menu_sub_layer)
                    {
                    case menu_sub_level_1:
                        menu_sub_layer = menu_sub_level_0;
                        eve_condition = view_temp_out;
                        break;
                    case menu_sub_level_2:
                        menu_sub_layer = menu_sub_level_1;
                        eve_condition = view_temp_cpu;
                        break;
                    case menu_sub_level_3:
                        menu_sub_layer = menu_sub_level_2;
                        eve_condition = view_batt;
                        break;
                    case menu_sub_level_4:
                        menu_sub_layer = menu_sub_level_3;
                        eve_condition = view_bright;
                        break;
                    }
                 break;
            }
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
    if (btn_event[3]) // right
    {
        if (btn_event[3] & BTN_PRESS_SHORT)
        {
            switch (menu_layer)
            {
            case time_date:
                switch (menu_sub_layer)
                {
                case menu_sub_level_m1:
                    menu_sub_layer = menu_sub_level_0;
                    eve_condition = normal;
                    break;
                case menu_sub_level_0:
                    menu_sub_layer = menu_sub_level_1;
                    eve_condition = view_day_of_week;
                    break;
                case menu_sub_level_1:
                    menu_sub_layer = menu_sub_level_2;
                    eve_condition = view_day_and_month;
                    break;
                case menu_sub_level_2:
                    menu_sub_layer = menu_sub_level_3;
                    eve_condition = view_year;
                    break;
                }
                break;
                case measured_data:
                    switch (menu_sub_layer)
                    {
                    case menu_sub_level_0:
                        menu_sub_layer = menu_sub_level_1;
                        eve_condition = view_temp_cpu;
                        break;
                    case menu_sub_level_1:
                        menu_sub_layer = menu_sub_level_2;
                        eve_condition = view_batt;
                        break;
                    case menu_sub_level_2:
                        menu_sub_layer = menu_sub_level_3;
                        eve_condition = view_bright;
                        break;
                    case menu_sub_level_3:
                        menu_sub_layer = menu_sub_level_4;
                        eve_condition = view_rtc_corr;
                        break;
                    }
                    break;
            }
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
    if (btn_event[4]) // mode
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
    if (btn_event[5]) // alarm
    {
        if (btn_event[5] & BTN_PRESS_SHORT)
        {
            SIGNALS_OUT ^= LED_OSCI_FAULT;
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
