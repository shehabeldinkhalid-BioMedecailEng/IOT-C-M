#ifndef SPI_H_
#define SPI_H_

#include "../../ATmega640_REGS.h"

/* SPI Instance */
#define SPI0    0

/* SPI Mode */
#define SPI_MODE0    0
#define SPI_MODE1    1
#define SPI_MODE2    2
#define SPI_MODE3    3

/* SPI Clock */
#define SPI_DIV2     0
#define SPI_DIV4     1
#define SPI_DIV8     2
#define SPI_DIV16    3
#define SPI_DIV32    4
#define SPI_DIV64    5
#define SPI_DIV128   6

void SPI_InitMaster(uint8_t spi, uint8_t mode, uint8_t clock);

void SPI_Select(uint8_t spi);

void SPI_Deselect(uint8_t spi);

uint8_t SPI_Transfer(uint8_t spi, uint8_t data);

void SPI_SendByte(uint8_t spi, uint8_t data);

uint8_t SPI_ReceiveByte(uint8_t spi);

#endif