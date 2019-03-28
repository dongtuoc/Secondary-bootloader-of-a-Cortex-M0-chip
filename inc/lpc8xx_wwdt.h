/*********************************************Copyright (c)***********************************************
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               Lpc8xx_wwdt.h
** Last modified date:      2017-9-27
** Last version:            V1.0
** Description:             看门狗函数头文件       
*********************************************************************************************************/


#ifndef LPC8XX_WWDT_H_
#define LPC8XX_WWDT_H_


/* 模式寄存器 */
#define WDEN      0
#define WDRESET   1
#define WDTOF     2
#define WDINT     3
#define WDPROTECT 4
#define LOCK      5

void WatchDog_Initial(void);
void wdt_feed(void);

#endif 
