/*
 * PWM.c
 *
 * Created: 8/31/2026 5:11:08 PM
 *  Author: ENG_shehab eldin
 */ 
#include "PWM.h"


/* =========================================================
 * Internal Helper Functions
 * ========================================================= */

static uint8_t PWM_SetOutputPin(uint8_t timer, uint8_t channel)
{
    switch (timer)
    {
        /* =================================================
         * TIMER 0
         * OC0A -> PB7
         * OC0B -> PG5
         * ================================================= */
        case PWM_TIMER0:

            if (channel == PWM_CHANNEL_A)
            {
                DDRB |= (1 << PB7);
            }
            else if (channel == PWM_CHANNEL_B)
            {
                DDRG |= (1 << PG5);
            }
            else
            {
                return PWM_INVALID_CHANNEL;
            }

            break;


        /* =================================================
         * TIMER 1
         * OC1A -> PB5
         * OC1B -> PB6
         * OC1C -> PB7
         * ================================================= */
        case PWM_TIMER1:

            if (channel == PWM_CHANNEL_A)
            {
                DDRB |= (1 << PB5);
            }
            else if (channel == PWM_CHANNEL_B)
            {
                DDRB |= (1 << PB6);
            }
            else if (channel == PWM_CHANNEL_C)
            {
                DDRB |= (1 << PB7);
            }
            else
            {
                return PWM_INVALID_CHANNEL;
            }

            break;


        /* =================================================
         * TIMER 2
         * OC2A -> PB4
         * OC2B -> PH6
         * ================================================= */
        case PWM_TIMER2:

            if (channel == PWM_CHANNEL_A)
            {
                DDRB |= (1 << PB4);
            }
            else if (channel == PWM_CHANNEL_B)
            {
                DDRH |= (1 << PH6);
            }
            else
            {
                return PWM_INVALID_CHANNEL;
            }

            break;


        /* =================================================
         * TIMER 3
         * OC3A -> PE3
         * OC3B -> PE4
         * OC3C -> PE5
         * ================================================= */
        case PWM_TIMER3:

            if (channel == PWM_CHANNEL_A)
            {
                DDRE |= (1 << PE3);
            }
            else if (channel == PWM_CHANNEL_B)
            {
                DDRE |= (1 << PE4);
            }
            else if (channel == PWM_CHANNEL_C)
            {
                DDRE |= (1 << PE5);
            }
            else
            {
                return PWM_INVALID_CHANNEL;
            }

            break;


        /* =================================================
         * TIMER 4
         * OC4A -> PH3
         * OC4B -> PH4
         * OC4C -> PH5
         * ================================================= */
        case PWM_TIMER4:

            if (channel == PWM_CHANNEL_A)
            {
                DDRH |= (1 << PH3);
            }
            else if (channel == PWM_CHANNEL_B)
            {
                DDRH |= (1 << PH4);
            }
            else if (channel == PWM_CHANNEL_C)
            {
                DDRH |= (1 << PH5);
            }
            else
            {
                return PWM_INVALID_CHANNEL;
            }

            break;


        /* =================================================
         * TIMER 5
         * OC5A -> PL3
         * OC5B -> PL4
         * OC5C -> PL5
         * ================================================= */
        case PWM_TIMER5:

            if (channel == PWM_CHANNEL_A)
            {
                DDRL |= (1 << PL3);
            }
            else if (channel == PWM_CHANNEL_B)
            {
                DDRL |= (1 << PL4);
            }
            else if (channel == PWM_CHANNEL_C)
            {
                DDRL |= (1 << PL5);
            }
            else
            {
                return PWM_INVALID_CHANNEL;
            }

            break;


        default:
            return PWM_INVALID_TIMER;
    }

    return PWM_OK;
}


/* =========================================================
 * Set Clock Prescaler
 * ========================================================= */

