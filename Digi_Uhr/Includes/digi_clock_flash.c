/*
 * digi_clock_flash.c
 *
 *  Created on: 19.02.2021
 *      Author: andi
 */

#include <stdbool.h>
#include <stdint.h>
#include <msp430.h>
#include <msp430f5529.h>
#include <digi_clock.h>
#include <digi_clock_flash.h>


void FlashWrite(void)
{
    char *Flash_ptrA;
    char *Flash_ptrD;
    int8_t pointer_i;

    Flash_ptrA = (char *) 0x1980;             // Initialize Flash segment A ptr
    Flash_ptrD = (char *) 0x1800;             // Initialize Flash segment D ptr


    if (*Flash_ptrA == 0x0FF)  // AND flash_which_data == write_ver // First Flash-write
    {
        FCTL3 = FWKEY;                             // Clear Lock bit
        FCTL1 = FWKEY | WRT;

        FlashAtomWrite();

        FCTL1 = FWKEY;                            // Clear Erase bit for write operation
        FCTL3 = FWKEY | LOCK;                     // Set Lock bit
    }
    else
    {
        FCTL3 = FWKEY;                             // Clear Lock bit
        FCTL1 = FWKEY | WRT;
        for (pointer_i = 127; pointer_i >= 0; pointer_i--)                // Save Flash
        {
            *(Flash_ptrD + pointer_i) = *(Flash_ptrA + pointer_i);
        }
        __disable_interrupt();                     // 5xx Workaround: Disable global
                                                   // interrupt while erasing. Re-Enable
                                                   // GIE if needed

        DISPLAY_OUT = 0x0F ;                       // switch the display off; I don't know
                                                   // how long it takes to write / delete
                                                   // the flash memory?
        FCTL1 = FWKEY | ERASE;                     // Set Erase bit
        *Flash_ptrA = 0;                           // Dummy write to erase Flash seg D
        FCTL1 = FWKEY | WRT;                       // Clear ERASE bit

        FlashAtomWrite();

//        FCTL3 = FWKEY;                            // Clear Lock bit
        FCTL1 = FWKEY | ERASE;                    // Set Erase bit
        *Flash_ptrD = 0;                          // Dummy write to erase Flash seg D
        FCTL1 = FWKEY;                            // Clear Erase bit for write operation
        FCTL3 = FWKEY | LOCK;                     // Set Lock bit

        __enable_interrupt();

    }

}

void FlashDeleteBlock(void)
{

}

// Imported: the called function must set FCTL1 and FCTL3 !!!!

