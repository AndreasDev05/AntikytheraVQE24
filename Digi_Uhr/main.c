#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <msp430.h>
#include <msp430f5529.h>
// !!!! Hardwareversion of PCB
#define HW_Ver1_0
//
#include <digi_clock.h>
#include <digi_clock_isr.h>
#include <digi_clock_flash.h>
#include <digi_clock_btn_menu.h>
#include <digi_clock_alarm_fkt.h>
#include <digi_clock_fkt.h>


// Variables global clock management

// Variablen in den Interruptfkt. -----
    volatile bool is_pwr_good = true;
    volatile uint8_t is_sec,is_msec,is_msec2, is_100ms, is_100ms2, is_300ms = 0;
    uint8_t old_min, old_hour, old_day = 1;

    // Which value will be displayed
    volatile enum EVE_con_typ eve_condition = view_sec_and_min;

// Variables for display management
    // to fast find the pin-quartet on display-port
    unsigned const int disp_pos[2] = {0x8040,0x2010};
    // display-memory position and BCD-number
    volatile uint8_t disp_out[4] = {0x40,0x81,0x12,0x24};
    // display-memory decimal point and other LEDs
    volatile uint8_t disp_point = DIGT_PNT1, disp_point_blink;
    // for fast overlay-functions on the display-memory
    volatile uint16_t  *disp_out_int = disp_out;
    volatile void *disp_out_int_ptr = &disp_out;
    // to reduce flicker save the display in this memory
    volatile uint8_t disp_out_buf[4];
    volatile uint16_t  *disp_out_buf_int = &disp_out_buf;
    volatile void *disp_out_buf_int_ptr = &disp_out_buf;
    // pointer to the active display-memory
    volatile uint8_t *disp_out_point = disp_out;
    volatile uint8_t disp_count = 0;
    // the brightness of the display: it is recommended to use values between 10 and "TIME_PERIOD_DIGT"-10.
    volatile uint16_t  disp_brightness = 10;  // brightness

// Variables for button-logic
    volatile uint8_t btn_is_int_set;
    uint8_t    btn_in_work = 0;
    uint16_t   btn_counter[6] = {0,0,0,0,0,0};
    uint8_t    btn_event[6] = {0,0,0,0,0,0};
    void const *btn_event_int_ptr = (uint16_t *)&btn_event[0]; // later used 16 bit integer values are used to accelerate the comparison.
    const uint8_t btn_adaptation[] = {BUTTON_1,BUTTON_2,BUTTON_3,BUTTON_4,BUTTON_5,BUTTON_6};

    uint8_t menu_stop_timer = 0;

// Variables for alarm-logic
    uint8_t     alarm_start_flags = 0;
    const uint8_t alarm_intv1[] = {1,1,9};  // Stepcounter, ON-Inv, OFF-Inv, ... in 0,1sec
    const uint8_t alarm_intv2[] = {3,1,3,1,7};
    const uint8_t alarm_intv3[] = {3,1,1,1,5};
    const uint8_t alarm_intvT[] = {3,6,11,2,9,1,4,17,1,3,1,4,5,1,1,1};
/*                                 |      | | |
                                   |      | | Count of beeper ON in 100msec
                                   |      | Count of beeper OFF in 100msec
                                   |      Count of Bytes for the melodies (minus 1)
                                   Byte 1-3 Offset too melodies */
    struct alarm_set alarm_data[5];
    uint8_t alarm_order[5];

// Variables for FlashWriting
    enum FLASH_write_data flash_which_data;

// Variables for ADC12
    volatile uint16_t adc_out_raw[8];
    uint16_t adc_out_bright_contr, adc_out_bright_f_disp, adc_out_batt_f_contr,
        adc_out_batt_f_disp, adc_out_batt_f_disp_raw, adc_out_temp_cpu_f_disp_raw,
        adc_out_temp_out_f_disp_raw;
    int16_t adc_out_temp_cpu_f_disp, adc_out_temp_out_f_disp;
    volatile bool adc_conv_ready;
    uint8_t  adc_out_ready = 0; // Bitarray that signalizes witch ADC-channel is ready - named bits in the header
    uint8_t  adc_power_count = 0; // if i switch on the power for the analog circuit
    uint16_t temp_s_sum;
    int32_t  temp_cpu_coefficient, temp_out_coefficient, batt_coefficient; // precalculated factor for CPU-temperature measurement
    int32_t  calcu_extension;

/**
 * main.c
 */
