#ifndef _UART_H_
#define _UART_H_

#include "LPC8xx.h"
#include <string.h>

#define UART_SUCCESS        0
#define UART_PARAM_ERR      1
#define UART_NO_CHAR        2
#define UART_NO_SPACE       3
#define UART_BUSY           4
#define UART_ERROR          255
#define RXRDY               (0x01<<0)
#define RXIDLE              (0x01<<1)
#define TXRDY               (0x01<<2)
#define TXIDLE              (0x01<<3)
#define RS485PORT        (1<<27)
#define DIR485_TX	LPC_GPIO->PIN[0] |=  RS485PORT
#define DIR485_RX	LPC_GPIO->PIN[0] &=  ~RS485PORT

extern void UART_Init(void);
extern uint8_t UART_Open(void);
extern uint8_t UART_Close(void);
extern uint8_t UART_SendByte(uint8_t ucData);
extern void UART_SendString(char *pucData, uint32_t uiLen);
extern uint8_t UART_RecvByte(uint8_t *ucData);
extern void UART_RecvString(uint8_t *pucData, uint32_t uiLen);
extern uint8_t UART_RecvByte(uint8_t *ucData);

#endif                                                             

