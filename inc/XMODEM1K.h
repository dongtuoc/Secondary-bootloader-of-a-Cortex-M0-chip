#ifndef __XMODEM1K_H
#define __XMODEM1K_H

#include <stdint.h>

typedef U8  (*pFunPKTHAND)(U8  *pu8Data, U16  u16Len);
extern U8 u8Xmodem1kClient (pFunPKTHAND   pfunPktHandle, U16 u16ShortDly, U16  u8LongDly);

#endif                                                            
