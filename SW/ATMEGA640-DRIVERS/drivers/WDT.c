/*
 * WDT.c
 *
 * Created: 8/31/2026 3:47:50 PM
 *  Author: ENG_shehab eldin
 */ 

#include "WDT.h"


void WDT_Enable(uint8_t timeout)
{
	uint8_t wdt_value;

	wdt_value = timeout & 0x07;

	if (timeout >= 8)
	{
		wdt_value |= (1 << WDP3);
	}

	/* Timed sequence */
	WDTCSR |= (1 << WDCE) | (1 << WDE);

	/* Enable WDT + set prescaler */
	WDTCSR = (1 << WDE) | wdt_value;
}


void WDT_Disable(void)
{
	/* Timed sequence */
	WDTCSR |= (1 << WDCE) | (1 << WDE);

	WDTCSR = 0x00;
}


void WDT_Reset(void)
{
	__asm__ __volatile__ ("wdr");
}