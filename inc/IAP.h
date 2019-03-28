
#ifndef __IAP_H__                                                    
#define __IAP_H__                                                  

#define IAP_ADDRESS            0x1FFF1FF1

#define PREPARE_SECTOR      		50
#define COPY_RAM_TO_FLASH   	51
#define ERASE_SECTOR        		52
#define BLANK_CHECK_SECTOR  	53
#define READ_PART_ID        		54
#define READ_BOOT_CODE_REV  	55
#define COMPARE             			56
#define REINVOKE_ISP        		57
#define READ_UID            			58
#define ERASE_PAGE          			59


typedef enum 
{
    CMD_SUCCESS = 0,
    INVALID_COMMAND,
    SRC_ADDR_ERROR,
    DST_ADDR_ERROR,
    SRC_ADDR_NOT_MAPPED,
    DST_ADDR_NOT_MAPPED,
    COUNT_ERROR,
    INVALID_SECTOR,
    SECTOR_NOT_BLANK,
    SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION,
    COMPARE_ERROR,
    BUSY,
} __e_iap_status;


int iap_erase_sector(unsigned int sector_start, unsigned int sector_end);
int iap_erase_page(unsigned int page_start, unsigned int page_end);
int iap_prepare_sector(unsigned int sector_start, unsigned int sector_end);
int iap_copy_ram_to_flash(void* ram_address, void* flash_address,unsigned int count);
int dataCompare (U32  dst, U32  src, U32  no) ;

#endif

