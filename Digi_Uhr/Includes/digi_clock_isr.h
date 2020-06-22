/*
 * digi_clock_isr.h
 *
 *  Created on: 05.05.2020
 *      Author: andi
 */

#include <stdbool.h>
#include <stdint.h>
#include <msp430.h>
//#include <msp430f5529.h>

#ifndef INCLUDES_DIGI_CLOCK_ISR_H_
#define INCLUDES_DIGI_CLOCK_ISR_H_

//#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void);

__interrupt void TIMER0_A1_ISR(void);

__interrupt void TIMER1_A0_ISR(void);

__interrupt void TIMER1_A1_ISR(void);

__interrupt void ADC12ISR (void);

#endif /* INCLUDES_DIGI_CLOCK_ISR_H_ */
