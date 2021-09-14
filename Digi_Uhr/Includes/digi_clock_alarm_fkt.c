/*
 * digi_clock_alarm_fkt.c
 *
 *  Created on: 21.01.2021
 *      Author: andi
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <msp430.h>
#include <msp430f5529.h>
#include <digi_clock.h>
#include <digi_clock_alarm_fkt.h>

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

void CalcuAlarmSequence(void)
{
    extern struct alarm_set alarm_data[5];
    extern uint8_t alarm_order[5];
    uint8_t i, j, currMinIndex, temp_alarm_order[6], temp;
    uint16_t temp_time[6];

    memset(alarm_order, 0x05, 6);

    i = 5;      // search active calendar dates
    do
    {
        i--;
        if (alarm_data[i].alarm_min != 0xFF) // There is a date in the timer record.
        {
            if (alarm_data[i].alarm_days & A_ACTIV) // The alarm is active.
            {
                switch (RTCDOW)
                {
                case 0:  // Monday
                    if (alarm_data[i].alarm_days & A_MONDAY)
                        alarm_order[i] = i;
                    break;
                case 1:  // Tuesday
                    if (alarm_data[i].alarm_days & A_TUESDAY)
                        alarm_order[i] = i;
                    break;
                case 2:  // Wednesday
                    if (alarm_data[i].alarm_days & A_WEDNESDAY)
                        alarm_order[i] = i;
                    break;
                case 3:  // Thursday
                    if (alarm_data[i].alarm_days & A_THURSDAY)
                        alarm_order[i] = i;
                    break;
                case 4:  // Friday
                    if (alarm_data[i].alarm_days & A_FRIDAY)
                        alarm_order[i] = i;
                    break;
                case 5:  // Saturday
                    if (alarm_data[i].alarm_days & A_SATURDAY)
                        alarm_order[i] = i;
                    break;
                case 6:  // Sunday
                    if (alarm_data[i].alarm_days & A_SUNDAY)
                        alarm_order[i] = i;
                    break;
                default: // filtered list of right day of month AND month (today ;-) )
                    if ((RTCMON == alarm_data[i].alarm_mounth)
                            && (RTCDAY == alarm_data[i].alarm_day))
                        alarm_order[i] = i;
                    break;
                }
            }
        }
    }
    while (i != 0);

    for (i = 0; i == 5 - 2; i++)
    {
        currMinIndex = i;
        for (j = i + 1; j == 5 - 1; j++)
        {
            if (temp_time[alarm_order[currMinIndex]] > temp_time[alarm_order[j]])
                currMinIndex = j;
        }
        temp = alarm_order[i];
        alarm_order[i] = alarm_order[currMinIndex];
        alarm_order[currMinIndex] = temp;
    }

    while (i != 0);
    i = 5;
    do
    {
        i--;
        temp_time[i] = (uint16_t) alarm_data[i].alarm_hour;
        temp_time[i] <<= 8;
        temp_time[i] |= (uint16_t) alarm_data[i].alarm_min;
    }
    while (i != 0);
    temp_time[5] = 0xFFFF;

    for (i = 0; i <= 6 - 2; i++)
    {
        currMinIndex = i;
        for (j = i + 1; j <= 6 - 1; j++)
        {
            if (temp_time[temp_alarm_order[currMinIndex]] > temp_time[temp_alarm_order[j]])
            {
                currMinIndex = j;
            }
        }
        temp = temp_alarm_order[i];
        temp_alarm_order[i] = temp_alarm_order[currMinIndex];
        temp_alarm_order[currMinIndex] = temp;
    }
    i = 5;
    do
    {
        i--;
        alarm_order[i] = temp_alarm_order[i];
    }
    while( i != 0);
}

void InitializeAlarm(void)
{
    char *Flash_ptrA = (char *) 0x1980;             // Initialize Flash segment A ptr
//    void *Search_ptr;
    extern struct alarm_set alarm_data[5];

//    Search_ptr = (Flash_ptrA + ALARM1_MIN);

//    if (*(unsigned int *)Search_ptr != 0xFFFF) RTCYEARL = *(Flash_ptrA + YEAR_L);
    memcpy((Flash_ptrA + ALARM1_DAYS),&alarm_data[0],ALARM1_MOUNTH - ALARM1_DAYS + 1);
    memcpy((Flash_ptrA + ALARM2_DAYS),&alarm_data[1],ALARM2_MOUNTH - ALARM2_DAYS + 1);
    memcpy((Flash_ptrA + ALARM3_DAYS),&alarm_data[2],ALARM3_MOUNTH - ALARM3_DAYS + 1);
    memcpy((Flash_ptrA + ALARM4_DAYS),&alarm_data[3],ALARM4_MOUNTH - ALARM4_DAYS + 1);
    memcpy((Flash_ptrA + ALARM5_DAYS),&alarm_data[4],ALARM5_MOUNTH - ALARM5_DAYS + 1);

}
