#include "Control.h"

void Buzzer_Init(DT_uint8_t Pin)
{
	DDRB |= (1 << Pin);
	PORTB &= ~(1 << Pin);
}

void Buzzer_On(DT_uint8_t Pin)
{
	PORTB |= (1 << Pin);
}

void Buzzer_Off(DT_uint8_t Pin)
{
	PORTB &= ~(1 << Pin);
}

void Buzzer_Toggle(DT_uint8_t Pin)
{
	PORTB ^= (1 << Pin);
}


void LED_Init(DT_uint8_t Pin)
{
	DDRB |= (1 << Pin);
	PORTB &= ~(1 << Pin);
}

void LED_On(DT_uint8_t Pin)
{
	PORTB |= (1 << Pin);
}

void LED_Off(DT_uint8_t Pin)
{
	PORTB &= ~(1 << Pin);
}

void LED_Toggle(DT_uint8_t Pin)
{
	PORTB ^= (1 << Pin);
}