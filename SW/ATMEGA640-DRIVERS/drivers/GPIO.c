#include "GPIO.h"

void GPIO_SetDirection(volatile uint8_t *DDR, uint8_t pin, uint8_t direction)
{
	if (direction == GPIO_OUTPUT)
	{
		*DDR |= (1 << pin);
	}
	else
	{
		*DDR &= ~(1 << pin);
	}
}


void GPIO_SetPin(volatile uint8_t *PORT, uint8_t pin)
{
	*PORT |= (1 << pin);
}


void GPIO_ClearPin(volatile uint8_t *PORT, uint8_t pin)
{
	*PORT &= ~(1 << pin);
}


void GPIO_TogglePin(volatile uint8_t *PORT, uint8_t pin)
{
	*PORT ^= (1 << pin);

}


uint8_t GPIO_ReadPin(volatile uint8_t *PIN, uint8_t pin)
{
	return ((*PIN & (1 << pin)) != 0);
}