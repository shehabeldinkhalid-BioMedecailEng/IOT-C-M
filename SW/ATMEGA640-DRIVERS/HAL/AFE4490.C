#include "AFE4490.h"

/*
 * AFE4490 configuration used here is the 100-Hz timing set from the
 * TI AFE4490 register examples. It assumes the AFE timing clock and
 * optical hardware are wired consistently with that configuration.
 * SpO2 is calculated from the RED/IR ratio and MUST be calibrated for
 * the actual optical sensor/mechanics before clinical use.
 */

#define AFE4490_SPI_MODE       SPI_MODE0
#define AFE4490_SPI_CLOCK     SPI_DIV16

#define AFE4490_CONTROL0_READ  0x001000UL
#define AFE4490_CONTROL0_RESET 0x000008UL

static DT_int32_t AFE4490_Signed22(DT_uint32_t raw)
{
    DT_uint32_t value = raw & 0x003FFFFFUL;

    if (value & 0x00200000UL)
    {
        value |= 0xFFC00000UL;
    }

    return (DT_int32_t)value;
}

void AFE4490_WriteRegister(DT_uint8_t address, DT_uint32_t data)
{
    SPI_Select(SPI0);
    SPI_Transfer(SPI0, address & 0x7F);
    SPI_Transfer(SPI0, (DT_uint8_t)(data >> 16));
    SPI_Transfer(SPI0, (DT_uint8_t)(data >> 8));
    SPI_Transfer(SPI0, (DT_uint8_t)data);
    SPI_Deselect(SPI0);
}

DT_uint32_t AFE4490_ReadRegister(DT_uint8_t address)
{
    DT_uint32_t data;

    SPI_Select(SPI0);
    SPI_Transfer(SPI0, address & 0x7F);
    data  = ((DT_uint32_t)SPI_Transfer(SPI0, 0x00) << 16);
    data |= ((DT_uint32_t)SPI_Transfer(SPI0, 0x00) << 8);
    data |= (DT_uint32_t)SPI_Transfer(SPI0, 0x00);
    SPI_Deselect(SPI0);

    return data;
}

DT_int32_t AFE4490_ReadSigned22(DT_uint8_t address)
{
    return AFE4490_Signed22(AFE4490_ReadRegister(address));
}

DT_uint8_t AFE4490_DataReady(void)
{
    return ((PIND & (1 << AFE4490_DRDY_PIN)) != 0) ? DT_TRUE : DT_FALSE;
}

static void AFE4490_WriteTimingRegisters(void)
{
    /* TI 100-Hz configuration values. */
    AFE4490_WriteRegister(AFE4490_LED2STC,      0x007580UL);
    AFE4490_WriteRegister(AFE4490_LED2ENDC,     0x009C3EUL);
    AFE4490_WriteRegister(AFE4490_LED2LEDSTC,   0x007530UL);
    AFE4490_WriteRegister(AFE4490_LED2LEDENDC,  0x009C3FUL);
    AFE4490_WriteRegister(AFE4490_ALED2STC,     0x000050UL);
    AFE4490_WriteRegister(AFE4490_ALED2ENDC,    0x00270EUL);

    AFE4490_WriteRegister(AFE4490_LED1STC,      0x002760UL);
    AFE4490_WriteRegister(AFE4490_LED1ENDC,     0x004E1EUL);
    AFE4490_WriteRegister(AFE4490_LED1LEDSTC,   0x002710UL);
    AFE4490_WriteRegister(AFE4490_LED1LEDENDC,  0x004E1FUL);
    AFE4490_WriteRegister(AFE4490_ALED1STC,     0x004E70UL);
    AFE4490_WriteRegister(AFE4490_ALED1ENDC,    0x00752EUL);

    AFE4490_WriteRegister(AFE4490_LED2CONVST,   0x000006UL);
    AFE4490_WriteRegister(AFE4490_LED2CONVEND,  0x00270FUL);
    AFE4490_WriteRegister(AFE4490_ALED2CONVST,  0x002716UL);
    AFE4490_WriteRegister(AFE4490_ALED2CONVEND, 0x004E1FUL);

    AFE4490_WriteRegister(AFE4490_LED1CONVST,   0x004E26UL);
    AFE4490_WriteRegister(AFE4490_LED1CONVEND,  0x00752FUL);
    AFE4490_WriteRegister(AFE4490_ALED1CONVST,  0x007536UL);
    AFE4490_WriteRegister(AFE4490_ALED1CONVEND, 0x009C3FUL);

    AFE4490_WriteRegister(AFE4490_ADCRSTSTCT0,  0x000000UL);
    AFE4490_WriteRegister(AFE4490_ADCRSTENDCT0, 0x000005UL);
    AFE4490_WriteRegister(AFE4490_ADCRSTSTCT1,  0x002710UL);
    AFE4490_WriteRegister(AFE4490_ADCRSTENDCT1, 0x002715UL);
    AFE4490_WriteRegister(AFE4490_ADCRSTSTCT2,  0x004E20UL);
    AFE4490_WriteRegister(AFE4490_ADCRSTENDCT2, 0x004E25UL);
    AFE4490_WriteRegister(AFE4490_ADCRSTSTCT3,  0x007530UL);
    AFE4490_WriteRegister(AFE4490_ADCRSTENDCT3, 0x007535UL);
    AFE4490_WriteRegister(AFE4490_PRPCOUNT,      0x009C3FUL);
}

