#ifndef TIMER_H_
#define TIMER_H_

#include "../../ATmega640_REGS.h"


/* =========================================================
 * TIMER TYPES
 * ========================================================= */

#define TIMER0      0
#define TIMER1      1
#define TIMER2      2
#define TIMER3      3
#define TIMER4      4
#define TIMER5      5


/* =========================================================
 * TIMER MODES
 * ========================================================= */

#define TIMER_NORMAL_MODE       0
#define TIMER_CTC_MODE          1
#define TIMER_FAST_PWM_MODE     2


/* =========================================================
 * PRESCALER
 * ========================================================= */

#define TIMER_NO_CLOCK          0
#define TIMER_PRESCALER_1       1
#define TIMER_PRESCALER_8       2
#define TIMER_PRESCALER_64      3
#define TIMER_PRESCALER_256     4
#define TIMER_PRESCALER_1024    5


/* =========================================================
 * FUNCTIONS
 * ========================================================= */

void TIMER_Init(uint8_t timer);

void TIMER_Start(uint8_t timer, uint8_t prescaler);

void TIMER_Stop(uint8_t timer);

void TIMER_SetCounter(uint8_t timer, uint16_t value);

uint16_t TIMER_GetCounter(uint8_t timer);

void TIMER_SetCompareA(uint8_t timer, uint16_t value);

void TIMER_SetCompareB(uint8_t timer, uint16_t value);
void TIMER_StartCounter(uint8_t timer);

#endif