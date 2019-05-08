
#include "xmodem1k.h"
#include "LPC8xx.h"
#include "ticktask.h"
#include "UART.h"
#include "CRC.h"
#include "lpc8xx_wwdt.h"
#include "IAP.h"
#include "BOOT.h"

#define SOH                         (0x01)                                
#define STX   (0x02)                                
#define EOT   (0x04)                                
#define ACK   (0x06)                                
#define NAK   (0x15)                               
#define CAN   (0x18)                                
#define POLL  (0x43)                                

#define STAT_IDLE_DATA    0                              
#define STAT_IDLE_C       1                               
#define STAT_CONNECT      2
#define STAT_RECEIVE      3                               
#define STAT_HANDLE       4                                 
#define STAT_ACK          5                              
#define STAT_NAK          6                                
#define STAT_CAN          7                              
#define STAT_END          8                           
#define STAT_TIMEOUT_C    9                    

#define LONGPKT_LEN       1024             
#define SHORTPKT_LEN      128                       
#define PKT_HEAD_LEN      3                               

#define PKT_TIMEOUT_MS    100                    
#define CHAR_TIMEOUT_MS   2                             
typedef union 
{
    struct
	{
        U8 u8ShortBuff[SHORTPKT_LEN]; 
        U8 u8ShortCRC[2];                  
    };
    struct 
	{
        U8 u8LongBuff[LONGPKT_LEN];  
        U8 u8LongCRC[2];
    };
} __uXMODEM_DATA, *__puXMODEM_DATA;

typedef struct 
{
    U8 u8Ctrl;                                
    U8 u8Index;                               
    U8 u8Patch;                                
} __tXMODEM_HEAD, *__ptXMODEM_HEAD;

__uXMODEM_DATA  uData; 
__puXMODEM_DATA puData = &uData;

__tXMODEM_HEAD  tHead;                      
__ptXMODEM_HEAD ptHead = &tHead;

