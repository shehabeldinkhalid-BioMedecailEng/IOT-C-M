#ifndef ATMEGA640_REGS_H_
#define ATMEGA640_REGS_H_

#include <stdint.h>

/* =========================================================
 * Generic Register Access
 * ========================================================= */

#define REG8(address)   (*(volatile uint8_t *)(address))
#define REG16(address)  (*(volatile uint16_t *)(address))

#define BIT(bit)         (1U << (bit))


/* =========================================================
 * GPIO - PORT A
 * ========================================================= */

#define PINA    REG8(0x20)
#define DDRA    REG8(0x21)
#define PORTA   REG8(0x22)

/* =========================================================
 * GPIO - PORT B
 * ========================================================= */

#define PINB    REG8(0x23)
#define DDRB    REG8(0x24)
#define PORTB   REG8(0x25)

/* =========================================================
 * GPIO - PORT C
 * ========================================================= */

#define PINC    REG8(0x26)
#define DDRC    REG8(0x27)
#define PORTC   REG8(0x28)

/* =========================================================
 * GPIO - PORT D
 * ========================================================= */

#define PIND    REG8(0x29)
#define DDRD    REG8(0x2A)
#define PORTD   REG8(0x2B)

/* =========================================================
 * GPIO - PORT E
 * ========================================================= */

#define PINE    REG8(0x2C)
#define DDRE    REG8(0x2D)
#define PORTE   REG8(0x2E)

/* =========================================================
 * GPIO - PORT F
 * ========================================================= */

#define PINF    REG8(0x2F)
#define DDRF    REG8(0x30)
#define PORTF   REG8(0x31)

/* =========================================================
 * GPIO - PORT G
 * ========================================================= */

#define PING    REG8(0x32)
#define DDRG    REG8(0x33)
#define PORTG   REG8(0x34)

/* =========================================================
 * GPIO - PORT H
 * ========================================================= */

#define PINH    REG8(0x100)
#define DDRH    REG8(0x101)
#define PORTH   REG8(0x102)

/* =========================================================
 * GPIO - PORT J
 * ========================================================= */

#define PINJ    REG8(0x103)
#define DDRJ    REG8(0x104)
#define PORTJ   REG8(0x105)

/* =========================================================
 * GPIO - PORT K
 * ========================================================= */

#define PINK    REG8(0x106)
#define DDRK    REG8(0x107)
#define PORTK   REG8(0x108)

/* =========================================================
 * GPIO - PORT L
 * ========================================================= */

#define PINL    REG8(0x109)
#define DDRL    REG8(0x10A)
#define PORTL   REG8(0x10B)


/* =========================================================
 * GPIO Pin Numbers
 * ========================================================= */

#define PIN0    0
#define PIN1    1
#define PIN2    2
#define PIN3    3
#define PIN4    4
#define PIN5    5
#define PIN6    6
#define PIN7    7

#define PA0     0
#define PA1     1
#define PA2     2
#define PA3     3
#define PA4     4
#define PA5     5
#define PA6     6
#define PA7     7

#define PB0     0
#define PB1     1
#define PB2     2
#define PB3     3
#define PB4     4
#define PB5     5
#define PB6     6
#define PB7     7

#define PC0     0
#define PC1     1
#define PC2     2
#define PC3     3
#define PC4     4
#define PC5     5
#define PC6     6
#define PC7     7

#define PD0     0
#define PD1     1
#define PD2     2
#define PD3     3
#define PD4     4
#define PD5     5
#define PD6     6
#define PD7     7

#define PE0     0
#define PE1     1
#define PE2     2
#define PE3     3
#define PE4     4
#define PE5     5
#define PE6     6
#define PE7     7

#define PF0     0
#define PF1     1
#define PF2     2
#define PF3     3
#define PF4     4
#define PF5     5
#define PF6     6
#define PF7     7

#define PG0     0
#define PG1     1
#define PG2     2
#define PG3     3
#define PG4     4
#define PG5     5

#define PH0     0
#define PH1     1
#define PH2     2
#define PH3     3
#define PH4     4
#define PH5     5
#define PH6     6
#define PH7     7

#define PJ0     0
#define PJ1     1
#define PJ2     2
#define PJ3     3
#define PJ4     4
#define PJ5     5
#define PJ6     6
#define PJ7     7

#define PK0     0
#define PK1     1
#define PK2     2
#define PK3     3
#define PK4     4
#define PK5     5
#define PK6     6
#define PK7     7

#define PL0     0
#define PL1     1
#define PL2     2
#define PL3     3
#define PL4     4
#define PL5     5
#define PL6     6
#define PL7     7


/* =========================================================
 * USART 0
 * ========================================================= */

