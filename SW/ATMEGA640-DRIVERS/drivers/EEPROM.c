/*
 * EEPROM.c
 *
 * Created: 8/31/2026 4:55:18 PM
 *  Author: ENG_shehab eldin
 */ 

#include "EEPROM.h"
#include <avr/interrupt.h>


/* =========================================================
 * EEPROM - Wait Until Ready
 * ========================================================= */

void EEPROM_WaitReady(void)
{
    while (EECR & (1 << EEPE))
    {
        /* Wait */
    }
}


/* =========================================================
 * EEPROM - Check Busy
 * ========================================================= */

uint8_t EEPROM_IsBusy(void)
{
    if (EECR & (1 << EEPE))
    {
        return 1;
    }

    return 0;
}


/* =========================================================
 * EEPROM - Write Byte
 * ========================================================= */

uint8_t EEPROM_WriteByte(uint16_t address, uint8_t data)
{
    uint8_t sreg;

    /* Check address */
    if (address > EEPROM_MAX_ADDRESS)
    {
        return EEPROM_INVALID_ADDRESS;
    }

    /* Wait for previous write to finish */
    EEPROM_WaitReady();

    /* Save interrupt state */
    sreg = SREG;

    /* Disable global interrupts */
    cli();

    /* Set EEPROM address */
    EEARL = (uint8_t)(address & 0xFF);
    EEARH = (uint8_t)((address >> 8) & 0x0F);

    /* Set EEPROM data */
    EEDR = data;

    /*
     * Master Write Enable
     */
    EECR |= (1 << EEMPE);

    /*
     * Start EEPROM Write
     * Must happen within 4 CPU cycles
     */
    EECR |= (1 << EEPE);

    /* Restore interrupt state */
    SREG = sreg;

    return EEPROM_OK;
}


/* =========================================================
 * EEPROM - Read Byte
 * ========================================================= */

uint8_t EEPROM_ReadByte(uint16_t address, uint8_t *data)
{
    if (address > EEPROM_MAX_ADDRESS)
    {
        return EEPROM_INVALID_ADDRESS;
    }

    /* Wait for previous write to finish */
    EEPROM_WaitReady();

    /* Set EEPROM address */
    EEARL = (uint8_t)(address & 0xFF);
    EEARH = (uint8_t)((address >> 8) & 0x0F);

    /* Start EEPROM Read */
    EECR |= (1 << EERE);

    /* Read data */
    *data = EEDR;

    return EEPROM_OK;
}


/* =========================================================
 * EEPROM - Update Byte
 * ========================================================= */

uint8_t EEPROM_UpdateByte(uint16_t address, uint8_t data)
{
    uint8_t current_data;
    uint8_t status;

    /* Read current EEPROM value */
    status = EEPROM_ReadByte(address, &current_data);

    if (status != EEPROM_OK)
    {
        return status;
    }

    /* Write only if data is different */
    if (current_data != data)
    {
        return EEPROM_WriteByte(address, data);
    }

    return EEPROM_OK;
}