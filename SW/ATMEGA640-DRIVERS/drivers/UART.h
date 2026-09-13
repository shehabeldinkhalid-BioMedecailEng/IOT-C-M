#ifndef UART_H_
#define UART_H_

#include "../../DataTypes.h"


/* =========================
 * USART Channels
 * ========================= */

#define UART0    0
#define UART1    1
#define UART2    2
#define UART3    3


/* =========================
 * UART Configuration
 * ========================= */

#define UART_ASYNC        0
#define UART_SYNC         1

#define UART_PARITY_NONE  0
#define UART_PARITY_EVEN  2
#define UART_PARITY_ODD   3

#define UART_STOP_1       1
#define UART_STOP_2       2

#define UART_DATA_5       5
#define UART_DATA_6       6
#define UART_DATA_7       7
#define UART_DATA_8       8
#define UART_DATA_9       9


/* =========================
 * Initialization
 * ========================= */

void UART_Init(
    DT_uint8_t UART,
    DT_uint32_t BaudRate
);


/* =========================
 * Transmit
 * ========================= */

void UART_SendByte(
    DT_uint8_t UART,
    DT_uint8_t Data
);

void UART_SendString(
    DT_uint8_t UART,
    const char* String
);


/* =========================
 * Receive
 * ========================= */

DT_uint8_t UART_ReceiveByte(
    DT_uint8_t UART
);

void UART_ReceiveString(
    DT_uint8_t UART,
    char* Buffer,
    DT_uint16_t Size
);


/* =========================
 * Status
 * ========================= */

DT_uint8_t UART_IsDataAvailable(
    DT_uint8_t UART
);

DT_uint8_t UART_IsTransmitReady(
    DT_uint8_t UART
);

DT_uint8_t UART_IsTransmitComplete(
    DT_uint8_t UART
);


/* =========================
 * Control
 * ========================= */

void UART_Enable(
    DT_uint8_t UART
);

void UART_Disable(
    DT_uint8_t UART
);


#endif /* UART_H_ */