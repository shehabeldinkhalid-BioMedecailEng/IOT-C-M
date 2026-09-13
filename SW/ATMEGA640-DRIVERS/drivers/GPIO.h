#ifndef GPIO_H_
#define GPIO_H_

#include "../../ATmega640_REGS.h"

#define GPIO_OUTPUT 1
#define GPIO_INPUT  0

void GPIO_SetDirection(volatile uint8_t *DDR, uint8_t pin, uint8_t direction);
void GPIO_SetPin(volatile uint8_t *PORT, uint8_t pin);
void GPIO_ClearPin(volatile uint8_t *PORT, uint8_t pin);
void GPIO_TogglePin(volatile uint8_t *PORT, uint8_t pin);
uint8_t GPIO_ReadPin(volatile uint8_t *PIN, uint8_t pin);

#endif