#define UCSR0A  REG8(0xC0)
#define UCSR0B  REG8(0xC1)
#define UCSR0C  REG8(0xC2)
#define UBRR0L  REG8(0xC4)
#define UBRR0H  REG8(0xC5)
#define UDR0    REG8(0xC6)

/* USART0 bits */

#define RXC0    7
#define TXC0    6
#define UDRE0   5
#define FE0     4
#define DOR0    3
#define UPE0    2
#define U2X0    1
#define MPCM0   0

#define RXCIE0  7
#define TXCIE0  6
#define UDRIE0  5
#define RXEN0   4
#define TXEN0   3
#define UCSZ02  2
#define RXB80   1
#define TXB80   0

#define UMSEL01 7
#define UMSEL00 6
#define UPM01   5
#define UPM00   4
#define USBS0   3
#define UCSZ01  2
#define UCSZ00  1
#define UCPOL0  0


/* =========================================================
 * USART 1
 * ========================================================= */

#define UCSR1A  REG8(0xC8)
#define UCSR1B  REG8(0xC9)
#define UCSR1C  REG8(0xCA)
#define UBRR1L  REG8(0xCC)
#define UBRR1H  REG8(0xCD)
#define UDR1    REG8(0xCE)


/* =========================================================
 * USART 2
 * ========================================================= */

#define UCSR2A  REG8(0xD0)
#define UCSR2B  REG8(0xD1)
#define UCSR2C  REG8(0xD2)
#define UBRR2L  REG8(0xD4)
#define UBRR2H  REG8(0xD5)
#define UDR2    REG8(0xD6)


/* =========================================================
 * USART 3
 * ========================================================= */

#define UCSR3A  REG8(0x130)
#define UCSR3B  REG8(0x131)
#define UCSR3C  REG8(0x132)
#define UBRR3L  REG8(0x134)
#define UBRR3H  REG8(0x135)
#define UDR3    REG8(0x136)


/* =========================================================
 * SPI
 * ========================================================= */

#define SPCR0   REG8(0x4C)
#define SPSR0   REG8(0x4D)
#define SPDR0   REG8(0x4E)

/* SPI bits */

#define SPIE0   7
#define SPE0    6
#define DORD0   5
#define MSTR0   4
#define CPOL0   3
#define CPHA0   2
#define SPR01   1
#define SPR00   0

#define SPIF0   7
#define WCOL0   6
#define SPI2X0  0

/* =========================================================
 * SPI REGISTERS - ATmega640
 * ========================================================= */

/* SPI Registers */
#define SPCR    REG8(0x4C)
#define SPSR    REG8(0x4D)
#define SPDR    REG8(0x4E)


/* =========================================================
 * SPCR - SPI Control Register
 * ========================================================= */

#define SPIE    7
#define SPE     6
#define DORD    5
#define MSTR    4
#define CPOL    3
#define CPHA    2
#define SPR1    1
#define SPR0    0


/* =========================================================
 * SPSR - SPI Status Register
 * ========================================================= */

#define SPIF    7
#define WCOL    6
#define SPI2X   0

/* =========================================================
 * TWI / I2C
 * ========================================================= */

#define TWBR0   REG8(0xB8)
#define TWSR0   REG8(0xB9)
#define TWAR0   REG8(0xBA)
#define TWDR0   REG8(0xBB)
#define TWCR0   REG8(0xBC)
#define TWAMR0  REG8(0xBD)

/* TWI bits */

#define TWINT0  7
#define TWEA0   6
#define TWSTA0  5
#define TWSTO0  4
#define TWWC0   3
#define TWEN0   2
#define TWIE0   0


/* =========================================================
 * ADC
 * ========================================================= */

#define ADMUX   REG8(0x7C)
#define ADCSRA  REG8(0x7A)
#define ADCSRB  REG8(0x7B)
#define ADCL    REG8(0x78)
#define ADCH    REG8(0x79)

/* ADC bits */

#define REFS1   7
#define REFS0   6
#define ADLAR   5

#define ADEN    7
#define ADSC    6
#define ADATE   5
#define ADIF    4
#define ADIE    3
#define ADPS2   2
#define ADPS1   1
#define ADPS0   0


/* =========================================================
 * TIMER 0
 * ========================================================= */

#define TCCR0A  REG8(0x44)
#define TCCR0B  REG8(0x45)
#define TCNT0   REG8(0x46)
#define OCR0A   REG8(0x47)
#define OCR0B   REG8(0x48)

#define COM0A1  7
#define COM0A0  6
#define COM0B1  5
#define COM0B0  4
#define WGM01   1
#define WGM00   0

#define FOC0A   7
#define FOC0B   6
#define WGM02   3
#define CS02    2
#define CS01    1
#define CS00    0


