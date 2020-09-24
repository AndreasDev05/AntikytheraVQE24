/*
 * digi_clock.c
 *
 *  Created on: 27.04.2020
 *      Author: andi
 */
#include <stdbool.h>
#include <stdint.h>
#include <msp430.h>
#include <msp430f5529.h>
#include "digi_clock.h"

extern volatile uint8_t disp_out[4];
extern const uint16_t disp_pos[2];
extern uint8_t disp_point;
extern volatile uint8_t *disp_out_point;
extern volatile uint16_t  *disp_out_int;
extern volatile uint8_t disp_out_buf[4];
extern volatile uint16_t  *disp_out_buf_int;
extern volatile void *disp_out_buf_int_ptr;
extern     int32_t calcu_extension,calcu_extension1;

void InitializeCPU(void)
{
    extern int32_t temp_cpu_coefficient;
    int32_t temp_prod;
    WDTCTL = WDTPW | WDTHOLD;       // stop watchdog timer
    __enable_interrupt();           // Enable interrupts globally
    temp_prod = CALADC12_15V_85C - CALADC12_15V_30C;
    temp_cpu_coefficient = (int32_t)(85000 - 30000) / temp_prod;
    __no_operation();
/*
 *        85.0 - 30.0           K in mKelvin
 * -------------------------
 * CAL_ADC_T85 - CAL_ADC_T30
 *
 */
}

void InitializePins(void)
{
    BUTTON_REN   = 0x0FF;    // all Pins of Port 1 input with a resistor
    BUTTON_OUT   = 0x0FF;    // all Pins of Port 1 with pull up
    BUTTON_IES   = BUTTON_1 | BUTTON_2 | BUTTON_3 | BUTTON_4 | BUTTON_5 | BUTTON_6; // buttonPins of Port1 Hi/Lo edge
    BUTTON_IFG   = 0x000;    // P1 IFGs cleared
    BUTTON_IE    = BUTTON_1 | BUTTON_2 | BUTTON_3 | BUTTON_4 | BUTTON_5 | BUTTON_6; // buttonPins interrupt enable
    DISPLAY_DIR  = 0xFF;     // configure P2.x as output
    SIGNALS_DIR  = DARK | LED_OSCI_FAULT | LED_SEC | LED_DP;

    CONTROL_DIR  = TURNON_OPA | TURNON_RELAY | AL1;
    CONTROL_DS  |= AL1;
    CONTROL_OUT |= AL1;

    VOLTMETER_SEL = U_BAT | U_TEMP | U_LIGTH;
}

void Initialize_UCS_and_Crystals(void)
{
    //Set Pins
    CRYSTAL_SEL |= XT2IN | XT2OUT | XIN | XOUT; // switch on crystalpins
    // set DCO fields and let the FLL work its magic
    __bis_SR_register(SCG0);            // Disable the FLL control loop
    UCSCTL0 = 0x0000;                   // Set lowest possible DCOx, MODx
    UCSCTL1 = DCORSEL_4;                // Select DCO range 8MHz operation
    UCSCTL2 |= FLLD__2 + 243;           // Set DCO Multiplier for 8MHz
                                               //  D  * ( N  + 1) * FLLRef = Fdco
                                               // 1/2 * (243 + 1) * 32768  = 4.000MHz
    __bic_SR_register(SCG0);            // Enable the FLL control loop

    UCSCTL6 |= XCAP_3;                        // Internal load cap
    UCSCTL6 = XT2DRIVE_0 | XT1DRIVE_0 ;       // primarily configure XT2 for 4MHz
    UCSCTL6 &= ~XT1OFF;                       // Set XT1 On
    UCSCTL5 = DIVM_0;

    if (InitializeXT2(true) == true)
        UCSCTL4 = SELA__XT1CLK | SELS__XT2CLK | SELM__XT2CLK;    // (Sub)Masterclock == XT1
    else {
        UCSCTL4 = SELA__XT1CLK | SELS__DCOCLK | SELM__DCOCLK;    // (Sub)Masterclock == DCO
        SIGNALS_OUT |= LED_OSCI_FAULT;
    }

}

