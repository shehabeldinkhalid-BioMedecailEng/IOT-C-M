#ifndef AFE4490_H_
#define AFE4490_H_

#include "../../DataTypes.h"
#include "../../DRIVERS/SPI/SPI.h"
#include "../../ATmega640_REGS.h"
#include "AFE4490_REG.h"

/* AFE4490 control pins used by this project.
 * LED_ALM, PD_ALM/BD_ALM and DIAG_END are intentionally NOT used.
 */
#define AFE4490_CS_PIN       PB0
#define AFE4490_DRDY_PIN     PD2
#define AFE4490_PWDN_PIN     PD4

#define AFE4490_SAMPLE_RATE_HZ 100U

/* This is the project-level sample record, not an AFE register. */
typedef struct
{
    DT_int32_t ir;
    DT_int32_t ir_ambient;
    DT_int32_t red;
    DT_int32_t red_ambient;
    DT_int32_t ir_ac;
    DT_int32_t red_ac;
    DT_uint16_t heart_rate_bpm;
    DT_uint16_t spo2_x10;
    DT_uint8_t valid;
} AFE4490_Sample_t;

void AFE4490_Init(void);
void AFE4490_WriteRegister(DT_uint8_t address, DT_uint32_t data);
DT_uint32_t AFE4490_ReadRegister(DT_uint8_t address);
DT_int32_t AFE4490_ReadSigned22(DT_uint8_t address);
DT_uint8_t AFE4490_DataReady(void);
DT_uint8_t AFE4490_ReadSample(AFE4490_Sample_t* sample);

#endif