/* =========================================================
 * TIMER 1
 * ========================================================= */

#define TCCR1A  REG8(0x80)
#define TCCR1B  REG8(0x81)
#define TCCR1C  REG8(0x82)

#define TCNT1L  REG8(0x84)
#define TCNT1H  REG8(0x85)

#define OCR1AL  REG8(0x88)
#define OCR1AH  REG8(0x89)
#define OCR1BL  REG8(0x8A)
#define OCR1BH  REG8(0x8B)
#define OCR1CL  REG8(0x8C)
#define OCR1CH  REG8(0x8D)

#define ICR1L   REG8(0x86)
#define ICR1H   REG8(0x87)


/* =========================================================
 * TIMER 2
 * ========================================================= */

#define TCCR2A  REG8(0xB0)
#define TCCR2B  REG8(0xB1)
#define TCNT2   REG8(0xB2)
#define OCR2A   REG8(0xB3)
#define OCR2B   REG8(0xB4)


/* =========================================================
 * TIMER 3
 * ========================================================= */

#define TCCR3A  REG8(0x90)
#define TCCR3B  REG8(0x91)
#define TCCR3C  REG8(0x92)

#define TCNT3L  REG8(0x94)
#define TCNT3H  REG8(0x95)

#define ICR3L   REG8(0x96)
#define ICR3H   REG8(0x97)

#define OCR3AL  REG8(0x98)
#define OCR3AH  REG8(0x99)
#define OCR3BL  REG8(0x9A)
#define OCR3BH  REG8(0x9B)
#define OCR3CL  REG8(0x9C)
#define OCR3CH  REG8(0x9D)


/* =========================================================
 * TIMER 4
 * ========================================================= */

#define TCCR4A  REG8(0xA0)
#define TCCR4B  REG8(0xA1)
#define TCCR4C  REG8(0xA2)

#define TCNT4L  REG8(0xA4)
#define TCNT4H  REG8(0xA5)

#define ICR4L   REG8(0xA6)
#define ICR4H   REG8(0xA7)

#define OCR4AL  REG8(0xA8)
#define OCR4AH  REG8(0xA9)
#define OCR4BL  REG8(0xAA)
#define OCR4BH  REG8(0xAB)
#define OCR4CL  REG8(0xAC)
#define OCR4CH  REG8(0xAD)


/* =========================================================
 * TIMER 5
 * ========================================================= */

#define TCCR5A  REG8(0x120)
#define TCCR5B  REG8(0x121)
#define TCCR5C  REG8(0x122)

#define TCNT5L  REG8(0x124)
#define TCNT5H  REG8(0x125)

#define ICR5L   REG8(0x126)
#define ICR5H   REG8(0x127)

#define OCR5AL  REG8(0x128)
#define OCR5AH  REG8(0x129)
#define OCR5BL  REG8(0x12A)
#define OCR5BH  REG8(0x12B)
#define OCR5CL  REG8(0x12C)
#define OCR5CH  REG8(0x12D)

/* =========================================================
 * TIMER 1 BITS
 * ========================================================= */

#define COM1A1  7
#define COM1A0  6
#define COM1B1  5
#define COM1B0  4
#define COM1C1  3
#define COM1C0  2
#define WGM11   1
#define WGM10   0

#define ICNC1   7
#define ICES1   6
#define WGM13   4
#define WGM12   3
#define CS12    2
#define CS11    1
#define CS10    0

#define FOC1A   7
#define FOC1B   6
#define FOC1C   5

/* =========================================================
 * TIMER 2 BITS
 * ========================================================= */

#define COM2A1  7
#define COM2A0  6
#define COM2B1  5
#define COM2B0  4
#define WGM21   1
#define WGM20   0

#define FOC2A   7
#define FOC2B   6
#define WGM22   3
#define CS22    2
#define CS21    1
#define CS20    0

/* =========================================================
 * TIMER 3 BITS
 * ========================================================= */

#define COM3A1  7
#define COM3A0  6
#define COM3B1  5
#define COM3B0  4
#define COM3C1  3
#define COM3C0  2
#define WGM31   1
#define WGM30   0

#define ICNC3   7
#define ICES3   6
#define WGM33   4
#define WGM32   3
#define CS32    2
#define CS31    1
#define CS30    0

#define FOC3A   7
#define FOC3B   6
#define FOC3C   5

/* =========================================================
 * TIMER 4 BITS
 * ========================================================= */

#define COM4A1  7
#define COM4A0  6
#define COM4B1  5
#define COM4B0  4
#define COM4C1  3
#define COM4C0  2
#define WGM41   1
#define WGM40   0

