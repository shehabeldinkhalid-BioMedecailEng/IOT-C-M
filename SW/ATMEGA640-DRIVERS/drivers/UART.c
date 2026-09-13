#include "UART.h"
#include "../../ATmega640_REGS.h"
#define F_CPU 16000000UL

/* =========================================================
 * USART BIT DEFINITIONS
 * USART0 bit positions are already defined in the register
 * file. USART1/2/3 use the same register bit positions.
 * ========================================================= */
/* UCSRnA */
#define UART_RXC_BIT      7
#define UART_TXC_BIT      6
#define UART_UDRE_BIT     5
#define UART_FE_BIT       4
#define UART_DOR_BIT      3
#define UART_UPE_BIT      2
#define UART_U2X_BIT      1

/* UCSRnB */
#define UART_RXCIE_BIT    7
#define UART_TXCIE_BIT    6
#define UART_UDRIE_BIT    5
#define UART_RXEN_BIT     4
#define UART_TXEN_BIT     3
#define UART_UCSZ2_BIT    2
#define UART_RXB8_BIT     1
#define UART_TXB8_BIT     0

/* UCSRnC */
#define UART_UMSEL1_BIT   7
#define UART_UMSEL0_BIT   6
#define UART_UPM1_BIT     5
#define UART_UPM0_BIT     4
#define UART_USBS_BIT     3
#define UART_UCSZ1_BIT    2
#define UART_UCSZ0_BIT    1
#define UART_UCPOL_BIT    0

/* =========================================================
 * Private Register Access
 * ========================================================= */

static volatile DT_uint8_t* UART_Get_UCSR_A(DT_uint8_t UART)
{
    switch (UART)
    {
        case UART0:
            return &UCSR0A;

        case UART1:
            return &UCSR1A;

        case UART2:
            return &UCSR2A;

        case UART3:
            return &UCSR3A;

        default:
            return 0;
    }
}


static volatile DT_uint8_t* UART_Get_UCSR_B(DT_uint8_t UART)
{
    switch (UART)
    {
        case UART0:
            return &UCSR0B;

        case UART1:
            return &UCSR1B;

        case UART2:
            return &UCSR2B;

        case UART3:
            return &UCSR3B;

        default:
            return 0;
    }
}


static volatile DT_uint8_t* UART_Get_UCSR_C(DT_uint8_t UART)
{
    switch (UART)
    {
        case UART0:
            return &UCSR0C;

        case UART1:
            return &UCSR1C;

        case UART2:
            return &UCSR2C;

        case UART3:
            return &UCSR3C;

        default:
            return 0;
    }
}


static volatile DT_uint8_t* UART_Get_UBRR_L(DT_uint8_t UART)
{
    switch (UART)
    {
        case UART0:
            return &UBRR0L;

        case UART1:
            return &UBRR1L;

        case UART2:
            return &UBRR2L;

        case UART3:
            return &UBRR3L;

        default:
            return 0;
    }
}


static volatile DT_uint8_t* UART_Get_UBRR_H(DT_uint8_t UART)
{
    switch (UART)
    {
        case UART0:
            return &UBRR0H;

        case UART1:
            return &UBRR1H;

        case UART2:
            return &UBRR2H;

        case UART3:
            return &UBRR3H;

        default:
            return 0;
    }
}


static volatile DT_uint8_t* UART_Get_UDR(DT_uint8_t UART)
{
    switch (UART)
    {
        case UART0:
            return &UDR0;

        case UART1:
            return &UDR1;

        case UART2:
            return &UDR2;

        case UART3:
            return &UDR3;

        default:
            return 0;
    }
}


/* =========================================================
 * UART_Init
 *
 * Default Frame:
 * Asynchronous
 * 8 Data Bits
 * No Parity
 * 1 Stop Bit
 * Normal Speed
 * ========================================================= */

void UART_Init(
    DT_uint8_t UART,
    DT_uint32_t BaudRate
)
{
    DT_uint16_t UBRR_Value;

    volatile DT_uint8_t* UCSR_A;
    volatile DT_uint8_t* UCSR_B;
    volatile DT_uint8_t* UCSR_C;

    volatile DT_uint8_t* UBRR_L;
    volatile DT_uint8_t* UBRR_H;


    UCSR_A = UART_Get_UCSR_A(UART);
    UCSR_B = UART_Get_UCSR_B(UART);
    UCSR_C = UART_Get_UCSR_C(UART);

    UBRR_L = UART_Get_UBRR_L(UART);
    UBRR_H = UART_Get_UBRR_H(UART);


    if ((UCSR_A == 0) ||
        (UCSR_B == 0) ||
        (UCSR_C == 0) ||
        (UBRR_L == 0) ||
        (UBRR_H == 0))
    {
        return;
    }


    if (BaudRate == 0)
    {
        return;
    }


    /* Normal speed */
    *UCSR_A &= ~(1 << UART_U2X_BIT);


    /* Calculate Baud Rate */
    UBRR_Value =
        (DT_uint16_t)
        ((F_CPU / (16UL * BaudRate)) - 1UL);


    /* Load Baud Rate */
    *UBRR_H =
        (DT_uint8_t)(UBRR_Value >> 8);

    *UBRR_L =
        (DT_uint8_t)(UBRR_Value & 0xFF);


    /* Disable USART before configuration */
    *UCSR_B = 0x00;


    /*
     * Asynchronous
     * No Parity
     * 1 Stop Bit
     * 8 Data Bits
     */

    *UCSR_C =
        (1 << UART_UCSZ1_BIT) |
        (1 << UART_UCSZ0_BIT);


    /* Enable RX and TX */

    *UCSR_B =
        (1 << UART_RXEN_BIT) |
        (1 << UART_TXEN_BIT);
}


