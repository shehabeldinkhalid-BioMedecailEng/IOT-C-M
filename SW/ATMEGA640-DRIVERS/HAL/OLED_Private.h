/*
 * OLED_Private.h
 *
 * Created: 8/31/2026 11:00:09 PM
 *  Author: ENG_shehab eldin
 */ 


#ifndef OLED_PRIVATE_H_
#define OLED_PRIVATE_H_

#define OLED_CONTROL_COMMAND    0x00
#define OLED_CONTROL_DATA       0x40

/*
???? ???? ?????? ???? ???? Command ??? Data.
ATmega640
    ?
    ? I²C
    ?
  SH1106
    ?	
    ??? Command 0x00 co ==> 0  ? "???? ????"      
    ?
    ??? Data 0x40  D/c ===> 1    ? "???? ???????? ??"  
*/

#define OLED_DISPLAY_OFF        0xAE
#define OLED_DISPLAY_ON         0xAF

#define OLED_SET_PAGE           0xB0

#define OLED_SET_CONTRAST       0x81



#endif /* OLED_PRIVATE_H_ */