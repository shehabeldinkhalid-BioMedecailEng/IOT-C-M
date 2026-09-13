#ifndef I2C_H_
#define I2C_H_

#include "../../ATmega640_REGS.h"
#include "../../DataTypes.h"
#define I2C0  0

#define I2C_WRITE  0
#define I2C_READ   1

void I2C_Init(DT_uint8_t i2c, DT_uint32_t clock);

void I2C_Start(DT_uint8_t i2c);

void I2C_Stop(DT_uint8_t i2c);

DT_uint8_t I2C_Write(DT_uint8_t i2c,DT_uint8_t data);

DT_uint8_t I2C_ReadACK(DT_uint8_t i2c);

DT_uint8_t I2C_ReadNACK(DT_uint8_t i2c);

#endif