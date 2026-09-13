#ifndef PWM_H_
#define PWM_H_

#include "../../../ATmega640_REGS.h"


/* =========================================================
 * PWM TIMERS
 * ========================================================= */

#define PWM_TIMER0    0
#define PWM_TIMER1    1
#define PWM_TIMER2    2
#define PWM_TIMER3    3
#define PWM_TIMER4    4
#define PWM_TIMER5    5


/* =========================================================
 * PWM CHANNELS
 * ========================================================= */

#define PWM_CHANNEL_A    0
#define PWM_CHANNEL_B    1
#define PWM_CHANNEL_C    2


/* =========================================================
 * PWM MODES
 * ========================================================= */

#define PWM_FAST         0
#define PWM_PHASE_CORRECT 1


/* =========================================================
 * PWM OUTPUT MODES
 * ========================================================= */

#define PWM_NON_INVERTING  0
#define PWM_INVERTING      1


/* =========================================================
 * PWM PRESCALERS
 * ========================================================= */

#define PWM_NO_PRESCALER   1
#define PWM_DIV_8          8
#define PWM_DIV_64         64
#define PWM_DIV_256        256
#define PWM_DIV_1024       1024


/* =========================================================
 * PWM STATUS
 * ========================================================= */

#define PWM_OK             0
#define PWM_INVALID_TIMER  1
#define PWM_INVALID_CHANNEL 2
#define PWM_INVALID_DUTY   3


/* =========================================================
 * PWM API
 * ========================================================= */

uint8_t PWM_Init(uint8_t timer,
                 uint8_t channel,
                 uint8_t mode,
                 uint8_t output_mode,
                 uint16_t prescaler);

uint8_t PWM_SetDuty(uint8_t timer,
                    uint8_t channel,
                    uint8_t duty);

uint8_t PWM_Stop(uint8_t timer,
                 uint8_t channel);


#endif