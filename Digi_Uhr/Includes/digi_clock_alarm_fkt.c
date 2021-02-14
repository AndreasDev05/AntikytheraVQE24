/*
 * digi_clock_alarm_fkt.c
 *
 *  Created on: 21.01.2021
 *      Author: andi
 */

#include <stdbool.h>
#include <stdint.h>
#include <digi_clock.h>


void AlarmBeep(void)
{
    extern  uint8_t     alarm_start_flags;
    static  uint8_t     beep_status = 0;

    if ((alarm_start_flags & ALARM_BEEP) && (beep_status == 0))
    {
        CONTROL_OUT &= ~AL1;
        beep_status = 1;
    }
    else
    {
        CONTROL_OUT |= AL1;
        beep_status = 0;
        alarm_start_flags &= ~ALARM_BEEP;
    }
}

void AlarmBell(void)
// this fkt. sets in alarm_start_flags ALARM_STOP-bit
{
    extern uint8_t     alarm_start_flags;
    extern uint8_t     alarm_intvT[];
    static uint8_t     alarm_melody_offset, alarm_melody_point, alarm_status_flag = 0;
    static uint8_t     alarm_onoff_count, alarm_length_count;

    if (alarm_status_flag)
    {
        if (alarm_onoff_count == 0)
        {
            if (alarm_length_count & BIT0)
                BEEPER_ON;
            else
                BEEPER_OFF;
            alarm_length_count--;
            if (alarm_length_count != 0)
            {
                alarm_melody_point--;
                alarm_onoff_count = alarm_intvT[alarm_melody_point];
            }
            else
            {
                BEEPER_OFF;
                alarm_status_flag = 0;
            }
        }
        else
        {
            alarm_onoff_count--;
        }
    }
    else // if alarm_status_flag
    {
        alarm_status_flag = 1;
        switch (alarm_start_flags)
        {
        case ALARM_LEV01:
            alarm_melody_offset = alarm_intvT[0];
            break;
        case ALARM_LEV02:
            alarm_melody_offset = alarm_intvT[1];
            break;
        case ALARM_LEV03:
            alarm_melody_offset = alarm_intvT[2];
            break;
        }  //switch
        alarm_length_count = alarm_intvT[alarm_melody_offset];
        alarm_melody_point = alarm_melody_offset + alarm_length_count;
        alarm_onoff_count = alarm_intvT[alarm_melody_point];
        BEEPER_ON;
        alarm_onoff_count--;
    } // if (alarm_status_flag)
    if (alarm_start_flags & ALARM_STOP)
    {
        alarm_status_flag = 0;
        BEEPER_OFF;
        alarm_start_flags &= ~ALARM_STOP;
    }
    /*
     if ((my_alarm_flags.alarm_beep == 0)&((alarm_cycle_count & 1) == 0))    // Signalgeber einschalten
     {
     my_alarm_flags.alarm_beep = 1;
     P2OUT |= BIT4;
     }
     if ((my_alarm_flags.alarm_beep == 1)&((alarm_cycle_count & 1) != 0))    // Signalgeber ausschalten
     {
     my_alarm_flags.alarm_beep = 0;
     P2OUT &= ~BIT4;
     }

     if (alarm_intv_count < alarm_intv[alarm_cycle_count])
     ++alarm_intv_count; // Toggelt und zählt die Alarmintervalzeit hoch
     else    // wird der Alarmzykluszähler erhöht
     {
     alarm_intv_count = 0;
     if (alarm_cycle_count < alarm_int_step_size[alarm_mode+1])
     ++alarm_cycle_count;
     else alarm_cycle_count = alarm_int_step_size[alarm_mode]+1;
     } */
}