U8 u8Xmodem1kClient(pFunPKTHAND pfunPktHandle, U16  u16ShortDly, U16 u8LongDly)
{
    U32 u32ByteCnt   = 0;                                         
    U8  u8TimeoutCnt = 0;                                         
    U8  u8DataerrCnt = 0;                                         
    U8  u8PktIndex   = 1;                                         
    U8  u8STATE = STAT_IDLE_C;                                  
    U8  u8Data=0;                                                 
    volatile U16 u16PktLen=0;                                    
    U8  u8Message=0;
	U8 IfFlashChanged = 0;

    sysTimerClr(1);
	
    while (1)
	{
		wdt_feed();
	  
        switch (u8STATE)
		{
        	case STAT_IDLE_C:
				if (sysTimerGet(1) >= (u8LongDly * SECOND_PER_TICK))
	                u8STATE = STAT_TIMEOUT_C;
			 	else {
	                u8Data = POLL;
	                do{
							u8Message = UART_SendByte(u8Data);											
	               	   }while (u8Message == UART_NO_SPACE);
					sysTimerClr(0);
	                u8STATE = STAT_IDLE_DATA;
				}
            	break; 
        	case STAT_IDLE_DATA:
				if (UART_RecvByte(&u8Data) == UART_SUCCESS)
				{
	                u8STATE = STAT_CONNECT;
	                sysTimerClr(0);
				} 
				else
				{
					if (sysTimerGet(0) >= (u16ShortDly * SECOND_PER_TICK))                   
					{
	                    u8STATE = STAT_IDLE_C;
	                }
				}
				break;
			case STAT_CONNECT:                                              
				if ((u8Data == SOH) || (u8Data == STX))
				{
	                u16PktLen = (u8Data == SOH)? SHORTPKT_LEN : LONGPKT_LEN;
	                ((U8 *)ptHead)[u32ByteCnt] = u8Data;
	                u32ByteCnt++;
	                u8STATE = STAT_RECEIVE;
				} 
				else
				{
                	u8STATE = STAT_IDLE_C;
				}
				break;
        	case STAT_RECEIVE:
				if (UART_RecvByte(&u8Data) == UART_SUCCESS) 
				{            
					if (u32ByteCnt < PKT_HEAD_LEN) 
					{
	                    ((U8 *)ptHead)[u32ByteCnt] = u8Data;
	                    if (ptHead->u8Ctrl == EOT)
						{
	                        u8STATE = STAT_ACK;
	                        break;
	                    }
	                } 
					else 
					{
	                    ((U8 *)puData)[u32ByteCnt - 3] = u8Data;
	                }
                	u32ByteCnt++;
				
	                if (u32ByteCnt >= u16PktLen + PKT_HEAD_LEN + 2)
					{
	                    u8STATE = STAT_HANDLE;
	                }
	                u8TimeoutCnt = 0;
	                sysTimerClr(0);
            	} 
				else
				{                              
	               if (sysTimerGet(0) >= ((u32ByteCnt == 0) ? PKT_TIMEOUT_MS : CHAR_TIMEOUT_MS)) 
	               {
	                    sysTimerClr(0);
	                    u8TimeoutCnt++;
	                    u8STATE = STAT_NAK;
	               	}
	            }
	            break;    
			case STAT_HANDLE: 
        	{
	            U16 u16CRCTemp=0;
	            U16 u16CRCTemp1=0;
			
            	if (ptHead->u8Ctrl != ((u16PktLen == SHORTPKT_LEN) ? SOH : STX))  
				{                                                         
	                u8DataerrCnt++;
	                u8STATE = STAT_NAK;
	                break;
	            }
	            if (ptHead->u8Index + ptHead->u8Patch != 0xFF)
				{          
	                u8DataerrCnt++;
	                u8STATE = STAT_NAK;
	                break;
	            }
	            if ((ptHead->u8Index) == (u8PktIndex - 1))
	            {
	            	u8STATE = STAT_ACK;
	                break;
	            }
	            if (ptHead->u8Index != u8PktIndex)
	            {
	            	u8DataerrCnt++;
	                u8STATE = STAT_NAK;
	                break;
	            }
	            u16CRCTemp = ((U16)(*((U8 *)puData + u16PktLen)) << 8) | (*((U8 *)puData + u16PktLen + 1));
	            
	            u16CRCTemp1=u16CRCVerify((U8 *)puData, u16PktLen, 0);
	            if ( u16CRCTemp1 != u16CRCTemp)
				{
	                u8DataerrCnt++;
	                u8STATE = STAT_NAK;                                   
	                break;
	            }

				if (ptHead->u8Index == 1)
				{
					if	((puData->u8LongBuff[1020] != 0x5A) || (puData->u8LongBuff[1021]!= 0x43) ||
						 (puData->u8LongBuff[1022] != 0x5A) || (puData->u8LongBuff[1023] != 0x43))
					{
						return UPDATE_NO;
					}
					
					u32NextFlashWriteAddr = APP_START_Flash;  
						
					if (iap_erase_sector(APP_START_SECTOR, APP_END_SECTOR) != CMD_SUCCESS)
					{
						IfFlashChanged = 1;
						u8STATE = STAT_CAN;
						break;
					}
					IfFlashChanged = 1;
				}
	
	            if (!pfunPktHandle((U8 *)puData, u16PktLen))
				{
					IfFlashChanged = 1;
					u8PktIndex++;
	                u8STATE = STAT_ACK;                                     
	                break;
	            }
				IfFlashChanged = 1;
	            u8DataerrCnt++;
	            u8STATE = STAT_NAK;
	            break;
       		}
        	case STAT_ACK:                                                
	             u8Data = ACK;
	             do{
	                 	u8Message = UART_SendByte(u8Data);
	                }while (u8Message == UART_NO_SPACE);

	             if (ptHead->u8Ctrl == EOT)                                 
				 {                               
	                u8STATE = STAT_END;                                    
	                break;
	             }
	             u8DataerrCnt = 0;
	             u32ByteCnt = 0;
	             u8STATE = STAT_RECEIVE;
	             break;
			case STAT_NAK:
	             if ((u8DataerrCnt >= 5) || (u8TimeoutCnt >= 5)) 
	             {
	             	u8STATE = STAT_CAN;
	                break;
	             }
	             u8Data = NAK;
	             do{
						u8Message = UART_SendByte(u8Data);
	               }while (u8Message == UART_NO_SPACE);
			 
	             u32ByteCnt = 0;
	             u8STATE = STAT_RECEIVE;
	             break;
			case STAT_CAN:
				u8Data = CAN;
				do{
						u8Message = UART_SendByte(u8Data);
					}while (u8Message == UART_NO_SPACE);
				if (IfFlashChanged == 1)
				{
					IfFlashChanged = 0;
					return UPDATE_ERROR;
				}
				else
				{
					return UPDATE_NO;
				}
				break;
			case STAT_END:
				return UPDATE_OK;
			case STAT_TIMEOUT_C:
				return UPDATE_NO;
			default:
				break;
        }
    }
}