#define ICNC4   7
#define ICES4   6
#define WGM43   4
#define WGM42   3
#define CS42    2
#define CS41    1
#define CS40    0

#define FOC4A   7
#define FOC4B   6
#define FOC4C   5

/* =========================================================
 * TIMER 5 BITS
 * ========================================================= */

#define COM5A1  7
#define COM5A0  6
#define COM5B1  5
#define COM5B0  4
#define COM5C1  3
#define COM5C0  2
#define WGM51   1
#define WGM50   0

#define ICNC5   7
#define ICES5   6
#define WGM53   4
#define WGM52   3
#define CS52    2
#define CS51    1
#define CS50    0

#define FOC5A   7
#define FOC5B   6
#define FOC5C   5

/* =========================================================
 * WATCHDOG TIMER
 * ========================================================= */

#define WDTCSR  REG8(0x60)

/* WDTCSR bits */

#define WDIF    7
#define WDIE    6
#define WDP3    5
#define WDCE    4
#define WDE     3
#define WDP2    2
#define WDP1    1
#define WDP0    0

/* =========================================================
 * EXTERNAL INTERRUPT
 * ========================================================= */

#define EICRA   REG8(0x69)
#define EICRB   REG8(0x6A)

#define EIFR    REG8(0x3C)
#define EIMSK   REG8(0x3D)


/* =========================================================
 * EICRA - INT0 ~ INT3
 * ========================================================= */

#define ISC00   0
#define ISC01   1

#define ISC10   2
#define ISC11   3

#define ISC20   4
#define ISC21   5

#define ISC30   6
#define ISC31   7


/* =========================================================
 * EICRB - INT4 ~ INT7
 * ========================================================= */

#define ISC40   0
#define ISC41   1

#define ISC50   2
#define ISC51   3

#define ISC60   4
#define ISC61   5

#define ISC70   6
#define ISC71   7


/* =========================================================
 * EIMSK
 * ========================================================= */

#define INT0    0
#define INT1    1
#define INT2    2
#define INT3    3
#define INT4    4
#define INT5    5
#define INT6    6
#define INT7    7


/* =========================================================
 * EIFR
 * ========================================================= */

#define INTF0   0
#define INTF1   1
#define INTF2   2
#define INTF3   3
#define INTF4   4
#define INTF5   5
#define INTF6   6
#define INTF7   7


/* =========================================================
 * EEPROM
 * ========================================================= */

#define EECR    REG8(0x3F)
#define EEDR    REG8(0x40)
#define EEARL   REG8(0x41)
#define EEARH   REG8(0x42)

#define EERE    0
#define EEPE    1
#define EEMPE   2
#define EERIE   3
#define EEPM0   4
#define EEPM1   5


/* =========================================================
 * Global Interrupt Control
 * ========================================================= */

#define SREG    REG8(0x5F)

#define I_BIT   7


/* =========================================================
 * ADC REGISTERS - ATmega640
 * ========================================================= */

/* ADC Data Registers */
#define ADCL        REG8(0x78)
#define ADCH        REG8(0x79)

/* ADC Control and Status */
#define ADCSRA      REG8(0x7A)
#define ADCSRB      REG8(0x7B)

/* ADC Multiplexer */
#define ADMUX       REG8(0x7C)

/* Digital Input Disable */
#define DIDR2       REG8(0x7D)
#define DIDR0       REG8(0x7E)


/* =========================================================
 * ADMUX BITS
 * ========================================================= */

#define REFS1       7
#define REFS0       6
#define ADLAR       5

#define MUX4        4
#define MUX3        3
#define MUX2        2
#define MUX1        1
#define MUX0        0


/* =========================================================
 * ADCSRA BITS
 * ========================================================= */

#define ADEN        7
#define ADSC        6
#define ADATE       5
#define ADIF        4
#define ADIE        3

#define ADPS2       2
#define ADPS1       1
#define ADPS0       0


/* =========================================================
 * ADCSRB BITS
 * ========================================================= */

#define ACME        6
#define MUX5        3

#define ADTS2       2
#define ADTS1       1
#define ADTS0       0


/* =========================================================
 * DIDR0 BITS
 * ADC0 - ADC7
 * ========================================================= */

#define ADC0D       0
#define ADC1D       1
#define ADC2D       2
#define ADC3D       3
#define ADC4D       4
#define ADC5D       5
#define ADC6D       6
#define ADC7D       7


/* =========================================================
 * DIDR2 BITS
 * ADC8 - ADC15
 * ========================================================= */

#define ADC8D       0
#define ADC9D       1
#define ADC10D      2
#define ADC11D      3
#define ADC12D      4
#define ADC13D      5
#define ADC14D      6
#define ADC15D      7

#endif