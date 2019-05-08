
#include "CRC.h"

U16 u16CRCVerify (const U8 *pu8Data,U32 u32Len,U16 u16CRCInit)
{
    U8  u8i=0;
    U32 u16CRC = u16CRCInit;

    while (u32Len--) {
        u8i = 8;
        u16CRC = u16CRC ^ (((U16)*pu8Data++) << 8);
        do {
            if (u16CRC & 0x8000) {
                u16CRC = u16CRC << 1 ^ 0x1021;
            }else{
                u16CRC = u16CRC << 1;
            }
        } while (--u8i);
    }
    return u16CRC;
}
