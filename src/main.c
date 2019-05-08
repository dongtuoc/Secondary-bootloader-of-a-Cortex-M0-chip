
#include "includes.h"
#include "BOOT.h" 

#define BOD_INT_RESV    (0 << 2)                                        /* Reserved                     */
#define BOD_INT_LVL1    (1 << 2)                                        /* 2.3 ~2.4                     */
#define BOD_INT_LVL2    (2 << 2)                                        /* 2.6 ~2.7                     */
#define BOD_INT_LVL3    (3 << 2)                                        /* 2.8 ~2.9                     */
#define BOD_RST_RESV    (0 << 0)                                        /* Reserved                     */
#define BOD_RST_LVL1    (1 << 0)                                        /* 2.1 ~2.2                     */
#define BOD_RST_LVL2    (2 << 0)                                        /* 2.4 ~2.5                     */
#define BOD_RST_LVL3    (3 << 0)                                        /* 2.6 ~2.8                     */
#define BOD_RST_ENABLE  (1 << 4)                                        /* enable BOD              */

#define POR_RESET       (1 << 0)
#define EXT_RESET       (1 << 1)
#define WDT_RESET       (1 << 2)
#define BOD_RESET       (1 << 3)
#define SYS_RESET       (1 << 4)

void BODInit(void)
{
    LPC_SYSCON->BODCTRL = BOD_RST_LVL3| BOD_RST_ENABLE;    
    NVIC_EnableIRQ(BOD_IRQn);            
}

void SysTick_Handler(void)
{	
    timerTick();
    static U8 timecount;
    timecount++;
    if ((timecount % 100) == 0)
    {
        timecount = 0;
        LPC_GPIO->NOT[0] |=  (1<<21);
    }
}

void GPIOInit (void)
{
    LPC_SYSCON->SYSAHBCLKCTRL |= ((1 << 6)|(1 << 7)|(1 << 14));         
    LPC_GPIO->DIR[0] |=  ((1<<27) | (1<<21));                                     
}

void vScene_Init (void)
{
    UART_Init();                                                      
    SysTick_Config(SystemCoreClock / SECOND_PER_TICK);
    GPIOInit();
}

void SysTick_Reset (void)
{
    SysTick->LOAD  = 0;                                               
    SysTick->VAL   = 0;                                            
    SysTick->CTRL  = 0;                                             
}

void vScene_Renew (void)
{
    SysTick_Reset();
    LPC_SYSCON->SYSAHBCLKCTRL &= ~(7 << 14);
    LPC_SYSCON->SYSAHBCLKCTRL &= ~(1 << 17);
    LPC_SWM->PINASSIGN[0] |= ( 0xFF << 16 ); 
    NVIC_DisableIRQ(WDT_IRQn);
}

int main (void)
{
    SystemCoreClockUpdate();   
    vBootLoader(&vScene_Init,&vScene_Renew);
}

