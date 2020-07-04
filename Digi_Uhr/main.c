#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <msp430.h>
#include <msp430f5529.h>
#include <digi_clock.h>
#include <digi_clock_isr.h>

// Variables global clock management

// Variablen in den Interruptfkt. -----
    volatile bool is_pwr_good = true;
    volatile uint8_t is_sec,is_msec,is_msec2, is_100ms, is_300ms = 0;

    // Which value will be displayed
    volatile uint8_t eve_condition = view_temp_cpu;

// Variables for display management
    // to fast find the pin-quartet on display-port
    unsigned const int disp_pos[2] = {0x2010,0x8040};
    // display-memory position and BCD-number
    volatile uint8_t disp_out[4] = {0x10,0x21,0x42,0x84};
    // display-memory decimal point
    volatile uint8_t disp_point;
    // for fast overlay-functions on the display-memory
    volatile uint16_t  *disp_out_int = &disp_out;
    // to reduce flicker save the display in this memory
    volatile uint8_t disp_out_buf[4];
    volatile uint16_t  *disp_out_buf_int = &disp_out_buf;
    // pointer to the active display-memory
    volatile uint8_t *disp_out_point = disp_out;
    volatile uint8_t disp_count = 0;
    // the brightness of the display: it is recommended to use values between 10 and "TIME_PERIOD_DIGT"-10.
    volatile uint16_t  disp_brightness = 10;  // brightness

// Variables for ADC12
volatile uint16_t adc_out_raw[8];
uint16_t adc_out_bright_contr, adc_out_bright_f_disp, adc_out_batt_f_contr,
        adc_out_batt_f_disp, adc_out_temp_cpu_f_disp_raw,
        adc_out_temp_cpu_f_disp, adc_out_temp_out_f_disp;
volatile bool adc_conv_ready;
uint8_t  adc_out_ready = 0;  // Bitarray that signalizes witch ADC-channel is ready - named bits in the header
uint16_t temp_s_sum;
int32_t  temp_cpu_coefficient; // precalculated factor for CPU-temperature measurement
volatile int32_t calcu_extension,calcu_extension1;

/**
 * main.c
 */
int main(void)
{

    volatile uint16_t i;        // volatile to prevent optimization
    volatile uint8_t temp_byte = 0;

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
//                ADC_scheduler(measure_bright_f_contr);
                ADC_scheduler(measure_temp_cpu_f_disp);
                //                StartADCmeasurements(measurement_bright);
            }
            if (is_100ms != 0)
            {
                is_100ms--;
                if (adc_out_ready & TEMP_CPU_F_DISP_READY)
                {
//                    i = CALADC12_15V_30C;
                    __no_operation();
                    calcu_extension = (int32_t)((int32_t)adc_out_temp_cpu_f_disp_raw - (int32_t)CALADC12_15V_30C);
                    calcu_extension1 = calcu_extension * temp_cpu_coefficient;
                    calcu_extension = calcu_extension1 + 30000;
                    calcu_extension /= 100;
                    adc_out_temp_cpu_f_disp = calcu_extension;
/*                            (uint16_t) (((int32_t) () * temp_cpu_coefficient
                                    + 30000) / 100); */
                    GenerateDispOut();
                    adc_out_ready &= ~TEMP_CPU_F_DISP_READY;
                }
            }
            if (is_300ms != 0)
            {
                is_300ms--;
                disp_brightness += 10;
                if (disp_brightness > TIME_PERIOD_DIGT - 10)
                    disp_brightness = 5;
            }
//            for (i = 4000; i > 0; i--)
            ;     // delay
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
//          SIGNALS_OUT ^= LED_SEC;              // toggle P4.0
            __no_operation();                         // For debugger
        }
        else
        {
            // batt. operated
        }
    }
}