bool InitializeXT2(bool osci_on)           // the 4MHz-crystal for the mainclock
{
    volatile uint16_t i;               // volatile to prevent optimization
    unsigned fault_count = 0;

    if (osci_on) {
        UCSCTL6 &= ~XT2OFF;                // Set XT2 On
        do
        {
            fault_count++;
            UCSCTL7 &= ~(XT2OFFG | XT1LFOFFG | DCOFFG); // Clear OSCFault flag
            SFRIFG1 &= ~OFIFG;                 // Clear fault flags
            for (i = 0xFF; i > 0; i--);        // Time for flag to set
        }
        while (((UCSCTL7 & XT2OFFG) != 0) & (fault_count < 20)); // OSCFault flag still set?
        if ((UCSCTL7 & XT2OFFG) == 0)
            return (true);
        else
            return (false);
    } else {
        UCSCTL6 |= XT2OFF;                 // Set XT2 Off
        return (true);
    }
}

void InitializeTimerA(void)
{
    // --Configure timer A0-- (source 32kHz)//

    // Configure counter 0
    TA0CCTL0 = CCIE;             // CCR0 toggle, interrupt enabled
    TA0CCR0 = TIME_PERIOD_SEC;  // default for 1 sec

    // Configure counter 1
    TA0CCTL1 = CCIE;            // CCR1 toggle, interrupt enabled
    TA0CCR1 = TIME_PERIOD_1;       // default for 0.5 msec

    // Configure counter 2
    TA0CCTL2 = CCIE;            // CCR2 toggle, interrupt enabled
    TA0CCR2 = TIME_PERIOD_2;       // default for 0.125 msec

    // Configure counter 3
    TA0CCTL3 = CCIE;            // CCR2 toggle, interrupt enabled
    TA0CCR3 = TIME_PERIOD_300ms;   // default for 333 msec

    // Configure counter 4
    TA0CCTL4 = CCIE;            // CCR2 toggle, interrupt enabled
    TA0CCR4 = TIME_PERIOD_100ms2;   // default for 100 msec

    // Configure timer A0 configureregister
    TA0CTL = TASSEL__ACLK | MC__CONTINUOUS | TACLR;

    // --Configure timer A1-- (source 4MHz)//

    // Configure counter 0
    TA1CCTL0 = CCIE;             // CCR0 toggle, interrupt enabled
    TA1CCR0 = TIME_PERIOD_DIGT;  // default for

    // Configure counter 1
    /*     TA0CCTL1 = CCIE;        // CCR1 toggle, interrupt enabled
     TA0CCR1  = 10383;           // default for 0.5 msec */

    // Configure counter 2
    TA1CCTL2 = CCIE;            // CCR1 toggle, interrupt enabled
    TA1CCR0 = TIME_PERIOD_100ms;   // default for 100 msec

    // Configure timer A0 configureregister
    TA1CTL = TASSEL__SMCLK | ID__8 | MC__CONTINUOUS | TACLR;
}

void InitializeADC12(void)
{
    REFCTL0 &= ~REFMSTR;                      // Reset REFMSTR to hand over control to
                                              // ADC12_A ref control registers
    ADC12CTL0 = ADC12SHT0_8 | ADC12REFON | ADC12ON | ADC12MSC;
                                              // Internal ref = 1.5V
    ADC12CTL1 = ADC12SHP | ADC12DIV_4 | ADC12SSEL_2 | ADC12CONSEQ_2;     // enable sample timer & cont. sample
//    ADC12MCTL0 = ADC12SREF_1 + ADC12INCH_10;  // ADC i/p ch A10 = temp sense i/p
    ADC12IE = 0x001;                          // ADC_IFG upon conv result-ADCMEMO
    __delay_cycles(100);                      // delay to allow Ref to settle
    ADC12CTL0 |= ADC12ENC;
    ADC12CTL0 |= ADC12SC;
}

