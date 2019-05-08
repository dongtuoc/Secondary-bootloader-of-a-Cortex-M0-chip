/*
*****************************************************************************************
*	WWDT CODE .C
*
*	THIS code is submitted by idongtuocan via ALPHADAMO.COM.
*	THIS code is used to 
*								-- WDTTHINGS
*	YOU are free to download it for deeper research. 
*  ANY questions you can contact me via www.alphadamo.com.
*****************************************************************************************
*/

#include "LPC8xx.h"
#include "lpc8xx_wwdt.h"
#include "lpc8xx_syscon.h"
#include "boot.h"


void WatchDog_Initial(void) 
{
    /* 使能看门狗时钟源 */
    LPC_SYSCON->SYSAHBCLKCTRL |= WWDT;

    /* 检测看门狗溢出标志是否被置位 */
    if (LPC_WWDT->MOD & (1<<WDTOF)) 
    {
        /* 通过写0来清除WDTOF标志 */
        LPC_WWDT->MOD &= ~(1<<WDTOF);
    }

    /* 开启看门狗振荡器 */
    LPC_SYSCON->PDRUNCFG &= ~(WDTOSC_PD);

    /* 在SYSCON中配置 WDCLK 频率
    *  DIVSEL--分频数  FREQSEL:模拟输出频率 配置WDTOSCCTRL为wdt_osc_clk 
    *  配置WDT时钟：div_sel  = 31 模拟时钟 Freq  = 0.6 MHz，分频值 DIV = 2 * div_sel + 2 = 64，
    *  WDT频率  WDT_OSC = 0.6MHz / 64 = 9375 Hz，WDT周期 Twdt_osc = 1 / 9375 (s)
    */
    LPC_SYSCON->WDTOSCCTRL = (1<<FREQSEL)|(31<<DIVSEL);

    /*
    * WDT提醒喂狗时间计算：
    * T = ( WDT_TC - WDT_WARNINT ) * 4 * Twdt_osc
    * 如，想设置为2s：
    * 2 = ( TC * 4 ) / 9375  --> TC = 0x1250
    */

    LPC_WWDT->WARNINT = 0x3FF;
    LPC_WWDT->WINDOW = 0x1250;
    LPC_WWDT->TC = 0x1250;      /*看门狗的定时时间TIMER = (TC*4)/(wdt_osc_clk)，这里的定时时间为2s*/
    LPC_WWDT->MOD = (0<<WDTOF)|(1<<WDEN)|(1<<WDRESET);

    /* 使能WWDT中断 */
    //NVIC_EnableIRQ(WDT_IRQn);

    /* 喂狗时序 */
    LPC_WWDT->FEED = 0xAA;
    LPC_WWDT->FEED = 0x55;
}

/*看门狗复位计算*/ 
void wdt_feed(void)
{ 
    if( (LPC_WWDT->TV) <= (LPC_WWDT->WINDOW))
    {
        /* 喂狗 */
        LPC_WWDT->FEED = 0xAA;
        LPC_WWDT->FEED = 0x55;
    }
}

