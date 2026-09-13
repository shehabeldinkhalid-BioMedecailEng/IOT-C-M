#include "I2C.h"

#define F_CPU 16000000UL

void I2C_Init(uint8_t i2c, uint32_t clock)
{
	uint32_t twbr_value;

	if (i2c != I2C0)
	return;

	twbr_value = ((F_CPU / clock) - 16UL) / 2UL;

	TWBR0 = (uint8_t)twbr_value;

	TWSR0 = 0x00;

	TWCR0 = (1 << TWEN0);
}


void I2C_Start(uint8_t i2c)
{
	if (i2c != I2C0)
	return;

	TWCR0 = (1 << TWINT0) |
	(1 << TWSTA0) |
	(1 << TWEN0);

	while (!(TWCR0 & (1 << TWINT0)))
	{
	}
}


void I2C_Stop(uint8_t i2c)
{
	if (i2c != I2C0)
	return;

	TWCR0 = (1 << TWINT0) |
	(1 << TWSTO0) |
	(1 << TWEN0);
}


uint8_t I2C_Write(uint8_t i2c, uint8_t data)
{
	if (i2c != I2C0)
	return 0;

	TWDR0 = data;

	TWCR0 = (1 << TWINT0) |
	(1 << TWEN0);

	while (!(TWCR0 & (1 << TWINT0)))
	{
	}

	return 1;
}


uint8_t I2C_ReadACK(uint8_t i2c)
{
	if (i2c != I2C0)
	return 0;

	TWCR0 = (1 << TWINT0) |
	(1 << TWEA0) |
	(1 << TWEN0);

	while (!(TWCR0 & (1 << TWINT0)))
	{
	}

	return TWDR0;
}


uint8_t I2C_ReadNACK(uint8_t i2c)
{
	if (i2c != I2C0)
	return 0;

	TWCR0 = (1 << TWINT0) |
	(1 << TWEN0);

	while (!(TWCR0 & (1 << TWINT0)))
	{
	}

	return TWDR0;
}