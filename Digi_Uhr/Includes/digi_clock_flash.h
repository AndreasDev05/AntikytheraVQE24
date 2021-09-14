/*
 * digi_clock_flash.h
 *
 *  Created on: 19.02.2021
 *      Author: andi
 */

#include <stdbool.h>
#include <stdint.h>
#include <msp430.h>

#ifndef INCLUDES_DIGI_CLOCK_FLASH_H_
#define INCLUDES_DIGI_CLOCK_FLASH_H_

void FlashWrite(void);

void FlashAtomWrite(void);

void FlashDeleteBlock(void);

void InitialVarWithFlash(void);

#endif /* INCLUDES_DIGI_CLOCK_FLASH_H_ */
