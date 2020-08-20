/**
* This file is part of the sboot project.
*
* Copyright 2020, Huang Yang <george_hy@outlook.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#ifndef _UPGRADE_FLASH_H_
#define _UPGRADE_FLASH_H_

#include "types.h"
#include "stm32f10x_flash.h"

BEGIN_DECLS

#define FLASH_BLOCK_SIZE            2048

typedef struct
{
    uint32_t magic;
    uint32_t checksum;
    uint32_t image_size;
    union
    {
        struct
        {
            uint32_t not_obsolete : 1;
            uint32_t rfu : 31;
        };
        uint32_t flags;
    };
} __PACKED flash_image_header_t;


bool flash_image_check(void);
bool flash_image_upgrade(void);
FLASH_Status flash_page_erase(uint32_t address);
FLASH_Status flash_page_write(uint32_t address, uint8_t *pbuf);
void flash_image_header_write(flash_image_header_t *pheader);
uint32_t flash_image_checksum_calc(uint32_t address, uint32_t image_size);


END_DECLS

#endif /* _SBOOT_H_ */