int main(void)
{

    volatile uint8_t temp_byte = 0;

//    btn_event_int_ptr = (uint16_t *)&btn_event[0];

//////////// Initialize CPU function
    InitializeCPU();
// Initialize UCS
    Initialize_UCS_and_Crystals();
// Initialize the real time clock
    InitializeRTC();
// Initialize ADC12
    InitializeADC12();

// Initialize PortPins
    InitializePins();

// Initialize Timer A0 & A1
    InitializeTimerA();

// main loop

    while (1)
    {
        if (is_pwr_good)  // USB-supply is ready
        {
            if (is_sec != 0)
            {
                is_sec--;
//                SIGNALS_OUT    ^= DARK;                            // Toggle P7.3
//                CONTROL_OUT ^= TURNON_RELAY;
                switch (eve_condition)
                {
                case view_temp_out:
//                ADC_scheduler(measure_bright_f_contr);
                    ADC_scheduler(measure_temp_out_f_disp);
                    break;
                case view_temp_cpu:
                    ADC_scheduler(measure_temp_cpu_f_disp);
                    break;
                case view_batt:
                    ADC_scheduler(measure_batt_f_disp);
                    break;
                case view_bright:
                    ADC_scheduler(measure_bright_f_disp);
                    break;
                }
                if (RTCMIN != old_min)
                {
                    old_min = RTCMIN;
//                    alarm_start_flags |= ALARM_BEEP;
/*                    if (RTCMIN == 0x23) alarm_start_flags |= ALARM_LEV02;
                    if (RTCMIN == 0x24)
                    {
                        alarm_start_flags &= ~ALARM_LEV02;
                        alarm_start_flags |= ALARM_STOP;
                    }   */
                    if (RTCHOUR != old_hour)
                    {
                        old_hour = RTCHOUR;
                        if (RTCDAY != old_day)
                        {

                        }
                    }
                }
                if (menu_stop_timer != 0)
                {
                    menu_stop_timer--;
                    if (menu_stop_timer == 0) clock_event_to_menu(MENU_STOP_FLAG);
                }
                //                StartADCmeasurements(measurement_bright);
//                CONTROL_OUT ^= AL1;
            }
//-----///////////////
            if (is_100ms != 0)  // is derived from 4MHz clock
            {
                is_100ms--;
                if (adc_power_count != 0)
                {
                    ADC_scheduler(status_adc_pwr_go_on);
                }
                if (adc_out_ready & TEMP_CPU_F_DISP_READY)
                {
//                    i = CALADC12_15V_30C;
                    __no_operation();
/*                    calcu_extension = (int32_t)((int32_t)adc_out_temp_cpu_f_disp_raw - (int32_t)CALADC12_15V_30C);
                    calcu_extension *= temp_cpu_coefficient;
                    calcu_extension += 30000;
                    calcu_extension /= 100;
                    adc_out_temp_cpu_f_disp = calcu_extension; */
/*                            (uint16_t) (((int32_t) () * temp_cpu_coefficient
                                    + 30000) / 100); */
                    calcu_temp_cpu_to_int();
                    GenerateDispOut();
                    adc_out_ready &= ~TEMP_CPU_F_DISP_READY;
                }
                if (adc_out_ready & TEMP_OUT_F_DISP_READY)
                {
                    calcu_temp_out_to_int();
                    GenerateDispOut();
                    adc_out_ready &= ~TEMP_OUT_F_DISP_READY;
                }
                if (adc_out_ready & BATT_F_DISP_READY)
                {
                    calcu_batt_to_int();
                    GenerateDispOut();
                    adc_out_ready &= ~BATT_F_DISP_READY;
                }
                if (adc_out_ready & BRIGHT_F_DISP_READY)
                {
                    GenerateDispOut();
                    adc_out_ready &= ~BRIGHT_F_DISP_READY;
                }
            } // if (is_100ms != 0)
//-----///////////////
            if (is_100ms2 != 0)  // is derived from 32kHz clock
            {
                is_100ms2--;
                btn_to_event();
                if ((alarm_start_flags & ALARM_BEEP) != 0)
                    AlarmBeep();
                if (alarm_start_flags & (ALARM_STOP | ALARM_LEV01 | ALARM_LEV02 | ALARM_LEV03 | ALARM_LEV04))
                    AlarmBell();
//                if (my_alarm_flags.alarm_on == 1) AlarmBell();

            } // if (is_100ms2 != 0)
//-----///////////////
            if (is_300ms != 0)
            {
                switch (eve_condition)
                {
                case normal:
                    GenerateDispOut();
                    break;
                case view_sec_and_min:
                    GenerateDispOut();
                    break;
                }
                is_300ms--;
                disp_brightness += 10;
                if (disp_brightness > TIME_PERIOD_DIGT - 10)
                    disp_brightness = 5;
            }
//            for (i = 4000; i > 0; i--)
            ;     // delay
//------///////////////
            if (adc_conv_ready)
            {
                ADC_scheduler(status_adc_ready);
                /*                adc_conv_ready = false;
                 temp_s_sum = 0;
                 for (i = 8; i > 0; i--)
                 {
                 temp_s_sum += adc_out_raw[i - 1];
                 }
                 temp_s_sum >>= 3; */
            }
/*
 * Button-events
 */
            if ((*(uint16_t *) btn_event_int_ptr)
                    || (*(uint16_t *) (btn_event_int_ptr + sizeof(uint16_t)))
                    || (*(uint16_t *) (btn_event_int_ptr + 2 * sizeof(uint16_t))))
            {
                clock_event_to_menu(MENU_WHATEVER_FLAG);
                GenerateDispOut();
            }
            __no_operation();                         // For debugger
        }
        else  // is_pwr_good
        {
            // batt. operated
        }
    }
}