void FlashAtomWrite(void)
{
    char *Flash_ptrA;
    char *Flash_ptrD;
    extern enum FLASH_write_data flash_which_data;
    extern struct alarm_set alarm_data[5];
    uint16_t pointer_i;

    Flash_ptrA = (char *) 0x1980;             // Initialize Flash segment A ptr
    Flash_ptrD = (char *) 0x1800;             // Initialize Flash segment D ptr

    switch (flash_which_data)
    {
    case write_date:
        *Flash_ptrA = *Flash_ptrD;
         for (pointer_i = RTC_CORRETION; pointer_i <= 127; pointer_i++)
         {
             *(Flash_ptrA + pointer_i) = *(Flash_ptrD + pointer_i);
         }
         *(Flash_ptrA + YEAR_H) = RTCYEARH;
         *(Flash_ptrA + YEAR_L) = RTCYEARL;
         *(Flash_ptrA + MOUNTS) = RTCMON;
         *(Flash_ptrA + FLASH_DAY) = RTCDAY;
        break;
    case write_ALARM1:
        for (pointer_i = FLASH_VER; pointer_i <= RTC_CORRETION; pointer_i++)
        {
            *(Flash_ptrA + pointer_i) = *(Flash_ptrD + pointer_i);
        }
        *(Flash_ptrA + ALARM1_DAYS) = alarm_data[0].alarm_days;
        *(Flash_ptrA + ALARM1_MIN) = alarm_data[0].alarm_min;
        *(Flash_ptrA + ALARM1_HOUR) = alarm_data[0].alarm_hour;
        *(Flash_ptrA + ALARM1_DAY) = alarm_data[0].alarm_day;
        *(Flash_ptrA + ALARM1_MOUNTH) = alarm_data[0].alarm_mounth;
        for (pointer_i = ALARM2_DAYS; pointer_i <= 127; pointer_i++)
        {
            *(Flash_ptrA + pointer_i) = *(Flash_ptrD + pointer_i);
        }
        break;
    case write_ALARM2:
        for (pointer_i = FLASH_VER; pointer_i <= ALARM1_MOUNTH; pointer_i++)
        {
            *(Flash_ptrA + pointer_i) = *(Flash_ptrD + pointer_i);
        }
        *(Flash_ptrA + ALARM2_DAYS) = alarm_data[1].alarm_days;
        *(Flash_ptrA + ALARM2_MIN) = alarm_data[1].alarm_min;
        *(Flash_ptrA + ALARM2_HOUR) = alarm_data[1].alarm_hour;
        *(Flash_ptrA + ALARM2_DAY) = alarm_data[1].alarm_day;
        *(Flash_ptrA + ALARM2_MOUNTH) = alarm_data[1].alarm_mounth;
        for (pointer_i = ALARM3_DAYS; pointer_i <= 127; pointer_i++)
        {
            *(Flash_ptrA + pointer_i) = *(Flash_ptrD + pointer_i);
        }
        break;
    case write_ALARM3:
        for (pointer_i = FLASH_VER; pointer_i <= ALARM2_MOUNTH; pointer_i++)
        {
            *(Flash_ptrA + pointer_i) = *(Flash_ptrD + pointer_i);
        }
        *(Flash_ptrA + ALARM3_DAYS) = alarm_data[2].alarm_days;
        *(Flash_ptrA + ALARM3_MIN) = alarm_data[2].alarm_min;
        *(Flash_ptrA + ALARM3_HOUR) = alarm_data[2].alarm_hour;
        *(Flash_ptrA + ALARM3_DAY) = alarm_data[2].alarm_day;
        *(Flash_ptrA + ALARM3_MOUNTH) = alarm_data[2].alarm_mounth;
        for (pointer_i = ALARM4_DAYS; pointer_i <= 127; pointer_i++)
        {
            *(Flash_ptrA + pointer_i) = *(Flash_ptrD + pointer_i);
        }
        break;
    case write_ALARM4:
        for (pointer_i = FLASH_VER; pointer_i <= ALARM3_MOUNTH; pointer_i++)
        {
            *(Flash_ptrA + pointer_i) = *(Flash_ptrD + pointer_i);
        }
        *(Flash_ptrA + ALARM4_DAYS) = alarm_data[3].alarm_days;
        *(Flash_ptrA + ALARM4_MIN) = alarm_data[3].alarm_min;
        *(Flash_ptrA + ALARM4_HOUR) = alarm_data[3].alarm_hour;
        *(Flash_ptrA + ALARM4_DAY) = alarm_data[3].alarm_day;
        *(Flash_ptrA + ALARM4_MOUNTH) = alarm_data[3].alarm_mounth;
        for (pointer_i = ALARM5_DAYS; pointer_i <= 127; pointer_i++)
        {
            *(Flash_ptrA + pointer_i) = *(Flash_ptrD + pointer_i);
        }
        break;
    case write_ALARM5:
        for (pointer_i = FLASH_VER; pointer_i <= ALARM4_MOUNTH; pointer_i++)
        {
            *(Flash_ptrA + pointer_i) = *(Flash_ptrD + pointer_i);
        }
        *(Flash_ptrA + ALARM5_DAYS) = alarm_data[4].alarm_days;
        *(Flash_ptrA + ALARM5_MIN) = alarm_data[4].alarm_min;
        *(Flash_ptrA + ALARM5_HOUR) = alarm_data[4].alarm_hour;
        *(Flash_ptrA + ALARM5_DAY) = alarm_data[4].alarm_day;
        *(Flash_ptrA + ALARM5_MOUNTH) = alarm_data[4].alarm_mounth;
        for (pointer_i = TEMP_CORR_0; pointer_i <= 127; pointer_i++)
        {
            *(Flash_ptrA + pointer_i) = *(Flash_ptrD + pointer_i);
        }
        break;
    case write_RTC_correction:
        for (pointer_i = FLASH_VER; pointer_i <= FLASH_DAY; pointer_i++)
        {
            *(Flash_ptrA + pointer_i) = *(Flash_ptrD + pointer_i);
        }
        *(Flash_ptrA + RTC_CORRETION) = alarm_data[4].alarm_days;  // TODO: right variable
        for (pointer_i = ALARM1_DAYS; pointer_i <= 127; pointer_i++)
        {
            *(Flash_ptrA + pointer_i) = *(Flash_ptrD + pointer_i);
        }
        break;
    case write_TEMP_correction:
        for (pointer_i = FLASH_VER; pointer_i <= ALARM5_MOUNTH; pointer_i++)
        {
            *(Flash_ptrA + pointer_i) = *(Flash_ptrD + pointer_i);
        }
        *(Flash_ptrA + TEMP_CORR_0) = alarm_data[4].alarm_days;   // TODO: right variable
        *(Flash_ptrA + TEMP_CORR_80) = alarm_data[4].alarm_days;   // TODO: right variable
        for (pointer_i = BATT_CORR_1400; pointer_i <= 127; pointer_i++)
        {
            *(Flash_ptrA + pointer_i) = *(Flash_ptrD + pointer_i);
        }
        break;
    case write_BATT_correction:
        for (pointer_i = FLASH_VER; pointer_i <= TEMP_CORR_80; pointer_i++)
        {
            *(Flash_ptrA + pointer_i) = *(Flash_ptrD + pointer_i);
        }
        *(Flash_ptrA + BATT_CORR_1400) = alarm_data[4].alarm_days;  // TODO: right variable
        *(Flash_ptrA + BATT_CORR_3600) = alarm_data[4].alarm_days;  // TODO: right variable
        /*
            in future, write other variables, not only 0xFF ;-)
        */
        break;
    case write_ver:
        *Flash_ptrA = FLASH_DATA_VER;
        for (pointer_i = 1; pointer_i <= 127; pointer_i++)
        {
            *(Flash_ptrA + pointer_i) = *(Flash_ptrD + pointer_i);
        }
        break;
    }
}

