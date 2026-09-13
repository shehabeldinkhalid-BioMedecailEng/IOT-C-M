#ifndef OLED_H_
#define OLED_H_

#include "../../DataTypes.h"
#include "../../DRIVERS/I2C/I2C.h"

#include "OLED_Config.h"
#include "OLED_Private.h"
#include "OLED_Font.h"


void OLED_Init(void);

void OLED_Clear(void);

void OLED_SetCursor(DT_uint8_t Page, DT_uint8_t Column);

void OLED_SendCommand(DT_uint8_t Command);

void OLED_SendData(DT_uint8_t Data);

void OLED_WriteChar(char Character);

void OLED_WriteString(const char* String);

void OLED_WriteTemperature(DT_uint16_t Temperature);


#endif /* OLED_H_ */