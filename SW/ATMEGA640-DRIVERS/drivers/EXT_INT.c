#include "EXT_INT.h"


void EXT_INT_Init(uint8_t int_num, uint8_t mode)
{
    uint8_t shift;


    /* -----------------------------------------------------
     * INT0 ~ INT3
     * ----------------------------------------------------- */

    if (int_num <= INT_3)
    {
        shift = int_num * 2;

        /* Disable interrupt before changing sense control */
        EIMSK &= ~(1 << int_num);

        /* Clear old configuration */
        EICRA &= ~(3 << shift);

        /* Set new mode */
        EICRA |= (mode << shift);

        /* Clear interrupt flag */
        EIFR |= (1 << int_num);
    }


    /* -----------------------------------------------------
     * INT4 ~ INT7
     * ----------------------------------------------------- */

    else if (int_num <= INT_7)
    {
        shift = (int_num - 4) * 2;

        /* Disable interrupt before changing sense control */
        EIMSK &= ~(1 << int_num);

        /* Clear old configuration */
        EICRB &= ~(3 << shift);

        /* Set new mode */
        EICRB |= (mode << shift);

        /* Clear interrupt flag */
        EIFR |= (1 << int_num);
    }
}


void EXT_INT_Enable(uint8_t int_num)
{
    if (int_num <= INT_7)
    {
        EIMSK |= (1 << int_num);
    }
}


void EXT_INT_Disable(uint8_t int_num)
{
    if (int_num <= INT_7)
    {
        EIMSK &= ~(1 << int_num);
    }
}


void EXT_INT_ClearFlag(uint8_t int_num)
{
    if (int_num <= INT_7)
    {
        /* Writing 1 clears the flag */
        EIFR |= (1 << int_num);
    }
}