static uint8_t PWM_SetPrescaler(uint8_t timer, uint16_t prescaler)
{
    uint8_t cs_bits = 0;

    switch (prescaler)
    {
        case PWM_NO_PRESCALER:
            cs_bits = 1;
            break;

        case PWM_DIV_8:
            cs_bits = 2;
            break;

        case PWM_DIV_64:
            cs_bits = 3;
            break;

        case PWM_DIV_256:
            cs_bits = 4;
            break;

        case PWM_DIV_1024:
            cs_bits = 5;
            break;

        default:
            return PWM_INVALID_TIMER;
    }


    switch (timer)
    {
        case PWM_TIMER0:

            TCCR0B &= ~((1 << CS02) |
                        (1 << CS01) |
                        (1 << CS00));

            TCCR0B |= cs_bits;
            break;


        case PWM_TIMER1:

            TCCR1B &= ~((1 << CS12) |
                        (1 << CS11) |
                        (1 << CS10));

            TCCR1B |= cs_bits;
            break;


        case PWM_TIMER2:

            TCCR2B &= ~((1 << CS22) |
                        (1 << CS21) |
                        (1 << CS20));

            TCCR2B |= cs_bits;
            break;


        case PWM_TIMER3:

            TCCR3B &= ~((1 << CS32) |
                        (1 << CS31) |
                        (1 << CS30));

            TCCR3B |= cs_bits;
            break;


        case PWM_TIMER4:

            TCCR4B &= ~((1 << CS42) |
                        (1 << CS41) |
                        (1 << CS40));

            TCCR4B |= cs_bits;
            break;


        case PWM_TIMER5:

            TCCR5B &= ~((1 << CS52) |
                        (1 << CS51) |
                        (1 << CS50));

            TCCR5B |= cs_bits;
            break;


        default:
            return PWM_INVALID_TIMER;
    }

    return PWM_OK;
}


/* =========================================================
 * PWM Initialization
 * ========================================================= */

