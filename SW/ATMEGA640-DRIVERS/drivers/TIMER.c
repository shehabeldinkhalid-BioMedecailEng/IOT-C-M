#include "TIMER.h"


/* =========================================================
 * TIMER INIT
 * Normal Mode
 * ========================================================= */

void TIMER_Init(uint8_t timer)
{
    switch (timer)
    {
        case TIMER0:

            TCCR0A = 0x00;
            TCCR0B = 0x00;
            TCNT0  = 0x00;

            break;


        case TIMER1:

            TCCR1A = 0x00;
            TCCR1B = 0x00;
            TCCR1C = 0x00;

            TCNT1H = 0x00;
            TCNT1L = 0x00;

            break;


        case TIMER2:

            TCCR2A = 0x00;
            TCCR2B = 0x00;
            TCNT2  = 0x00;

            break;


        case TIMER3:

            TCCR3A = 0x00;
            TCCR3B = 0x00;
            TCCR3C = 0x00;

            TCNT3H = 0x00;
            TCNT3L = 0x00;

            break;


        case TIMER4:

            TCCR4A = 0x00;
            TCCR4B = 0x00;
            TCCR4C = 0x00;

            TCNT4H = 0x00;
            TCNT4L = 0x00;

            break;


        case TIMER5:

            TCCR5A = 0x00;
            TCCR5B = 0x00;
            TCCR5C = 0x00;

            TCNT5H = 0x00;
            TCNT5L = 0x00;

            break;


        default:

            break;
    }
}

void TIMER_Start(uint8_t timer, uint8_t prescaler)
{
	switch (timer)
	{
		case TIMER0:

		TCCR0B &= 0xF8;
		TCCR0B |= prescaler;

		break;


		case TIMER1:

		TCCR1B &= 0xF8;
		TCCR1B |= prescaler;

		break;


		case TIMER2:

		TCCR2B &= 0xF8;
		TCCR2B |= prescaler;

		break;


		case TIMER3:

		TCCR3B &= 0xF8;
		TCCR3B |= prescaler;

		break;


		case TIMER4:

		TCCR4B &= 0xF8;
		TCCR4B |= prescaler;

		break;


		case TIMER5:

		TCCR5B &= 0xF8;
		TCCR5B |= prescaler;

		break;


		default:

		break;
	}
}

void TIMER_Stop(uint8_t timer)
{
	switch (timer)
	{
		case TIMER0:

		TCCR0B &= 0xF8;

		break;


		case TIMER1:

		TCCR1B &= 0xF8;

		break;


		case TIMER2:

		TCCR2B &= 0xF8;

		break;


		case TIMER3:

		TCCR3B &= 0xF8;

		break;


		case TIMER4:

		TCCR4B &= 0xF8;

		break;


		case TIMER5:

		TCCR5B &= 0xF8;

		break;


		default:

		break;
	}
}

/* =========================================================
 * SET COUNTER
 * ========================================================= */

void TIMER_SetCounter(uint8_t timer, uint16_t value)
{
    switch (timer)
    {
        case TIMER0:

            TCNT0 = (uint8_t)value;

            break;

        case TIMER1:

            TCNT1H = (uint8_t)(value >> 8);
            TCNT1L = (uint8_t)value;

            break;

        case TIMER2:

            TCNT2 = (uint8_t)value;

            break;

        case TIMER3:

            TCNT3H = (uint8_t)(value >> 8);
            TCNT3L = (uint8_t)value;

            break;

        case TIMER4:

            TCNT4H = (uint8_t)(value >> 8);
            TCNT4L = (uint8_t)value;

            break;

        case TIMER5:

            TCNT5H = (uint8_t)(value >> 8);
            TCNT5L = (uint8_t)value;

            break;

        default:

            break;
    }
}

/* =========================================================
 * GET COUNTER
 * ========================================================= */

uint16_t TIMER_GetCounter(uint8_t timer)
{
    uint16_t value = 0;

    switch (timer)
    {
        case TIMER0:

            value = TCNT0;

            break;

        case TIMER1:

            value = ((uint16_t)TCNT1H << 8);
            value |= TCNT1L;

            break;

        case TIMER2:

            value = TCNT2;

            break;

        case TIMER3:

            value = ((uint16_t)TCNT3H << 8);
            value |= TCNT3L;

            break;

        case TIMER4:

            value = ((uint16_t)TCNT4H << 8);
            value |= TCNT4L;

            break;

        case TIMER5:

            value = ((uint16_t)TCNT5H << 8);
            value |= TCNT5L;

            break;

        default:

            value = 0;

            break;
    }

    return value;
}

void TIMER_StartCounter(uint8_t timer)
{
    switch (timer)
    {
        case TIMER0:

            /* External clock on T0 (PD7)
             * Falling edge
             */
            TCCR0B &= 0xF8;
            TCCR0B |= 0x06;

            break;

        default:

            break;
    }
}