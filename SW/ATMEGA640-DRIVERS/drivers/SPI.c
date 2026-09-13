#include "SPI.h"

void SPI_InitMaster(uint8_t spi, uint8_t mode, uint8_t clock)
{
    if (spi != SPI0)
        return;

    /* =========================
       SPI PIN CONFIGURATION
       ========================= */

    /* PB0 = SS   -> Output */
    /* PB1 = SCK  -> Output */
    /* PB2 = MOSI -> Output */
    /* PB3 = MISO -> Input */

    DDRB |= (1 << PB0);
    DDRB |= (1 << PB1);
    DDRB |= (1 << PB2);

    DDRB &= ~(1 << PB3);

    /* SS inactive */
    PORTB |= (1 << PB0);


    /* =========================
       SPI CONTROL REGISTER
       ========================= */

    /* Enable SPI
       Master Mode
    */

    SPCR = (1 << SPE) |
           (1 << MSTR);


    /* =========================
       SPI MODE
       ========================= */

    switch (mode)
    {
        case SPI_MODE0:

            SPCR &= ~(1 << CPOL);
            SPCR &= ~(1 << CPHA);

            break;


        case SPI_MODE1:

            SPCR &= ~(1 << CPOL);
            SPCR |=  (1 << CPHA);

            break;


        case SPI_MODE2:

            SPCR |=  (1 << CPOL);
            SPCR &= ~(1 << CPHA);

            break;


        case SPI_MODE3:

            SPCR |= (1 << CPOL);
            SPCR |= (1 << CPHA);

            break;


        default:

            SPCR &= ~(1 << CPOL);
            SPCR &= ~(1 << CPHA);

            break;
    }


    /* =========================
       SPI CLOCK
       ========================= */

    switch (clock)
    {
        case SPI_DIV2:

            SPCR &= ~((1 << SPR1) | (1 << SPR0));
            SPSR |= (1 << SPI2X);

            break;


        case SPI_DIV4:

            SPCR &= ~((1 << SPR1) | (1 << SPR0));
            SPSR &= ~(1 << SPI2X);

            break;


        case SPI_DIV8:

            SPCR &= ~(1 << SPR1);
            SPCR |=  (1 << SPR0);
            SPSR |=  (1 << SPI2X);

            break;


        case SPI_DIV16:

            SPCR &= ~(1 << SPR1);
            SPCR |=  (1 << SPR0);
            SPSR &= ~(1 << SPI2X);

            break;


        case SPI_DIV32:

            SPCR |=  (1 << SPR1);
            SPCR &= ~(1 << SPR0);
            SPSR |= (1 << SPI2X);

            break;


        case SPI_DIV64:

            SPCR |=  (1 << SPR1);
            SPCR &= ~(1 << SPR0);
            SPSR &= ~(1 << SPI2X);

            break;


        case SPI_DIV128:

            SPCR |= (1 << SPR1);
            SPCR |= (1 << SPR0);
            SPSR &= ~(1 << SPI2X);

            break;


        default:

            SPCR &= ~((1 << SPR1) | (1 << SPR0));
            SPSR &= ~(1 << SPI2X);

            break;
    }
}


/* =========================
   SPI SELECT
   ========================= */

void SPI_Select(uint8_t spi)
{
    if (spi != SPI0)
        return;

    PORTB &= ~(1 << PB0);
}


/* =========================
   SPI DESELECT
   ========================= */

void SPI_Deselect(uint8_t spi)
{
    if (spi != SPI0)
        return;

    PORTB |= (1 << PB0);
}


/* =========================
   SPI TRANSFER
   ========================= */

uint8_t SPI_Transfer(uint8_t spi, uint8_t data)
{
    if (spi != SPI0)
        return 0;

    SPDR = data;

    while (!(SPSR & (1 << SPIF)))
    {
    }

    return SPDR;
}


/* =========================
   SPI SEND
   ========================= */

void SPI_SendByte(uint8_t spi, uint8_t data)
{
    SPI_Transfer(spi, data);
}


/* =========================
   SPI RECEIVE
   ========================= */

uint8_t SPI_ReceiveByte(uint8_t spi)
{
    return SPI_Transfer(spi, 0xFF);
}