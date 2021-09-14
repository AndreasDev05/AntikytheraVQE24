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

/*/ #ifdef HW_Ver1_2
#define     BUTTON_1              BIT0      // Switch down
#define     BUTTON_2              BIT1      // Switch up
#define     BUTTON_3              BIT2      // Switch left
#define     BUTTON_4              BIT3      // Switch right
#define     BUTTON_5              BIT4      // Menu
#define     BUTTON_6              BIT5      // Alarm off
#endif */

// #ifdef HW_Ver1_0
#define     BUTTON_1              BIT0      // Switch down
#define     BUTTON_2              BIT2      // Switch up
#define     BUTTON_3              BIT4      // Switch left
#define     BUTTON_4              BIT6      // Switch right
#define     BUTTON_5              BIT1      // Menu
#define     BUTTON_6              BIT3      // Alarm off
//#endif

#define     BUTTON_down           BIT0      // Switch down
#define     BUTTON_up             BIT1      // Switch up
#define     BUTTON_left           BIT2      // Switch left
#define     BUTTON_right          BIT3      // Switch right
#define     BUTTON_mode           BIT4      // Menu
#define     BUTTON_alarm          BIT5      // Alarm off

#define     BUTTON_last           BIT5
#define     PWR_UP                BIT7      // 5 Volt ready

#define     BTN_PRESS_SHORT       BIT0
#define     BTN_PRESS_LONG        BIT1
#define     BTN_PRESS_INTER       BIT2
#define     BTN_PRESS_DOUBL       BIT3
#define     BTN_PRESS_INTER_FIRST BIT7

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

#define     DIGT_PNT1             BIT1
#define     DIGT_PNT2             BIT0
#define     DIGT_PNT3             BIT3
#define     DIGT_PNT4             BIT2
#define     SEC_PNT               BIT4
#define     MINUS_PNT             BIT5
#define     SEC_PNT_RESULT        BIT6
#define     MINUS_PNT_RESULT      BIT7

#define     SERIAL_SEL            P3SEL
#define     TXD                   BIT3      // TXD on P3.3
#define     RXD                   BIT4      // RXD on P3.4

#define     CONTROL_OUT           P4OUT
#define     CONTROL_DIR           P4DIR
#define     CONTROL_IN            P4IN
#define     CONTROL_DS            P4DS
#define     TURNON_OPA            BIT0      // switch the op-amp on
#define     TURNON_RELAY          BIT1      // switch the relay on
#define     AL1                   BIT2      // Alarm Piezo

#define     CRYSTAL_SEL           P5SEL
#define     XT2IN                 BIT2
#define     XT2OUT                BIT3
#define     XIN                   BIT4
#define     XOUT                  BIT5

#define     VOLTMETER_SEL         P6SEL
#define     U_BAT                 BIT0
#define     U_TEMP                BIT1
#define     U_LIGTH               BIT2

#define     SIGNALS_OUT           P7OUT
#define     SIGNALS_DIR           P7DIR
#define     SIGNALS_IN            P7IN
#define     SIGNALS_DS            P7DS

#define     DARK                  BIT0      // Blanking Dunkeltastung
#define     LED_OSCI_FAULT        BIT1      // LED Oszillator 4MHz schwingt nicht
#define     LED_SEC               BIT2      // LED Sekunden
#define     LED_MINUS             BIT3      // LED für ein negatives Ergebnis
#define     LED_DP                BIT7      // LED decimal point

// ADC

#define CALADC12_15V_30C  *((uint16_t *)0x1A1A)   // Temperature Sensor Calibration-30 C
                                                      //See device datasheet for TLV table memory mapping
#define CALADC12_15V_85C  *((uint16_t *)0x1A1C)   // Temperature Sensor Calibration-85 C

#define CORREC_OFFSET_PT100     3050

#define CALADC12_25V_PT100_00C  398               // Temperature sensor PT100 calibration 0,0 C (100Ω)

#define CALADC12_25V_PT100_80C  1947

#define CALADC12_25V_BATT_1V5  632               // Battery Calibration 1,5V

#define CALADC12_25V_BATT_3V6  2948              // Battery Calibration 3,6V

// TYPEDEF & STRUCTs

// Variablen die "globale" Zusände steuern ----
// variables that control "global" states

enum EVE_con_typ
{
    normal = 0, // (hh:mm)
    view_sec_and_min,
    view_day_and_month,
    view_day_of_week,
    view_alarm_time1,
    view_alarm_time2,
    view_alarm_time3,
    view_alarm_time4,
    view_alarm_time5,
    view_year,
    view_temp_cpu,
    view_temp_out,
    view_bright,
    view_batt,
    view_rtc_corr,
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
    change_a_son,
    change_rtc_corr
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
    status_it_is_nothing = 0,
    status_adc_ready,
    status_adc_pwr_go_on,
    measure_batt_f_contr,
    measure_batt_f_disp,
    measure_bright_f_contr,
    measure_bright_f_disp,
    measure_temp_cpu_f_disp,
    measure_temp_out_f_disp,
};