/* =========================================================
 * UART_SendByte
 * ========================================================= */

void UART_SendByte(
    DT_uint8_t UART,
    DT_uint8_t Data
)
{
    volatile DT_uint8_t* UCSR_A;
    volatile DT_uint8_t* UDR;


    UCSR_A = UART_Get_UCSR_A(UART);
    UDR    = UART_Get_UDR(UART);


    if ((UCSR_A == 0) || (UDR == 0))
    {
        return;
    }


    /* Wait until transmit buffer is empty */

    while (!(*UCSR_A & (1 << UART_UDRE_BIT)))
    {
    }


    /* Put data into transmit register */

    *UDR = Data;
}


/* =========================================================
 * UART_SendString
 * ========================================================= */

void UART_SendString(
    DT_uint8_t UART,
    const char* String
)
{
    if (String == 0)
    {
        return;
    }


    while (*String != '\0')
    {
        UART_SendByte(
            UART,
            (DT_uint8_t)*String
        );

        String++;
    }
}


/* =========================================================
 * UART_ReceiveByte
 * ========================================================= */

DT_uint8_t UART_ReceiveByte(
    DT_uint8_t UART
)
{
    volatile DT_uint8_t* UCSR_A;
    volatile DT_uint8_t* UDR;


    UCSR_A = UART_Get_UCSR_A(UART);
    UDR    = UART_Get_UDR(UART);


    if ((UCSR_A == 0) || (UDR == 0))
    {
        return 0;
    }


    /* Wait until data is received */

    while (!(*UCSR_A & (1 << UART_RXC_BIT)))
    {
    }


    /* Return received data */

    return *UDR;
}


/* =========================================================
 * UART_IsDataAvailable
 * ========================================================= */

DT_uint8_t UART_IsDataAvailable(
    DT_uint8_t UART
)
{
    volatile DT_uint8_t* UCSR_A;

    UCSR_A = UART_Get_UCSR_A(UART);


    if (UCSR_A == 0)
    {
        return DT_FALSE;
    }


    return
        ((*UCSR_A & (1 << UART_RXC_BIT)) != 0);
}


/* =========================================================
 * UART_IsTransmitReady
 * ========================================================= */

DT_uint8_t UART_IsTransmitReady(
    DT_uint8_t UART
)
{
    volatile DT_uint8_t* UCSR_A;

    UCSR_A = UART_Get_UCSR_A(UART);


    if (UCSR_A == 0)
    {
        return DT_FALSE;
    }


    return
        ((*UCSR_A & (1 << UART_UDRE_BIT)) != 0);
}


/* =========================================================
 * UART_IsTransmitComplete
 * ========================================================= */

DT_uint8_t UART_IsTransmitComplete(
    DT_uint8_t UART
)
{
    volatile DT_uint8_t* UCSR_A;

    UCSR_A = UART_Get_UCSR_A(UART);


    if (UCSR_A == 0)
    {
        return DT_FALSE;
    }


    return
        ((*UCSR_A & (1 << UART_TXC_BIT)) != 0);
}


/* =========================================================
 * UART_Enable
 * ========================================================= */

void UART_Enable(
    DT_uint8_t UART
)
{
    volatile DT_uint8_t* UCSR_B;

    UCSR_B = UART_Get_UCSR_B(UART);


    if (UCSR_B == 0)
    {
        return;
    }


    *UCSR_B |=
        (1 << UART_RXEN_BIT) |
        (1 << UART_TXEN_BIT);
}


/* =========================================================
 * UART_Disable
 * ========================================================= */

void UART_Disable(
    DT_uint8_t UART
)
{
    volatile DT_uint8_t* UCSR_B;

    UCSR_B = UART_Get_UCSR_B(UART);


    if (UCSR_B == 0)
    {
        return;
    }


    *UCSR_B &=
        ~((1 << UART_RXEN_BIT) |
          (1 << UART_TXEN_BIT));
}