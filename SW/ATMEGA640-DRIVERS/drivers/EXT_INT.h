#ifndef EXT_INT_H_
#define EXT_INT_H_

#include "../../ATmega640_REGS.h"


/* =========================================================
 * INTERRUPT NUMBERS
 * ========================================================= */

#define INT_0   0
#define INT_1   1
#define INT_2   2
#define INT_3   3
#define INT_4   4
#define INT_5   5
#define INT_6   6
#define INT_7   7


/* =========================================================
 * INTERRUPT SENSE MODE
 * ========================================================= */

#define LOW_LEVEL       0
#define LOGICAL_CHANGE  1
#define FALLING_EDGE    2
#define RISING_EDGE     3


void EXT_INT_Init(uint8_t int_num, uint8_t mode);

void EXT_INT_Enable(uint8_t int_num);

void EXT_INT_Disable(uint8_t int_num);

void EXT_INT_ClearFlag(uint8_t int_num);


#endif