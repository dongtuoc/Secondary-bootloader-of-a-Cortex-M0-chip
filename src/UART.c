#include "UART.h"

#define UART_BPS        115200    
#define BUFFSIZE        1024

void UART_Init (void)
{
  LPC_SWM->PINASSIGN[0] &= ~( 0xFFFFFF << 0 );
  LPC_SWM->PINASSIGN[0] |=  ( 26 << 0 );                          
  LPC_SWM->PINASSIGN[0] |=  ( 25 << 8 );                              
  LPC_SWM->PINASSIGN[0] |=  ( 27 << 16);                            

  LPC_SYSCON->UARTCLKDIV     = 1;                                     
  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<14);                             

  LPC_USART0->BRG = SystemCoreClock * LPC_SYSCON->SYSAHBCLKDIV /
                    (LPC_SYSCON->UARTCLKDIV * 16 * UART_BPS) - 1;   
    
  LPC_USART0->CFG = (1 << 0) |                                   
                    (1 << 2) |                                       
                    (2 << 4) |                                 
                    (0 << 6) |                                        
                    (1 << 18) |   //OETA
                    (1 << 20) |    //OESEL
                    (1 << 21);    //OEPOL
}

uint8_t UART_SendByte (uint8_t ucData)
{
  if((LPC_USART0->STAT & 0x04) != 0)                                    
  {                                    
    LPC_USART0->TXDATA = ucData;                                            
    while ((LPC_USART0->STAT & 0x08) == 0);
    return UART_SUCCESS;
  }
    return UART_NO_SPACE;
}

void UART_SendString (char *pucData, uint32_t uiLen)
{	
  while ( uiLen != 0 )
  {
    while (((LPC_USART0->STAT) & TXRDY) == 0);
    LPC_USART0->TXDATA = *pucData;
    while((LPC_USART0->STAT & TXIDLE)==0);

    pucData++;
    uiLen--;
  }
  while((LPC_USART0->STAT & TXIDLE)==0);
  return ;
}

uint8_t UART_RecvByte(uint8_t *ucData)
{
  if((LPC_USART0->STAT & 0x01) == 1)                               
  {                                 
    *ucData = LPC_USART0->RXDATA;
    return UART_SUCCESS;
  }
  return UART_NO_CHAR;
}