enum FLASH_write_data
{
    write_ver,
    write_date,
    write_RTC_correction,
    write_TEMP_correction,
    write_BATT_correction,
    write_ALARM1,
    write_ALARM2,
    write_ALARM3,
    write_ALARM4,
    write_ALARM5,
};
#define     BATT_F_CONTR_READY    BIT0
#define     BATT_F_DISP_READY     BIT1
#define     BRIGHT_F_CONTR_READY  BIT2
#define     BRIGHT_F_DISP_READY   BIT3
#define     TEMP_CPU_F_DISP_READY BIT4
#define     TEMP_OUT_F_DISP_READY BIT5
#define     FIRST_ADC_DISPLAY     BIT7

#define     ALARM_BEEP            BIT0
#define     ALARM_LEV01           BIT1
#define     ALARM_LEV02           BIT2
#define     ALARM_LEV03           BIT3
#define     ALARM_LEV04           BIT4
#define     ALARM_ISR_FLAG        BIT6
#define     ALARM_STOP            BIT7

#define     BEEPER_ON             CONTROL_OUT &= ~AL1
#define     BEEPER_OFF            CONTROL_OUT |= AL1

#define     MENU_STOP_COUNT       10    // in sec
#define     MENU_WHATEVER_FLAG     0
#define     MENU_STOP_FLAG         1

struct alarm_set {
    uint8_t alarm_days, alarm_min, alarm_hour, alarm_day, alarm_mounth;
};

enum MENU_layer
{
    time_date,
    measured_data,
    alarm_times,
    set_clock,
    set_alarm_clock,
    set_sensor_offsets,
};

enum MENU_sub_layer
{
    menu_sub_level_m1 = -1,
    menu_sub_level_0,
    menu_sub_level_1,
    menu_sub_level_2,
    menu_sub_level_3,
    menu_sub_level_4,
};

// FUNCTIONS
/* Initialize CPU and basic variables */
void InitializeCPU(void);
void InitializePins(void);
/* initialize both crystals */
void Initialize_UCS_and_Crystals(void);
/* the 4MHz-crystal for the mainclock */
bool InitializeXT2(bool osci_on);
void InitializeRTC(void);
void InitializeTimerA(void);
void InitializeADC12(void);

// The folding functions writing directly in the displaymemory
// Generates the display output
void GenerateDispOut(void);
/* Translate a integer to BCD */
void Int2str_m(int16_t number_int,volatile uint8_t *disp_local);
/* Translate a char to BCD */
void Char2str_m(uint8_t number_char, uint8_t *disp_local);
/* Translate numbers up to 99 into BCD for the first and last double digits*/
void Char2str_d(uint8_t number_char, uint8_t *disp_local, bool first_digits);

// Workfunctions
// for ADC
void StartADCmeasurements(enum ADC_mesure_typ what_mesure);

void ADC_scheduler(enum ADC_Work ADC_work);

// VARIABLE
/* The timer (32kHz) give a 1sec event */
#define     TIME_PERIOD_SEC     0x03FFF

/* Interval zum Refresh der Anzeige 333ms */
#define     TIME_PERIOD_1       100

/* Interval zum Steuern des Multiplex 0.125ms */
#define     TIME_PERIOD_2       3

/* Interval zum abfragen des ADC  100ms Ergebnises 50000 */
#define     TIME_PERIOD_100ms2   0x0C80

/* Interval zum Weiterschalten der Ziffer 6.66ms */
#define     TIME_PERIOD_DIGT       3333

/* Interval zum abfragen des ADC  100ms Ergebnises 50000 */
#define     TIME_PERIOD_100ms   0x0C350

/* Interval zum abfragen der Schalter 0.5ms */
#define     TIME_PERIOD_300ms   0x01555

/*  Flash-variables

    array-bytes
*/

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

//      alarm_days_bits

#define     A_MONDAY        BIT0
#define     A_TUESDAY       BIT1
#define     A_WEDNESDAY      BIT2
#define     A_THURSDAY      BIT3
#define     A_FRIDAY        BIT4
#define     A_SATURDAY      BIT5
#define     A_SUNDAY        BIT6
#define     A_ACTIV         BIT7

//      segment definition

#define FLASH_SEGMENT_A  *((uint8_t *)0x1980)

#define FLASH_SEGMENT_C  *((uint8_t *)0x1880)

#endif /* INCLUDES_DIGI_CLOCK_H_ */
