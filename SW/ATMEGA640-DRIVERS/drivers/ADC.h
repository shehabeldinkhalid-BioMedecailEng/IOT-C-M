#ifndef ADC_H_
#define ADC_H_

#include "../../ATmega640_REGS.h"


/* =========================================================
 * ADC CHANNELS
 * ========================================================= */

#define ADC_CHANNEL0      0
#define ADC_CHANNEL1      1
#define ADC_CHANNEL2      2
#define ADC_CHANNEL3      3
#define ADC_CHANNEL4      4
#define ADC_CHANNEL5      5
#define ADC_CHANNEL6      6
#define ADC_CHANNEL7      7

#define ADC_CHANNEL8      8
#define ADC_CHANNEL9      9
#define ADC_CHANNEL10     10
#define ADC_CHANNEL11     11
#define ADC_CHANNEL12     12
#define ADC_CHANNEL13     13
#define ADC_CHANNEL14     14
#define ADC_CHANNEL15     15


/* =========================================================
 * ADC REFERENCE
 * ========================================================= */

#define ADC_REF_AREF      0
#define ADC_REF_AVCC      1
#define ADC_REF_1V1       2
#define ADC_REF_2V56      3


/* =========================================================
 * ADC PRESCALER
 * ========================================================= */

#define ADC_PRESCALER_2       0
#define ADC_PRESCALER_4       1
#define ADC_PRESCALER_8       2
#define ADC_PRESCALER_16      3
#define ADC_PRESCALER_32      4
#define ADC_PRESCALER_64      5
#define ADC_PRESCALER_128     6


/* =========================================================
 * FUNCTIONS
 * ========================================================= */

void ADC_Init(uint8_t reference, uint8_t prescaler);

void ADC_SelectChannel(uint8_t channel);

uint16_t ADC_Read(uint8_t channel);

#endif