uint8_t PWM_Init(uint8_t timer,
                 uint8_t channel,
                 uint8_t mode,
                 uint8_t output_mode,
                 uint16_t prescaler)
{
    uint8_t result;


    /* Validate timer */
    if (timer > PWM_TIMER5)
    {
        return PWM_INVALID_TIMER;
    }


    /* Timer0 and Timer2 don't have Channel C */
    if ((timer == PWM_TIMER0 || timer == PWM_TIMER2) &&
        channel == PWM_CHANNEL_C)
    {
        return PWM_INVALID_CHANNEL;
    }


    /* Validate channel */
    if (channel > PWM_CHANNEL_C)
    {
        return PWM_INVALID_CHANNEL;
    }


    /* Set PWM output pin */
    result = PWM_SetOutputPin(timer, channel);

    if (result != PWM_OK)
    {
        return result;
    }


    /* =====================================================
     * TIMER 0
     * Fast PWM  : WGM02:0 = 011
     * Phase PWM : WGM02:0 = 001
     * ===================================================== */

    if (timer == PWM_TIMER0)
    {
        TCCR0A &= ~((1 << WGM01) |
                    (1 << WGM00));

        TCCR0B &= ~(1 << WGM02);


        if (mode == PWM_FAST)
        {
            TCCR0A |= (1 << WGM01) |
                      (1 << WGM00);
        }
        else
        {
            TCCR0A |= (1 << WGM00);
        }


        if (channel == PWM_CHANNEL_A)
        {
            TCCR0A &= ~((1 << COM0A1) |
                        (1 << COM0A0));

            if (output_mode == PWM_NON_INVERTING)
            {
                TCCR0A |= (1 << COM0A1);
            }
            else
            {
                TCCR0A |= (1 << COM0A1) |
                          (1 << COM0A0);
            }
        }
        else
        {
            TCCR0A &= ~((1 << COM0B1) |
                        (1 << COM0B0));

            if (output_mode == PWM_NON_INVERTING)
            {
                TCCR0A |= (1 << COM0B1);
            }
            else
            {
                TCCR0A |= (1 << COM0B1) |
                          (1 << COM0B0);
            }
        }
    }


    /* =====================================================
     * TIMER 1
     * 8-bit Fast PWM    WGM = 0101
     * 8-bit Phase PWM   WGM = 0001
     * ===================================================== */

    else if (timer == PWM_TIMER1)
    {
        TCCR1A &= ~((1 << WGM11) |
                    (1 << WGM10));

        TCCR1B &= ~((1 << WGM13) |
                    (1 << WGM12));


        if (mode == PWM_FAST)
        {
            TCCR1A |= (1 << WGM10);
            TCCR1B |= (1 << WGM12);
        }
        else
        {
            TCCR1A |= (1 << WGM10);
        }


        if (channel == PWM_CHANNEL_A)
        {
            TCCR1A &= ~((1 << COM1A1) |
                        (1 << COM1A0));

            TCCR1A |= (1 << COM1A1);

            if (output_mode == PWM_INVERTING)
            {
                TCCR1A |= (1 << COM1A0);
            }
        }

        else if (channel == PWM_CHANNEL_B)
        {
            TCCR1A &= ~((1 << COM1B1) |
                        (1 << COM1B0));

            TCCR1A |= (1 << COM1B1);

            if (output_mode == PWM_INVERTING)
            {
                TCCR1A |= (1 << COM1B0);
            }
        }

        else
        {
            TCCR1A &= ~((1 << COM1C1) |
                        (1 << COM1C0));

            TCCR1A |= (1 << COM1C1);

            if (output_mode == PWM_INVERTING)
            {
                TCCR1A |= (1 << COM1C0);
            }
        }
    }


    /* =====================================================
     * TIMER 2
     * ===================================================== */

    else if (timer == PWM_TIMER2)
    {
        TCCR2A &= ~((1 << WGM21) |
                    (1 << WGM20));

        TCCR2B &= ~(1 << WGM22);


        if (mode == PWM_FAST)
        {
            TCCR2A |= (1 << WGM21) |
                      (1 << WGM20);
        }
        else
        {
            TCCR2A |= (1 << WGM20);
        }


        if (channel == PWM_CHANNEL_A)
        {
            TCCR2A &= ~((1 << COM2A1) |
                        (1 << COM2A0));

            TCCR2A |= (1 << COM2A1);

            if (output_mode == PWM_INVERTING)
            {
                TCCR2A |= (1 << COM2A0);
            }
        }
        else
        {
            TCCR2A &= ~((1 << COM2B1) |
                        (1 << COM2B0));

            TCCR2A |= (1 << COM2B1);

            if (output_mode == PWM_INVERTING)
            {
                TCCR2A |= (1 << COM2B0);
            }
        }
    }


    /* =====================================================
     * TIMER 3 / 4 / 5
     * ===================================================== */

    else
    {
        volatile uint8_t *TCCRnA;
        volatile uint8_t *TCCRnB;

        uint8_t com1;
        uint8_t com0;
        uint8_t wgm1;
        uint8_t wgm0;
        uint8_t wgm2;
        uint8_t cs2;
        uint8_t cs1;
        uint8_t cs0;

        if (timer == PWM_TIMER3)
        {
            TCCRnA = &TCCR3A;
            TCCRnB = &TCCR3B;

            if (channel == PWM_CHANNEL_A)
            {
                com1 = COM3A1;
                com0 = COM3A0;
            }
            else if (channel == PWM_CHANNEL_B)
            {
                com1 = COM3B1;
                com0 = COM3B0;
            }
            else
            {
                com1 = COM3C1;
                com0 = COM3C0;
            }

            wgm1 = WGM31;
            wgm0 = WGM30;
            wgm2 = WGM32;

            cs2 = CS32;
            cs1 = CS31;
            cs0 = CS30;
        }

        else if (timer == PWM_TIMER4)
        {
            TCCRnA = &TCCR4A;
            TCCRnB = &TCCR4B;

            if (channel == PWM_CHANNEL_A)
            {
                com1 = COM4A1;
                com0 = COM4A0;
            }
            else if (channel == PWM_CHANNEL_B)
            {
                com1 = COM4B1;
                com0 = COM4B0;
            }
            else
            {
                com1 = COM4C1;
                com0 = COM4C0;
            }

            wgm1 = WGM41;
            wgm0 = WGM40;
            wgm2 = WGM42;

            cs2 = CS42;
            cs1 = CS41;
            cs0 = CS40;
        }

        else
        {
            TCCRnA = &TCCR5A;
            TCCRnB = &TCCR5B;

            if (channel == PWM_CHANNEL_A)
            {
                com1 = COM5A1;
                com0 = COM5A0;
            }
            else if (channel == PWM_CHANNEL_B)
            {
                com1 = COM5B1;
                com0 = COM5B0;
            }
            else
            {
                com1 = COM5C1;
                com0 = COM5C0;
            }

            wgm1 = WGM51;
            wgm0 = WGM50;
            wgm2 = WGM52;

            cs2 = CS52;
            cs1 = CS51;
            cs0 = CS50;
        }


        /* 8-bit Fast PWM */
        *TCCRnA &= ~((1 << wgm1) |
                     (1 << wgm0));

        *TCCRnB &= ~((1 << WGM53) |
                     (1 << wgm2));

        if (mode == PWM_FAST)
        {
            *TCCRnA |= (1 << wgm0);
            *TCCRnB |= (1 << wgm2);
        }
        else
        {
            *TCCRnA |= (1 << wgm0);
        }


        /* Configure output mode */

        *TCCRnA &= ~((1 << com1) |
                     (1 << com0));

        *TCCRnA |= (1 << com1);

        if (output_mode == PWM_INVERTING)
        {
            *TCCRnA |= (1 << com0);
        }


        /* Prescaler */

        *TCCRnB &= ~((1 << cs2) |
                     (1 << cs1) |
                     (1 << cs0));

        switch (prescaler)
        {
            case PWM_NO_PRESCALER:
                *TCCRnB |= (1 << cs0);
                break;

            case PWM_DIV_8:
                *TCCRnB |= (1 << cs1);
                break;

            case PWM_DIV_64:
                *TCCRnB |= (1 << cs1) |
                           (1 << cs0);
                break;

            case PWM_DIV_256:
                *TCCRnB |= (1 << cs2);
                break;

            case PWM_DIV_1024:
                *TCCRnB |= (1 << cs2) |
                           (1 << cs0);
                break;

            default:
                return PWM_INVALID_TIMER;
        }

        return PWM_OK;
    }


    /* Set prescaler for Timer 0/1/2 */
    return PWM_SetPrescaler(timer, prescaler);
}


