
#include <LPC8xx.h>
#include "IAP.h"

struct __cmd_table 
{
  U32 cmd_code;
  U32 param[4];
};

static struct __cmd_table cmd_table;

struct __result_table 
{
  U32 ret_code;
  U32 result[4];
};

static struct __result_table result_table;
typedef unsigned int (*IAP)(struct __cmd_table*, struct __result_table*);
static const IAP iap_call = (IAP) IAP_ADDRESS;


int iap_erase_sector(unsigned int sector_start, unsigned int sector_end) 
{
  cmd_table.cmd_code = ERASE_SECTOR;
  cmd_table.param[0] = sector_start;
  cmd_table.param[1] = sector_end;

  __disable_irq();
  iap_call(&cmd_table, &result_table);
  __enable_irq();

  return (int)result_table.ret_code;
}

int iap_erase_page(unsigned int page_start, unsigned int page_end)
{
  cmd_table.cmd_code = ERASE_PAGE;
  cmd_table.param[0] = page_start;
  cmd_table.param[1] = page_end;
  cmd_table.param[2] = SystemCoreClock / 1000;

  __disable_irq();
  iap_call(&cmd_table, &result_table);
  __enable_irq();

  return (int)result_table.ret_code;
}

int iap_prepare_sector(unsigned int sector_start, unsigned int sector_end) 
{
  cmd_table.cmd_code = PREPARE_SECTOR;
  cmd_table.param[0] = sector_start;
  cmd_table.param[1] = sector_end;

  __disable_irq();
  iap_call(&cmd_table, &result_table);
  __enable_irq();

  return (int)result_table.ret_code;
}

 ************************************************************************************/
int iap_copy_ram_to_flash(void* ram_address, void* flash_address, unsigned int count) 
{
	cmd_table.cmd_code = COPY_RAM_TO_FLASH;
	cmd_table.param[0] = (U32) flash_address;
	cmd_table.param[1] = (U32) ram_address;
	cmd_table.param[2] = count;
	cmd_table.param[3] = SystemCoreClock / 1000;

	__disable_irq();
	iap_call(&cmd_table, &result_table);
	__enable_irq();

	return (int)result_table.ret_code;
}

int dataCompare (U32  dst, U32  src, U32  no)
{  
    cmd_table.cmd_code = COMPARE;
    cmd_table.param[0] = dst;
    cmd_table.param[1] = src;
    cmd_table.param[2] = no;
    
    __disable_irq();
    iap_call(&cmd_table, &result_table);
    __enable_irq();

    return (int)result_table.ret_code;
}
 
