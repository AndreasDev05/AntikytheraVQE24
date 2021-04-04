/*
 * digi_clock_flash.c
 *
 *  Created on: 19.02.2021
 *      Author: andi
 */

#include <msp430.h>
#include <msp430f5529.h>

void FlashWrite(void)
{
    char *Flash_ptrA;
    char *Flash_ptrC;
    char i;

    Flash_ptrA = (char *) 0x1980;             // Initialize Flash segment A ptr
    Flash_ptrC = (char *) 0x1980;             // Initialize Flash segment D ptr

    FCTL3 = FWKEY;                            // Clear Lock bit
    FCTL1 |= (FWKEY | WRT);
    for (i = 128; i != 0; i--)                // Save Flash
    {
        *Flash_ptrC++ = *Flash_ptrA++;
    }

    Flash_ptrA = (char *) 0x1980;             // Initialize Flash segment A ptr
    Flash_ptrC = (char *) 0x1980;             // Initialize Flash segment D ptr

    if ((*Flash_ptrA == 0)||(*Flash_ptrA == 0x0FF))
    {
        __disable_interrupt();                    // 5xx Workaround: Disable global
                                                  // interrupt while erasing. Re-Enable
                                                  // GIE if needed
        FCTL3 = FWKEY;                            // Clear Lock bit
        FCTL1 = FWKEY+ERASE;                      // Set Erase bit
        *Flash_ptrA = 0;                          // Dummy write to erase Flash seg D
        FCTL1 = FWKEY+WRT;                        // Set WRT bit for write operation

    }
}