/* =========================================================
 * PWM Set Duty Cycle
 * ========================================================= */

uint8_t PWM_SetDuty(uint8_t timer,
                    uint8_t channel,
                    uint8_t duty)
{
    uint16_t compare_value;


    if (timer > PWM_TIMER5)
    {
        return PWM_INVALID_TIMER;
    }


    if (channel > PWM_CHANNEL_C)
    {
        return PWM_INVALID_CHANNEL;
    }


    if ((timer == PWM_TIMER0 || timer == PWM_TIMER2) &&
        channel == PWM_CHANNEL_C)
    {
        return PWM_INVALID_CHANNEL;
    }


    if (duty > 100)
    {
        return PWM_INVALID_DUTY;
    }


    /* 0 -> 0
     * 100 -> 255
     */
    compare_value = ((uint16_t)duty * 255U) / 100U;


    switch (timer)
    {
        case PWM_TIMER0:

            if (channel == PWM_CHANNEL_A)
            {
                OCR0A = (uint8_t)compare_value;
            }
            else
            {
                OCR0B = (uint8_t)compare_value;
            }

            break;


        case PWM_TIMER1:

            if (channel == PWM_CHANNEL_A)
            {
                OCR1AH = 0;
                OCR1AL = (uint8_t)compare_value;
            }
            else if (channel == PWM_CHANNEL_B)
            {
                OCR1BH = 0;
                OCR1BL = (uint8_t)compare_value;
            }
            else
            {
                OCR1CH = 0;
                OCR1CL = (uint8_t)compare_value;
            }

            break;


        case PWM_TIMER2:

            if (channel == PWM_CHANNEL_A)
            {
                OCR2A = (uint8_t)compare_value;
            }
            else
            {
                OCR2B = (uint8_t)compare_value;
            }

            break;


        case PWM_TIMER3:

            if (channel == PWM_CHANNEL_A)
            {
                OCR3AH = 0;
                OCR3AL = (uint8_t)compare_value;
            }
            else if (channel == PWM_CHANNEL_B)
            {
                OCR3BH = 0;
                OCR3BL = (uint8_t)compare_value;
            }
            else
            {
                OCR3CH = 0;
                OCR3CL = (uint8_t)compare_value;
            }

            break;


        case PWM_TIMER4:

            if (channel == PWM_CHANNEL_A)
            {
                OCR4AH = 0;
                OCR4AL = (uint8_t)compare_value;
            }
            else if (channel == PWM_CHANNEL_B)
            {
                OCR4BH = 0;
                OCR4BL = (uint8_t)compare_value;
            }
            else
            {
                OCR4CH = 0;
                OCR4CL = (uint8_t)compare_value;
            }

            break;


        case PWM_TIMER5:

            if (channel == PWM_CHANNEL_A)
            {
                OCR5AH = 0;
                OCR5AL = (uint8_t)compare_value;
            }
            else if (channel == PWM_CHANNEL_B)
            {
                OCR5BH = 0;
                OCR5BL = (uint8_t)compare_value;
            }
            else
            {
                OCR5CH = 0;
                OCR5CL = (uint8_t)compare_value;
            }

            break;


        default:
            return PWM_INVALID_TIMER;
    }


    return PWM_OK;
}