void StartADCmeasurements(enum ADC_mesure_typ what_mesure)
{

    switch (what_mesure)
    // sorted by frequency of the call
    {
    case measurement_bright:
        ADC12CTL0 |= ADC12REF2_5V;               // Internal ref = 2.5V
        ADC12MCTL0 = ADC12SREF_1 + ADC12INCH_2; // ADC i/p ch A2 = brightness sensor (P6.2)
        break;
    case measurement_batt:
        ADC12CTL0 &= ~ADC12REF2_5V;               // Internal ref = 1.5V
        ADC12MCTL0 = ADC12SREF_1 + ADC12INCH_0; // ADC i/p ch A0 = batteries voltage (P6.0)
        break;
    case measurement_temp_cpu:
        ADC12CTL0 &= ~ADC12REF2_5V;               // Internal ref = 1.5V
        ADC12MCTL0 = ADC12SREF_1 + ADC12INCH_10; // ADC i/p ch A10 = temp sense i/p (intern)
        break;
    case measurement_temp_out:
        ADC12CTL0 |= ADC12REF2_5V;                // Internal ref = 2.5V
        ADC12MCTL0 = ADC12SREF_1 + ADC12INCH_1; // ADC i/p ch A1 = temp sense out (P6.1)
        break;
    }
    ADC12CTL1 |= ADC12CONSEQ_2;  // read permanent one channel
    ADC12CTL0 |= ADC12ENC;
    ADC12CTL0 |= ADC12SC;
}