void InitialVarWithFlash(void)
{
    extern enum FLASH_write_data flash_which_data;
    extern struct alarm_set alarm_data[5];

    char *Flash_ptrA;
    Flash_ptrA = (char *) 0x1980;             // Initialize Flash segment A ptr

    /*

#define     FLASH_DATA_VER  0

#define     FLASH_VER       0
#define     YEAR_H          1
#define     YEAR_L          2
#define     MOUNTS          3
#define     FLASH_DAY       4
#define     RTC_CORRETION   5
#define     ALARM1_DAYS     7
#define     ALARM1_MIN      8
#define     ALARM1_HOUR     9
#define     ALARM1_DAY      10
#define     ALARM1_MOUNTH   11
#define     ALARM2_DAYS     12
#define     ALARM2_MIN      13
#define     ALARM2_HOUR     14
#define     ALARM2_DAY      15
#define     ALARM2_MOUNTH   16
#define     ALARM3_DAYS     17
#define     ALARM3_MIN      18
#define     ALARM3_HOUR     19
#define     ALARM3_DAY      20
#define     ALARM3_MOUNTH   21
#define     ALARM4_DAYS     22
#define     ALARM4_MIN      23
#define     ALARM4_HOUR     24
#define     ALARM4_DAY      25
#define     ALARM4_MOUNTH   26
#define     ALARM5_DAYS     27
#define     ALARM5_MIN      28
#define     ALARM5_HOUR     29
#define     ALARM5_DAY      30
#define     ALARM5_MOUNTH   31
#define     TEMP_CORR_0     33
#define     TEMP_CORR_80    35
#define     BATT_CORR_1400  37
#define     BATT_CORR_3600  39
     */
    if (*(Flash_ptrA + FLASH_VER) == FLASH_DATA_VER)
    {

    }
    else // is flash_ver correct, else initial flash or (in the future) correct flashstructure
    {
        flash_which_data = write_ver;
        FlashWrite();

    }
}
