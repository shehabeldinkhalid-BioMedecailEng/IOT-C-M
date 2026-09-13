#include "ADC.h"


void ADC_Init(uint8_t reference, uint8_t prescaler)
{
    /* Disable ADC first */
    ADCSRA = 0x00;

    /* =========================================
       Voltage Reference
       ========================================= */

    switch (reference)
    {
        case ADC_REF_AREF:

            ADMUX &= ~((1 << REFS1) | (1 << REFS0));

            break;


        case ADC_REF_AVCC:

            ADMUX &= ~(1 << REFS1);
            ADMUX |=  (1 << REFS0);

            break;


        case ADC_REF_1V1:

            ADMUX |= (1 << REFS1);
            ADMUX |= (1 << REFS0);

            break;


        case ADC_REF_2V56:

            ADMUX |= (1 << REFS1);
            ADMUX |= (1 << REFS0);

            break;


        default:

            ADMUX |= (1 << REFS0);

            break;
    }


    /* =========================================
       Right Adjust Result
       ========================================= */

    ADMUX &= ~(1 << ADLAR);


    /* =========================================
       Prescaler
       ========================================= */

    switch (prescaler)
    {
        case ADC_PRESCALER_2:

            ADCSRA &= ~((1 << ADPS2) |
                        (1 << ADPS1) |
                        (1 << ADPS0));

            break;


        case ADC_PRESCALER_4:

            ADCSRA &= ~(1 << ADPS2);
            ADCSRA |=  (1 << ADPS1);
            ADCSRA &= ~(1 << ADPS0);

            break;


        case ADC_PRESCALER_8:

            ADCSRA &= ~(1 << ADPS2);
            ADCSRA |=  (1 << ADPS1);
            ADCSRA |=  (1 << ADPS0);

            break;


        case ADC_PRESCALER_16:

            ADCSRA |=  (1 << ADPS2);
            ADCSRA &= ~(1 << ADPS1);
            ADCSRA &= ~(1 << ADPS0);

            break;


        case ADC_PRESCALER_32:

            ADCSRA |=  (1 << ADPS2);
            ADCSRA &= ~(1 << ADPS1);
            ADCSRA |=  (1 << ADPS0);

            break;


        case ADC_PRESCALER_64:

            ADCSRA |=  (1 << ADPS2);
            ADCSRA |=  (1 << ADPS1);
            ADCSRA &= ~(1 << ADPS0);

            break;


        case ADC_PRESCALER_128:

            ADCSRA |=  (1 << ADPS2);
            ADCSRA |=  (1 << ADPS1);
            ADCSRA |=  (1 << ADPS0);

            break;


        default:

            ADCSRA |=  (1 << ADPS2) |
                       (1 << ADPS1) |
                       (1 << ADPS0);

            break;
    }


    /* =========================================
       Disable Auto Trigger
       ========================================= */

    ADCSRA &= ~(1 << ADATE);


    /* =========================================
       Enable ADC
       ========================================= */

    ADCSRA |= (1 << ADEN);
}

void ADC_SelectChannel(uint8_t channel)
{
	/* Keep only valid 0-15 */
	if (channel > 15)
	return;


	/* MUX5 */
	if (channel & 0x20)
	ADCSRB |= (1 << MUX5);
	else
	ADCSRB &= ~(1 << MUX5);


	/* MUX4:MUX0 */

	ADMUX &= 0xE0;

	ADMUX |= (channel & 0x1F);
}

uint16_t ADC_Read(uint8_t channel)
{
    uint16_t result;


    /* Select channel */
    ADC_SelectChannel(channel);


    /* Start conversion */
    ADCSRA |= (1 << ADSC);


    /* Wait until conversion complete */
    while (!(ADCSRA & (1 << ADIF)))
    {
    }


    /* Clear ADC flag */
    ADCSRA |= (1 << ADIF);


    /* Read ADC result
       ADCL must be read first
       then ADCH
    */

    result = ADCL;

    result |= ((uint16_t)ADCH << 8);


    return result;
}