void ADC_scheduler(enum ADC_Work ADC_work)
{
    // with this function it is possible to control 4 ADC-task
    static uint16_t presentADCtask_point = 0;
    uint8_t adc_i;
    uint16_t adc_sum_raw;
    static enum ADC_Work _ADCtask[4] = { status_it_is_nothing,
                                         status_it_is_nothing,
                                         status_it_is_nothing,
                                         status_it_is_nothing };
    extern volatile uint16_t adc_out_raw[8];
    extern volatile bool adc_conv_ready;
    extern uint8_t adc_out_ready;
    extern uint16_t adc_out_bright_contr, adc_out_bright_f_disp,
                    adc_out_batt_f_contr, adc_out_batt_f_disp,
                    adc_out_temp_cpu_f_disp_raw, adc_out_temp_out_f_disp_raw;

    if (ADC_work != status_adc_ready)
    {
        if (_ADCtask[presentADCtask_point] == status_it_is_nothing) // first round
        {
            _ADCtask[presentADCtask_point] = ADC_work;
            switch (_ADCtask[presentADCtask_point])
            {
            case measure_bright_f_disp:
                StartADCmeasurements(measurement_bright);
                break;
            case measure_batt_f_contr:
                StartADCmeasurements(measurement_batt);
                break;
            case measure_batt_f_disp:
                StartADCmeasurements(measurement_batt);
                break;
            case measure_bright_f_contr:
                StartADCmeasurements(measurement_bright);
                break;
            case measure_temp_cpu_f_disp:
                StartADCmeasurements(measurement_temp_cpu);
                break;
            case measure_temp_out_f_disp:
                StartADCmeasurements(measurement_temp_out);
                break;
            }
        }
        else    // (_ADCtask[presentADCtask_point] == status_it_is_nothing) searching next free memory
        {
            for (adc_i = 1; adc_i < 4; adc_i++) // search the next free register; if more measurements, forget it ;-)
            {
                if (_ADCtask[(presentADCtask_point + adc_i) & 3] == status_it_is_nothing)
                {
                    _ADCtask[(presentADCtask_point + adc_i) & 3] = ADC_work;
                    break;  // stop the search
                }
            }
        }
    }
    else    // (ADC_work != status_adc_ready)
    {
        adc_conv_ready = false;
        adc_sum_raw = 0;
        for (adc_i = 8; adc_i > 0; adc_i--)
        {
            adc_sum_raw += adc_out_raw[adc_i - 1];
        }
        if (adc_sum_raw & 4)
            adc_sum_raw += 8; // a simple rounding, because no floating point operation was implemented
        adc_sum_raw >>= 3;  // eql. div 8
        switch (_ADCtask[presentADCtask_point])
        {
        case measure_bright_f_contr:
            adc_out_bright_contr = adc_sum_raw;
            adc_out_ready |= BRIGHT_F_CONTR_READY;
            break;
        case measure_batt_f_contr:
            adc_out_batt_f_contr = adc_sum_raw;
            adc_out_ready |= BATT_F_CONTR_READY;
            break;
        case measure_bright_f_disp:
            adc_out_bright_f_disp = adc_sum_raw;
            adc_out_ready |= BRIGHT_F_DISP_READY;
            break;
        case measure_batt_f_disp:
            adc_out_batt_f_disp = adc_sum_raw;
            adc_out_ready |= BATT_F_DISP_READY;
            break;
        case measure_temp_cpu_f_disp:
            adc_out_temp_cpu_f_disp_raw = adc_sum_raw;
            adc_out_ready |= TEMP_CPU_F_DISP_READY;
            break;
        case measure_temp_out_f_disp:
            adc_out_temp_out_f_disp_raw = adc_sum_raw;
            adc_out_ready |= TEMP_OUT_F_DISP_READY;
            break;
        }
        _ADCtask[presentADCtask_point] = status_it_is_nothing;
        presentADCtask_point++;
        presentADCtask_point &= 3;
        if (_ADCtask[presentADCtask_point] != status_it_is_nothing)
        {
            switch (_ADCtask[presentADCtask_point])
            {
            case measure_bright_f_disp:
                StartADCmeasurements(measurement_bright);
                break;
            case measure_batt_f_contr:
                StartADCmeasurements(measurement_batt);
                break;
            case measure_batt_f_disp:
                StartADCmeasurements(measurement_batt);
                break;
            case measure_bright_f_contr:
                StartADCmeasurements(measurement_bright);
                break;
            case measure_temp_cpu_f_disp:
                StartADCmeasurements(measurement_temp_cpu);
                break;
            case measure_temp_out_f_disp:
                StartADCmeasurements(measurement_temp_out);
                break;
            }
        }
    }
}