void AFE4490_Init(void)
{
    /* SPI pins are already owned by the project's SPI driver. */
    DDRD &= ~(1 << AFE4490_DRDY_PIN);
    PORTD &= ~(1 << AFE4490_DRDY_PIN);

    DDRD |= (1 << AFE4490_PWDN_PIN);
    PORTD |= (1 << AFE4490_PWDN_PIN);       /* AFE_PDN is active low */

    /* Software reset. CONTROL0 is write-only. */
    AFE4490_WriteRegister(AFE4490_CONTROL0, AFE4490_CONTROL0_RESET);

    /* Return to write mode while configuring. */
    AFE4490_WriteRegister(AFE4490_CONTROL0, 0x000000UL);

    AFE4490_WriteTimingRegisters();

    /* Conservative analog configuration; optical calibration may require adjustment. */
    AFE4490_WriteRegister(AFE4490_CONTROL1, 0x000107UL);
    AFE4490_WriteRegister(AFE4490_TIAGAIN,   0x00C006UL);
    AFE4490_WriteRegister(AFE4490_TIA_AMB_GAIN, 0x004006UL);
    AFE4490_WriteRegister(AFE4490_LEDCNTRL,  0x011414UL);
    AFE4490_WriteRegister(AFE4490_CONTROL2,  0x020000UL);

    /* Enable register readback. */
    AFE4490_WriteRegister(AFE4490_CONTROL0, AFE4490_CONTROL0_READ);

    /* AFE4490 timing runs from the programmed PRP; there is no START pin. */
}

DT_uint8_t AFE4490_ReadSample(AFE4490_Sample_t* sample)
{
    if (sample == 0)
    {
        return DT_FALSE;
    }

    sample->ir            = AFE4490_ReadSigned22(AFE4490_LED2VAL);
    sample->ir_ambient    = AFE4490_ReadSigned22(AFE4490_ALED2VAL);
    sample->red           = AFE4490_ReadSigned22(AFE4490_LED1VAL);
    sample->red_ambient   = AFE4490_ReadSigned22(AFE4490_ALED1VAL);
    sample->ir_ac         = AFE4490_ReadSigned22(AFE4490_LED2MINUSALED2VAL);
    sample->red_ac        = AFE4490_ReadSigned22(AFE4490_LED1MINUSALED1VAL);
    sample->heart_rate_bpm = 0;
    sample->spo2_x10       = 0;
    sample->valid          = DT_TRUE;

    return DT_TRUE;
}
