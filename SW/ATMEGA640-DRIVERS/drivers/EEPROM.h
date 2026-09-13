#ifndef EEPROM_H_
#define EEPROM_H_

#include "../../ATmega640_REGS.h"


/* =========================================================
 * EEPROM CONFIGURATION
 * ========================================================= */

#define EEPROM_SIZE         4096U
#define EEPROM_MAX_ADDRESS  (EEPROM_SIZE - 1U)


/* =========================================================
 * EEPROM PROGRAMMING MODES
 * ========================================================= */

#define EEPROM_MODE_ERASE_WRITE   0
#define EEPROM_MODE_ERASE         1
#define EEPROM_MODE_WRITE         2


/* =========================================================
 * EEPROM STATUS
 * ========================================================= */

#define EEPROM_OK                0
#define EEPROM_INVALID_ADDRESS   1


/* =========================================================
 * EEPROM API
 * ========================================================= */

uint8_t EEPROM_WriteByte(uint16_t address, uint8_t data);

uint8_t EEPROM_ReadByte(uint16_t address, uint8_t *data);

uint8_t EEPROM_UpdateByte(uint16_t address, uint8_t data);

uint8_t EEPROM_IsBusy(void);

void EEPROM_WaitReady(void);


#endif