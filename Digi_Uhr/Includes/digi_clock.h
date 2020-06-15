/*
 * digi_clock.h
 *
 *  Created on: 27.04.2020
 *      Author: andi
 */
#include <stdbool.h>
#include <msp430.h>
//#include <msp430f2229.h>

#ifndef INCLUDES_DIGI_CLOCK_H_
#define INCLUDES_DIGI_CLOCK_H_

#define     BUTTON_OUT            P1OUT
#define     BUTTON_DIR            P1DIR
#define     BUTTON_IN             P1IN
#define     BUTTON_IE             P1IE
#define     BUTTON_IES            P1IES
#define     BUTTON_IFG            P1IFG
#define     BUTTON_REN            P1REN
#define     BUTTON_1              BIT0      // Switch left
#define     BUTTON_2              BIT1      // Switch right
#define     BUTTON_3              BIT2      // Switch up
#define     BUTTON_4              BIT3      // Switch down
#define     BUTTON_5              BIT4      // Menu
#define     BUTTON_6              BIT5      // Alarm off
#define     PWR_UP                BIT7      // 5 Volt ready

#define     DISPLAY_OUT           P2OUT
#define     DISPLAY_DIR           P2DIR
#define     DISPLAY_IN            P2IN
#define     BCD_0                 BIT0
#define     BCD_1                 BIT1
#define     BCD_2                 BIT2
#define     BCD_3                 BIT3
#define     DIGT_1                BIT4
#define     DIGT_2                BIT5
#define     DIGT_3                BIT6
#define     DIGT_4                BIT7

#define     SERIAL_SEL            P3SEL
#define     TXD                   BIT3      // TXD on P3.3
#define     RXD                   BIT4      // RXD on P3.4
/*
#define     DISPLAY2_OUT          P4OUT
#define     DISPLAY2_DIR          P4DIR
#define     DISPLAY2_IN           P4IN
#define     DARK                  BIT0      // Dunkeltastung
#define     AL1                   BIT1      // Alarm Piezo
#define     LED_SEC               BIT2      // LED Sekunden
*/
#define     CRYSTAL_SEL           P5SEL
#define     XT2IN                 BIT2
#define     XT2OUT                BIT3
#define     XIN                   BIT4
#define     XOUT                  BIT5

#define     VOLTMETER_SEL         P6SEL
#define     U_BAT                 BIT0
#define     U_TEMP                BIT1
#define     U_LIGTH               BIT2

#define     DISPLAY2_OUT          P7OUT
#define     DISPLAY2_DIR          P7DIR
#define     DISPLAY2_IN           P7IN
#define     DARK                  BIT0      // Dunkeltastung
#define     AL1                   BIT1      // Alarm Piezo
#define     LED_OSCI_FAULT        BIT2      // LED Oszillator 4MHz schwingt nicht
#define     LED_SEC               BIT3      // LED Sekunden

// ADC

#define CALADC12_15V_30C  *((unsigned int *)0x1A1A)   // Temperature Sensor Calibration-30 C
                                                      //See device datasheet for TLV table memory mapping
#define CALADC12_15V_85C  *((unsigned int *)0x1A1C)   // Temperature Sensor Calibration-85 C

// TYPEDEF & STRUCTs

// Variablen die "globale" Zusände steuern ----
// variables that control "global" states

enum
{
    normal = 0, // (hh:mm)
    view_sec_and_min,
    view_day_and_month,
    view_day_of_week,
    view_year,
    view_temp_cpu,
    view_temp_out,
    change_sec,
    change_min,
    change_h,
    change_d,
    change_monts,
    change_year,
    change_arlarm,
    change_arl_time,
    change_a_mon,
    change_a_tue,
    change_a_wend,
    change_a_thu,
    change_a_fri,
    change_a_san,
    change_a_son
} ene_con_gr;

enum ADC_mesure_typ
{
    measurement_temp_cpu,
    measurement_batt,
    measurement_temp_out,
    measurement_bright
};

enum ADC_Work
{
    measure_batt_f_contr,
    measure_batt_f_disp,
    measure_bright_f_contr,
    measure_bright_f_disp,
    meeasur_temp_cpu_f_disp,
    meeasur_temp_out_f_disp,
};

// FUNCTIONS
/* Initialize CPU and basic variables */
void InitializeCPU(void);
void InitializePins(void);
/* initialize both crystals */
void Initialize_UCS_and_Crystals(void);
/* the 4MHz-crystal for the mainclock */
bool InitializeXT2(bool osci_on);
void InitializeTimerA(void);
void InitializeADC12(void);

// The folding functions writing directly in the displaymemory
// Generates the display output
void GenrateDispOut(void);
/* Translate a integer to BCD */
void Int2str_m(unsigned int number_int, unsigned char *disp_local);
/* Translate a char to BCD */
void Char2str_m(unsigned char number_char, unsigned char *disp_local);
/* Translate numbers up to 99 into BCD for the first and last double digits*/
void Char2str_d(unsigned char number_char, unsigned char *disp_local, bool first_digits);

// Workfunctions
// for ADC
void StartADCmeasurements(enum ADC_mesure_typ what_mesure);

void ADC_schedula(enum ADC_Work ADC_work);

// VARIABLE
/* The timer (32kHz) give a 1sec event */
#define     TIME_PERIOD_SEC     0x03FFF
/* Interval zum abfragen der Schalter 0.5ms */
#define     TIME_PERIOD_300ms   0x01555
/* Interval zum Refresh der Anzeige 333ms */
#define     TIME_PERIOD_1       100
/* Interval zum Steuern des Multiplex 0.125ms */
#define     TIME_PERIOD_2       3
/* Interval zum Weiterschalten der Ziffer 6.66ms */
#define     TIME_PERIOD_DIGT       3333



#endif /* INCLUDES_DIGI_CLOCK_H_ */
