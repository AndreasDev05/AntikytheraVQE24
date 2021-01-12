/*
 * digi_clock_fkt.c
 *
 *  Created on: 06.09.2020
 *      Author: andi
 */

#include <stdbool.h>
#include <stdint.h>
#include <msp430.h>
#include <msp430f5529.h>
#include "digi_clock.h"
#include "digi_clock_btn_menu.h"


void calcu_temp_cpu_to_int(void)
{
    int32_t calcu_extension;
    extern int16_t adc_out_temp_cpu_f_disp, adc_out_temp_cpu_f_disp_raw;
    extern int32_t  temp_cpu_coefficient;

    calcu_extension = (int32_t)((int32_t)adc_out_temp_cpu_f_disp_raw - (int32_t)CALADC12_15V_30C);
    calcu_extension *= temp_cpu_coefficient;
    calcu_extension += 30000;
    calcu_extension /= 100;
    adc_out_temp_cpu_f_disp = calcu_extension;
}

void calcu_temp_out_to_int(void)
{
    int32_t calcu_extension;
    uint16_t temp_adc;
    extern int16_t adc_out_temp_out_f_disp, adc_out_temp_out_f_disp_raw;
    extern int32_t  temp_out_coefficient;
    __no_operation();                         // For debugger

    calcu_extension = (int32_t)((int32_t)adc_out_temp_out_f_disp_raw - (int32_t)CALADC12_25V_PT100_00C);
    calcu_extension *= temp_out_coefficient;
    calcu_extension += -1000;
    calcu_extension /= 100;

    // invert the digits; in experimental is the voltage invert
//    temp_adc = ~adc_out_temp_out_f_disp_raw;
//    temp_adc = temp_adc & 0x0FFF;

/*
 *  formula to calculate display
 *
 *
 */
/*    calcu_extension = (int32_t) temp_adc - CORREC_OFFSET_PT100;
    calcu_extension = (int32_t) ((int32_t) calcu_extension) * 92;
    calcu_extension >>= 6; */
    adc_out_temp_out_f_disp = (int16_t) calcu_extension;
//    adc_out_temp_out_f_disp = adc_out_temp_out_f_disp_raw;
}

void calcu_batt_to_int(void)
{
    int32_t calcu_extension;
    extern int16_t adc_out_batt_f_disp, adc_out_batt_f_disp_raw;
    extern int32_t  batt_coefficient;
    __no_operation();                         // For debugger

    calcu_extension = (int32_t)((int32_t)adc_out_batt_f_disp_raw - (int32_t)CALADC12_25V_BATT_1V5);
    calcu_extension *= batt_coefficient;
    calcu_extension += 15000;
    calcu_extension /= 100;

    // invert the digits; in experimental is the voltage invert
//    temp_adc = ~adc_out_temp_out_f_disp_raw;
//    temp_adc = temp_adc & 0x0FFF;

/*
 *  formula to calculate display
 *
 *
 */
/*    calcu_extension = (int32_t) temp_adc - CORREC_OFFSET_PT100;
    calcu_extension = (int32_t) ((int32_t) calcu_extension) * 92;
    calcu_extension >>= 6; */
    adc_out_batt_f_disp = (int16_t) calcu_extension;
//    adc_out_batt_f_disp = adc_out_batt_f_disp_raw;
}
