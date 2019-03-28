
#include "BOOT.h"
#include "main.h"
#include "IAP.h"
#include "XMODEM1K.h"
#include "lpc8xx_wwdt.h"

U32 NextFlashWriteAddr=0;

U8 UpdateBootFlag()
{
	U8 ret_code=0;
	
	ret_code = iap_prepare_sector(BOOT_FLAG_SECT, BOOT_FLAG_SECT);
	if (ret_code != CMD_SUCCESS) 
		return ret_code;
	
  	ret_code = iap_erase_page(BOOT_FLAG_PAGE, BOOT_FLAG_PAGE);
	if (ret_code != CMD_SUCCESS) 
		return ret_code;

	return CMD_SUCCESS;
}

static void vControlSwitch(unsigned int sp,unsigned int pc)
{
  asm("ldr   r0, [r0]");
  asm("mov   sp, r0");
  asm("ldr   r0, [r1]");
  asm("bx    r0");
}

static U8 ProgramFlash(U8 *pu8Data, U8 u16Len)
{
	SysTick_Reset();

    if ((pu8Data != 0) && (u16Len != 0)) 
	{
        if (iap_prepare_sector(APP_START_SECTOR, APP_END_SECTOR) == CMD_SUCCESS)
		{
			if (iap_copy_ram_to_flash(pu8Data,(void *)u32NextFlashWriteAddr, u16Len) == CMD_SUCCESS)
			{
                if (dataCompare(u32NextFlashWriteAddr, (U32)pu8Data, u16Len) == CMD_SUCCESS) 
				{
                    u32NextFlashWriteAddr += u16Len; 
					SysTick_Config(SystemCoreClock / SECOND_PER_TICK);
                    return FLASHWRITE_OK;
                }
            }
        }
    }
    SysTick_Config(SystemCoreClock / SECOND_PER_TICK);
    return FLASHWRITE_ERROR;
}

static U8 u8UpdateMode(void)
{   
	sysTimerClr(0);

	if (iap_prepare_sector(APP_START_SECTOR, APP_END_SECTOR) == CMD_SUCCESS)
	{
		return u8Xmodem1kClient(ProgramFlash, (U16)BOOT_DELAYTIME_C, (U16)BOOT_WAITTIME_UPDATE);
	}
	
	return UPDATE_IDLE;
}

static U8 u8UserCodeEffect(void)
{
    U32 u32FirstWord  = *(U32 *) APP_START_Flash;
    U32 u32SecondWord = *(U32 *)(APP_START_Flash + sizeof(U32));

    if ((u32FirstWord < APP_START_RAM) || (u32FirstWord > APP_END_RAM)) 
	{      
        return USERCODE_ERROR;
    }

    if ((u32SecondWord < APP_START_Flash) || (u32SecondWord > APP_END_Flash)) 
	{
        return USERCODE_ERROR;
    }
    return USERCODE_OK;
}

void delay(U16 ms)
{
	for (U16 i=0;i<ms;i++)
	{
		wdt_feed();	
	}
}

void vBootLoader(void (* pfunSenceInitCallBack) (void), void (* pfunSenceRenewCallBack)(void))
{
    U8 ucMessage = 0;
	unsigned int sp=0,pc=0;
	U16 bootflag_read=0;

	sp = APP_START_Flash;
	pc = sp + 4;
	
    pfunSenceInitCallBack(); 

	while(1)
	{
		wdt_feed();

		do{
			ucMessage = u8UpdateMode(); 
			
			if ((UPDATE_OK == ucMessage)||(UPDATE_NO == ucMessage)||(ucMessage == UPDATE_IDLE)) 
			{
				break;
			} 
			else if (UPDATE_ERROR == ucMessage)
			{	
				UpdateBootFlag();
				break;
			}
		} while (1);

		bootflag_read = *( volatile U16 *)(BOOT_FLAG_ADDR);
		
		if (u8UserCodeEffect() == USERCODE_OK)
		{
			if (bootflag_read == 0xAA55)
			{
				delay(30000);
				UART_SendString("\r\nRun APP>>\r\n",sizeof("\r\nRun APP>>\r\n"));
				pfunSenceRenewCallBack();
				vControlSwitch(sp,pc);
			}
		}
	}
}

