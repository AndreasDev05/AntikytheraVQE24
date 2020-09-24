/*
 * digi_clock_fkt.h
 *
 *  Created on: 06.09.2020
 *      Author: andi
 */

#include <stdbool.h>
#include <stdint.h>
#include <msp430.h>

#ifndef INCLUDES_DIGI_CLOCK_FKT_H_
#define INCLUDES_DIGI_CLOCK_FKT_H_

void calcu_temp_cpu_to_int(void);

void calcu_temp_out_to_int(void);

void calcu_batt_to_int(void);

#endif /* INCLUDES_DIGI_CLOCK_FKT_H_ */
