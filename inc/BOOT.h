
#ifndef _BOOT_H_
#define _BOOT_H_

#include <stdint.h>
#include "UART.h"
#include "TICKTASK.h"

#define FLASH_ADDR_0              (0x00000000)
#define RAM_ADDR_0                (0x10000000)
#define RAM_ADDR_RESET         	  (RAM_ADDR_0 + 4)
#define VECTORTAB_WORD        	  (192) 
#define PAGE_BYTE                 (64)
#define SECT_BYTE                 (1024)
#define APP_START_Flash         	(0x00000C00)            
#define APP_END_Flash           	(0x00007BFF)          
#define APP_START_RESET         	(APP_START_Flash + 4) 
#define APP_START_RAM           	(0x100000C0)       
#define APP_END_RAM             	(0x10001FFF)            
#define BOOT_FLAG_ADDR			      (0x7BFE)						
#define BOOT_FLAG_PAGE			      (BOOT_FLAG_ADDR/PAGE_BYTE)
#define BOOT_FLAG_SECT			      (BOOT_FLAG_ADDR/SECT_BYTE)
#define APP_START_SECTOR        	(APP_START_Flash/SECT_BYTE)        
#define APP_END_SECTOR          	(31)                                    
#define BOOT_DELAYTIME_C        		1                                    
#define BOOT_WAITTIME_UPDATE        5                                   
#define FLASHWRITE_OK               0  
#define FLASHWRITE_ERROR            1   
#define UPDATE_IDLE				          0
#define UPDATE_OK             	 	  1        
#define UPDATE_ERROR          	    2  
#define UPDATE_NO 	           		  3  
#define USERCODE_OK           	    0   
#define USERCODE_ERROR       	      1 

extern U32 u32NextFlashWriteAddr;
extern void vControlSwitch(unsigned int sp,unsigned int pc);
extern void vBootLoader(void(* pfunSenceInitCallBack)(void),void(* pfunSenceRenewCallBack)(void));
extern U8 UpdateBootFlag(void);

#endif                                                    