/* =========================================================
 * PWM Stop
 * ========================================================= */

uint8_t PWM_Stop(uint8_t timer,
                 uint8_t channel)
{
    if (timer > PWM_TIMER5)
    {
        return PWM_INVALID_TIMER;
    }


    if (channel > PWM_CHANNEL_C)
    {
        return PWM_INVALID_CHANNEL;
    }


    if ((timer == PWM_TIMER0 || timer == PWM_TIMER2) &&
        channel == PWM_CHANNEL_C)
    {
        return PWM_INVALID_CHANNEL;
    }


    switch (timer)
    {
        case PWM_TIMER0:

            if (channel == PWM_CHANNEL_A)
            {
                TCCR0A &= ~((1 << COM0A1) |
                            (1 << COM0A0));
            }
            else
            {
                TCCR0A &= ~((1 << COM0B1) |
                            (1 << COM0B0));
            }

            break;


        case PWM_TIMER1:

            if (channel == PWM_CHANNEL_A)
            {
                TCCR1A &= ~((1 << COM1A1) |
                            (1 << COM1A0));
            }
            else if (channel == PWM_CHANNEL_B)
            {
                TCCR1A &= ~((1 << COM1B1) |
                            (1 << COM1B0));
            }
            else
            {
                TCCR1A &= ~((1 << COM1C1) |
                            (1 << COM1C0));
            }

            break;


        case PWM_TIMER2:

            if (channel == PWM_CHANNEL_A)
            {
                TCCR2A &= ~((1 << COM2A1) |
                            (1 << COM2A0));
            }
            else
            {
                TCCR2A &= ~((1 << COM2B1) |
                            (1 << COM2B0));
            }

            break;


        case PWM_TIMER3:

            if (channel == PWM_CHANNEL_A)
            {
                TCCR3A &= ~((1 << COM3A1) |
                            (1 << COM3A0));
            }
            else if (channel == PWM_CHANNEL_B)
            {
                TCCR3A &= ~((1 << COM3B1) |
                            (1 << COM3B0));
            }
            else
            {
                TCCR3A &= ~((1 << COM3C1) |
                            (1 << COM3C0));
            }

            break;


        case PWM_TIMER4:

            if (channel == PWM_CHANNEL_A)
            {
                TCCR4A &= ~((1 << COM4A1) |
                            (1 << COM4A0));
            }
            else if (channel == PWM_CHANNEL_B)
            {
                TCCR4A &= ~((1 << COM4B1) |
                            (1 << COM4B0));
            }
            else
            {
                TCCR4A &= ~((1 << COM4C1) |
                            (1 << COM4C0));
            }

            break;


        case PWM_TIMER5:

            if (channel == PWM_CHANNEL_A)
            {
                TCCR5A &= ~((1 << COM5A1) |
                            (1 << COM5A0));
            }
            else if (channel == PWM_CHANNEL_B)
            {
                TCCR5A &= ~((1 << COM5B1) |
                            (1 << COM5B0));
            }
            else
            {
                TCCR5A &= ~((1 << COM5C1) |
                            (1 << COM5C0));
            }

            break;
    }


    return PWM_OK;
}