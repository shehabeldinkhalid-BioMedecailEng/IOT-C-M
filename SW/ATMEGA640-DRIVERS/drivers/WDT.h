#ifndef WDT_H_
#define WDT_H_

#include "../../ATmega640_REGS.h"


/* =========================================================
 * WDT TIMEOUT
 * ========================================================= */

#define WDT_16MS     0
#define WDT_32MS     1
#define WDT_64MS     2
#define WDT_125MS    3
#define WDT_250MS    4
#define WDT_500MS    5
#define WDT_1S       6
#define WDT_2S       7
#define WDT_4S       8
#define WDT_8S       9


void WDT_Enable(uint8_t timeout);

void WDT_Disable(void);

void WDT_Reset(void);


#endif