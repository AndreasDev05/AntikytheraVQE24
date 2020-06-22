/*
 * digi_clock_isr.c
 *
 *  Created on: 05.05.2020
 *      Author: andi
 */

#include <stdbool.h>
#include <stdint.h>
#include <msp430.h>
#include <msp430f5529.h>
#include "digi_clock.h"
#include "digi_clock_isr.h"

// Variablen in den Interruptfkt. -----
    extern volatile uint8_t is_sec,is_msec,is_msec2,is_300ms;
// Variables for display management
    extern volatile const uint16_t disp_pos[4];
    extern volatile uint8_t disp_out[4];
    extern volatile uint16_t  *disp_out_int;
    // to reduce flicker save the display in this memory
    extern volatile uint8_t disp_out_buf[4];
    // pointer to the active displaymemory
    extern volatile uint8_t *disp_out_point;
    extern volatile uint8_t disp_count;
    // the brightness of the display: it is recommended to use values between 10 and "TIME_PERIOD_DIGT"-10.
    extern volatile uint16_t  disp_brightness;  // brightness

// Variables for ADC12
    extern volatile uint16_t adc_out_raw[8];
    extern volatile uint8_t temp_t_count;
    extern bool temp_s_ready;    // ready

#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void)
{
    TA0CCR0   += TIME_PERIOD_SEC;
    is_sec++;
    __no_operation();                         // For debugger

//    DISPLAY2_OUT    ^= LED_SEC;                            // Toggle P7.3
}

#pragma vector=TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR(void)
{
    switch (__even_in_range(TA0IV, 14))
    {
    case 0:
        break;
    case 2:
        TA0CCR1 += TIME_PERIOD_1;          // Add Offset to CCR1
        is_msec++;
        break;
    case 4:
        TA0CCR2 += TIME_PERIOD_2;                // Add Offset to CCR2
        is_msec2++;
//               DISPLAY2_OUT ^= LED_SEC;        // Toggle P7.3
        break;
    case 6:
        TA0CCR3 += TIME_PERIOD_300ms;
        is_300ms++;
        break;                         // CCR3 not used
    case 8:
        break;                         // CCR4 not used
    case 10:
        break;                         // CCR5 not used
    case 12:
        break;                         // Reserved not used
    case 14:                                // overflow
        break;
    default:
        break;
    }

}

#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR(void)
{
    TA1CCR0         += TIME_PERIOD_DIGT;
    DISPLAY2_OUT    |= LED_SEC;                            // switch P7.3 on
    DISPLAY_OUT = *(disp_out_point+disp_count);
//    if (disp_count < 3) disp_count++; else disp_count = 0;
    disp_count++;
    disp_count &= 0x03;

    //  Set the Compare-Register and starts the interrupt to adjust the brightness
    TA1CCR1  = TA1R + disp_brightness;     // brightness
    TA1CCTL1 = CCIE;            // CCR1 interrupt enabled
}

#pragma vector=TIMER1_A1_VECTOR
__interrupt void TIMER1_A1_ISR(void)
{
    switch(__even_in_range(TA1IV,14))
    {
      case 0:  break;
      case 2:  TA1CCTL1 &= ~CCIE;             // CCR1 interrupt disabled
               DISPLAY2_OUT &= ~LED_SEC;      // switch P7.3 off
               break;
      case 4:  TA1CCR2 += TIME_PERIOD_2;      // Add Offset to CCR2
               break;
      case 6:  break;                         // CCR3 not used
      case 8:  break;                         // CCR4 not used
      case 10: break;                         // CCR5 not used
      case 12: break;                         // Reserved not used
      case 14:                                // overflow
               break;
      default: break;
   }

}

#pragma vector=ADC12_VECTOR
__interrupt void ADC12ISR (void)
{
  static uint8_t adc_m_count = 8;
  switch(__even_in_range(ADC12IV,34))
  {
  case  0: break;                           // Vector  0:  No interrupt
  case  2: break;                           // Vector  2:  ADC overflow
  case  4: break;                           // Vector  4:  ADC timing overflow
  case  6:                                  // Vector  6:  ADC12IFG0
    adc_m_count--;
    adc_out_raw[adc_m_count] = ADC12MEM0;                   // Move results, IFG is cleared
    if (adc_m_count == 0)
    {
        adc_m_count = 8;
        ADC12CTL1 &=  ~(0x06);       //
        ADC12CTL0 &=  ~ADC12SC;            // switch ADC-process off
        ADC12CTL0 &=  ~ADC12ENC;
        temp_s_ready = true;
    }
    break;
  case  8: break;                           // Vector  8:  ADC12IFG1
  case 10: break;                           // Vector 10:  ADC12IFG2
  case 12: break;                           // Vector 12:  ADC12IFG3
  case 14: break;                           // Vector 14:  ADC12IFG4
  case 16: break;                           // Vector 16:  ADC12IFG5
  case 18: break;                           // Vector 18:  ADC12IFG6
  case 20: break;                           // Vector 20:  ADC12IFG7
  case 22: break;                           // Vector 22:  ADC12IFG8
  case 24: break;                           // Vector 24:  ADC12IFG9
  case 26: break;                           // Vector 26:  ADC12IFG10
  case 28: break;                           // Vector 28:  ADC12IFG11
  case 30: break;                           // Vector 30:  ADC12IFG12
  case 32: break;                           // Vector 32:  ADC12IFG13
  case 34: break;                           // Vector 34:  ADC12IFG14
  default: break;
  }
}
