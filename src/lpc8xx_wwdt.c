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
	/* ʹ�ܿ��Ź�ʱ��Դ */
	LPC_SYSCON->SYSAHBCLKCTRL |= WWDT;

	/* ��⿴�Ź������־�Ƿ���λ */
	if (LPC_WWDT->MOD & (1<<WDTOF)) 
	{
		/* ͨ��д0�����WDTOF��־ */
		LPC_WWDT->MOD &= ~(1<<WDTOF);
	}

	/* �������Ź����� */
	LPC_SYSCON->PDRUNCFG &= ~(WDTOSC_PD);
	
	/* ��SYSCON������ WDCLK Ƶ��
	*  DIVSEL--��Ƶ��  FREQSEL:ģ�����Ƶ�� ����WDTOSCCTRLΪwdt_osc_clk 
	*  ����WDTʱ�ӣ�div_sel  = 31 ģ��ʱ�� Freq  = 0.6 MHz����Ƶֵ DIV = 2 * div_sel + 2 = 64��
    *  WDTƵ��  WDT_OSC = 0.6MHz / 64 = 9375 Hz��WDT���� Twdt_osc = 1 / 9375 (s)
    */
	LPC_SYSCON->WDTOSCCTRL = (1<<FREQSEL)|(31<<DIVSEL);

    /*
    * WDT����ι��ʱ����㣺
    * T = ( WDT_TC - WDT_WARNINT ) * 4 * Twdt_osc
    * �磬������Ϊ2s��
    * 2 = ( TC * 4 ) / 9375  --> TC = 0x1250
    */

	LPC_WWDT->WARNINT = 0x3FF;
	LPC_WWDT->WINDOW = 0x1250;
	LPC_WWDT->TC = 0x1250;      /*���Ź��Ķ�ʱʱ��TIMER = (TC*4)/(wdt_osc_clk)������Ķ�ʱʱ��Ϊ2s*/
	LPC_WWDT->MOD = (0<<WDTOF)|(1<<WDEN)|(1<<WDRESET);

	/* ʹ��WWDT�ж� */
	//NVIC_EnableIRQ(WDT_IRQn);

	/* ι��ʱ�� */
	LPC_WWDT->FEED = 0xAA;
	LPC_WWDT->FEED = 0x55;
}

/*���Ź���λ����*/ 
void wdt_feed(void)
{ 
    if( (LPC_WWDT->TV) <= (LPC_WWDT->WINDOW))
	{
		/* ι�� */
		LPC_WWDT->FEED = 0xAA;
		LPC_WWDT->FEED = 0x55;
	}
}