void GenerateDispOut(void)
{
    extern volatile uint8_t eve_condition;
    extern uint16_t adc_out_bright_contr, adc_out_bright_f_disp,
                    adc_out_batt_f_contr, adc_out_batt_f_disp,
                    adc_out_temp_cpu_f_disp, adc_out_temp_out_f_disp;
/*    if ((my_display_flag.light_blink)&((time_ary[e_sec]& 1) != 0 ))
    {
        disp_out[3] = 0xF;
        disp_out[2] = 0xF;
        disp_out[1] = 0xF;
        disp_out[0] = 0xF;
    }
    else
    {   */
    disp_out_point = disp_out_buf;
    switch (eve_condition)
    {
    case normal:
        disp_out[3] = RTCMIN & 0x0F;
        disp_out[2] = RTCMIN >> 4;
        disp_out[1] = RTCHOUR & 0x0F;
        disp_out[0] = RTCHOUR >> 4;
        break;
/*    case view_temp_out:
        disp_out[3] = disp_hundred[1];
        disp_out[2] = disp_hundred[0];
        disp_out[1] = disp_ten[1];
        disp_out[0] = disp_ten[0];
        break; */
    case view_sec_and_min:
        disp_out[3] = RTCSEC & 0x0F;
        disp_out[2] = RTCSEC >> 4;
        disp_out[1] = RTCMIN & 0x0F;
        disp_out[0] = RTCMIN >> 4;
        break;
    case view_day_and_month:
        disp_out[3] = RTCDAY & 0x0F;
        disp_out[2] = RTCDAY >> 4;
        disp_out[1] = RTCMON & 0x0F;
        disp_out[0] = RTCMON >> 4;
        break;
    case view_day_of_week:
        disp_out[3] = 0xF;
        disp_out[2] = 0xF;
        disp_out[1] = 0xF;
        if (RTCDOW == 0)
            disp_out[0] = 7;
        else
            disp_out[0] = RTCDOW;
        break;
    case view_year:
        disp_out[3] = RTCYEARL & 0x0F;
        disp_out[2] = RTCYEARL >> 4;
        disp_out[1] = RTCYEARH & 0x0F;
        disp_out[0] = RTCYEARH >> 4;
        break;
    case view_temp_cpu:
//        Int2str_m(adc_out_temp_cpu_f_disp, &disp_out);
        Int2str_m(adc_out_temp_cpu_f_disp, &disp_out);
        break;
    case view_temp_out:
        Int2str_m(adc_out_temp_out_f_disp, &disp_out);
        break;
    }
    *disp_out_int &= 0x0F0F;
    *(disp_out_int + 1) &= 0x0F0F;
    *disp_out_int |= disp_pos[0];
    *(disp_out_int + 1) |= disp_pos[1];
    disp_out_point = disp_out;
    *disp_out_buf_int = *disp_out_int;
    *(disp_out_buf_int + 1) = *(disp_out_int + 1);

//    }
}

void Int2str_m(int16_t number_int,volatile uint8_t *disp_local)
// convert a integer with 9999 to BCD / char[4] array
// !! this function is for MCUs and has no ERRORcontrol !!
{
    int16_t tempInt;

    if (number_int < 0)
    {
//        SIGNALS_OUT |= LED_OSCI_FAULT;
        tempInt = number_int * (-1);
    }
    else
    {
//        SIGNALS_OUT &= ~LED_OSCI_FAULT;
        tempInt = number_int;
    }

    if (tempInt > 999)
    {
        *(disp_local + 3) = tempInt / 1000;
        tempInt = tempInt % 1000;
    }
    else
        *(disp_local + 3) = 0;
    if (tempInt > 99)
    {
        *(disp_local + 2) = tempInt / 100;
        tempInt %= 100;
    }
    else
        *(disp_local + 2) = 0;
    if (tempInt > 9)
    {
        *(disp_local + 1) = tempInt / 10;
        *disp_local = tempInt % 10;
    }
    else
    {
        *(disp_local + 1) = 0;
        *disp_local = tempInt;
    }
}

void Char2str_m(uint8_t number_char, uint8_t *disp_local)
{
    uint8_t tempChar;

    *(disp_out + 3) = 0; // The thousand digit is always 0 anyway.

    if (number_char > 99)
    {
        *(disp_out + 2) = number_char / 100;
        tempChar = number_char % 100;
    }
    else
        *(disp_out + 2) = 0;
    if (tempChar > 9)
    {
        *(disp_out + 1) = tempChar / 10;
        *disp_out = tempChar % 10;
    }
    else
    {
        *(disp_out + 1) = 0;
        *disp_out = tempChar;
    }
}

void Char2str_d(uint8_t number_char, uint8_t *disp_local, bool first_digits)
{
    if (first_digits)
    {
        if (number_char > 9)
        {
            *(disp_out + 1) = number_char / 10;
            *disp_out = number_char % 10;
        }
        else
        {
            *(disp_out + 1) = 0;
            *disp_out = number_char;
        }
    }
    else
    {
        if (number_char > 9)
        {
            *(disp_out + 3) = number_char / 10;
            *(disp_out + 2) = number_char % 10;
        }
        else
        {
            *(disp_out + 3) = 0;
            *(disp_out + 2) = number_char;
        }
    }
}
