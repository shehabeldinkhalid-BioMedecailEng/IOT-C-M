#ifndef OLED_FONT_H_
#define OLED_FONT_H_

#include "../../DataTypes.h"

#define OLED_FONT_WIDTH      5
#define OLED_FONT_SPACING    1

typedef struct
{
	char Character;

	DT_uint8_t Bitmap[OLED_FONT_WIDTH];

} OLED_FontChar_t;

#define OLED_LARGE_FONT_WIDTH    16
#define OLED_LARGE_FONT_PAGES    3

typedef struct
{
	char Character;

	DT_uint8_t Bitmap[OLED_LARGE_FONT_PAGES]
	[OLED_LARGE_FONT_WIDTH];

} OLED_LargeFontChar_t;


extern const OLED_FontChar_t OLED_Font[];

extern const DT_uint8_t OLED_Font_Size;

extern const OLED_LargeFontChar_t OLED_LargeFont[];

extern const DT_uint8_t OLED_LargeFont_Size;

#endif /* OLED_FONT_H_ */