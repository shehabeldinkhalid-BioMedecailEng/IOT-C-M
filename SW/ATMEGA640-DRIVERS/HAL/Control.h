#ifndef CONTROL_H_
#define CONTROL_H_

#include "../../DataTypes.h"
#include "../../ATmega640_REGS.h"

void Buzzer_Init(DT_uint8_t Pin);
void Buzzer_On(DT_uint8_t Pin);
void Buzzer_Off(DT_uint8_t Pin);
void Buzzer_Toggle(DT_uint8_t Pin);

void LED_Init(DT_uint8_t Pin);
void LED_On(DT_uint8_t Pin);
void LED_Off(DT_uint8_t Pin);
void LED_Toggle(DT_uint8_t Pin);

#endif