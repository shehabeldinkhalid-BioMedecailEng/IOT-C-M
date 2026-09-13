#include "../../ATmega640_REGS.h"
#include "../../DataTypes.h"

#include "OLED.h"
#include "OLED_Config.h"
#include "OLED_Font.h"


static const OLED_FontChar_t* OLED_FindCharacter(char Character)
{
	DT_uint8_t i;

	for (i = 0; i < OLED_Font_Size; i++)
	{
		if (OLED_Font[i].Character == Character)
		{
			return &OLED_Font[i];
		}
	}

	return 0;
}


void OLED_SendCommand(DT_uint8_t Command)
{
	I2C_Start(I2C0);

	I2C_Write(I2C0, OLED_I2C_ADDRESS);

	I2C_Write(I2C0, OLED_CONTROL_COMMAND);

	I2C_Write(I2C0, Command);

	I2C_Stop(I2C0);
}


void OLED_SendData(DT_uint8_t Data)
{
	I2C_Start(I2C0);

	I2C_Write(I2C0, OLED_I2C_ADDRESS);

	I2C_Write(I2C0, OLED_CONTROL_DATA);

	I2C_Write(I2C0, Data);

	I2C_Stop(I2C0);
}


void OLED_Init(void)
{
	OLED_SendCommand(0xAE);

	OLED_SendCommand(0xD5);
	OLED_SendCommand(0x80);

	OLED_SendCommand(0xA8);
	OLED_SendCommand(0x3F);

	OLED_SendCommand(0xD3);
	OLED_SendCommand(0x00);

	OLED_SendCommand(0x40);

	OLED_SendCommand(0x8D);
	OLED_SendCommand(0x14);

	OLED_SendCommand(0x20);
	OLED_SendCommand(0x00);

	OLED_SendCommand(0xA1);

	OLED_SendCommand(0xC8);

	OLED_SendCommand(0xDA);
	OLED_SendCommand(0x12);

	OLED_SendCommand(0x81);
	OLED_SendCommand(0x7F);

	OLED_SendCommand(0xD9);
	OLED_SendCommand(0xF1);

	OLED_SendCommand(0xDB);
	OLED_SendCommand(0x40);

	OLED_SendCommand(0xA6);

	OLED_SendCommand(0xAF);
}


void OLED_Clear(void)
{
	DT_uint8_t Page;
	DT_uint8_t Column;

	for (Page = 0; Page < OLED_PAGES; Page++)
	{
		OLED_SendCommand(0xB0 | Page);

		OLED_SendCommand(
		0x00 | (OLED_COLUMN_OFFSET & 0x0F)
		);

		OLED_SendCommand(
		0x10 | ((OLED_COLUMN_OFFSET >> 4) & 0x0F)
		);

		I2C_Start(I2C0);

		I2C_Write(I2C0, OLED_I2C_ADDRESS);

		I2C_Write(I2C0, OLED_CONTROL_DATA);

		for (Column = 0; Column < OLED_WIDTH; Column++)
		{
			I2C_Write(I2C0, 0x00);
		}

		I2C_Stop(I2C0);
	}
}


void OLED_SetCursor(DT_uint8_t Page, DT_uint8_t Column)
{
	OLED_SendCommand(0xB0 | Page);

	OLED_SendCommand(
	0x00 | (Column & 0x0F)
	);

	OLED_SendCommand(
	0x10 | ((Column >> 4) & 0x0F)
	);
}


void OLED_WriteChar(char Character)
{
	DT_uint8_t i;

	const OLED_FontChar_t* FontCharacter;

	FontCharacter = OLED_FindCharacter(Character);

	if (FontCharacter == 0)
	{
		return;
	}

	for (i = 0; i < OLED_FONT_WIDTH; i++)
	{
		OLED_SendData(FontCharacter->Bitmap[i]);
	}

	OLED_SendData(0x00);
}


void OLED_WriteString(const char* String)
{
	if (String == 0)
	{
		return;
	}

	while (*String != '\0')
	{
		OLED_WriteChar(*String);

		String++;
	}
}