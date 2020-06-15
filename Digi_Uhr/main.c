#include <stdbool.h>
#include <string.h>
#include <msp430.h>
#include <msp430f5529.h>
#include <digi_clock.h>
#include <digi_clock_isr.h>

// Variables global clock management

// Variablen in den Interruptfkt. -----
    volatile bool is_pwr_good = true;
    volatile unsigned char is_sec,is_msec,is_msec2, is_300ms = 0;

    // Which value will be displayed
    volatile unsigned char eve_condition = view_temp_cpu;

// Variables for display management
    // to fast find the pin-quartet on display-port
    unsigned const int disp_pos[2] = {0x2010,0x8040};
    // display-memory position and BCD-number
    volatile unsigned char disp_out[4] = {0,1,2,4};
    // display-memory decimal point
    volatile unsigned char disp_point;
    // for fast overlay-functions on the display-memory
    volatile unsigned int  *disp_out_int = &disp_out;
    // to reduce flicker save the display in this memory
    volatile unsigned char disp_out_buf[4];
    volatile unsigned int  *disp_out_buf_int = &disp_out_buf;
    // pointer to the active display-memory
    volatile unsigned char *disp_out_point = disp_out;
    volatile unsigned char disp_count = 0;
    // the brightness of the display: it is recommended to use values between 10 and "TIME_PERIOD_DIGT"-10.
    volatile unsigned int  disp_brightness = 10;  // brightness

// Variables for ADC12
    volatile unsigned int temp_raw[8];
    volatile unsigned char temp_t_count = 8;
    bool temp_s_ready;
    unsigned int temp_s_sum;

/**
 * main.c
 */
int main(void)
{

    volatile unsigned int i;        // volatile to prevent optimization
    volatile unsigned char temp_byte = 0;

//////////// Initialize CPU function
    InitializeCPU();
// Initialize UCS
    Initialize_UCS_and_Crystals();
// Initialize the real time clock
    RTCCTL1 = RTCSSEL_0 | RTCMODE_H | RTCBCD_H;
    // RTCCTL23 = RTCCALF_3; //PIN 2.6 toggels with 1Hz
    RTCSEC = 0x022; // starts on sec 22
    RTCMIN = 0x022; // starts on sec 22

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
                StartADCmeasurements(measurement_bright);
            }
            if (is_300ms != 0)
            {
                is_300ms--;
                disp_brightness += 10;
                if (disp_brightness > TIME_PERIOD_DIGT - 10)
                    disp_brightness = 5;
                GenrateDispOut();
            }
//            for (i = 4000; i > 0; i--)
                ;     // delay
            if (temp_s_ready)
            {
                temp_s_ready = false;
                temp_s_sum = 0;
                for (i = 8; i > 0; i--)
                {
                    temp_s_sum += temp_raw[i - 1];
                }
                temp_s_sum >>= 3;
            }
//          DISPLAY2_OUT ^= LED_SEC;              // toggle P4.0
            __no_operation();                         // For debugger
        }
        else
        {
            // batt. operated
        }
    }
}
