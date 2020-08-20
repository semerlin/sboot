/**
* This file is part of the sboot project.
*
* Copyright 2020, Huang Yang <george_hy@outlook.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#include <string.h>
#include "upgrade_flash.h"
#include "trace.h"
#include "flash_map.h"
#include "stm32f10x.h"
#include "crc32.h"

#define FLASH_MAGIC                 0xdeadbeef
#define FLASH_FAILED_TRY_COUNT      3

static uint8_t image_buffer[FLASH_BLOCK_SIZE];

FLASH_Status flash_page_erase(uint32_t address)
{
    FLASH_Status status = FLASH_COMPLETE;
    uint8_t try_count;
    for (try_count = 0; try_count < FLASH_FAILED_TRY_COUNT; try_count ++)
    {
        status = FLASH_ErasePage(address);
        if (FLASH_COMPLETE != status)
        {
            /* try again */
            TRACE("erase page 0x%08x failed: %d, retry %d...", address, status, try_count);
            continue;
        }

        break;
    }

    return status;
}

static void flash_page_read(uint32_t address, uint8_t *pbuf)
{
    uint32_t *pdata = (uint32_t *)pbuf;
    for (uint32_t i = 0; i < FLASH_BLOCK_SIZE / 4; ++i)
    {
        pdata[i] = *(volatile uint32_t *)(address + i * 4);
    }
}

FLASH_Status flash_page_write(uint32_t address, uint8_t *pbuf)
{
    FLASH_Status status = FLASH_COMPLETE;
    uint32_t *pdata = (uint32_t *)pbuf;
    uint8_t try_count;
    for (uint32_t i = 0; i < FLASH_BLOCK_SIZE / 4; ++i)
    {
        for (try_count = 0; try_count < FLASH_FAILED_TRY_COUNT; try_count ++)
        {
            status = FLASH_ProgramWord(address + i * 4, pdata[i]);
            if (FLASH_COMPLETE != status)
            {
                /* try again */
                TRACE("write address 0x%08x failed: %d, retry %d...", address + i * 4, status, try_count);
                continue;
            }

            break;
        }

        if (try_count >= FLASH_FAILED_TRY_COUNT)
        {
            break;
        }
    }

    return status;
}

uint32_t flash_image_checksum_calc(uint32_t address, uint32_t image_size)
{
    uint32_t block_count = image_size / FLASH_BLOCK_SIZE;
    uint32_t crc_val = 0;
    for (uint32_t i = 0; i < block_count; ++i)
    {
        for (uint32_t j = 0; j < FLASH_BLOCK_SIZE; ++j)
        {
            image_buffer[j] = *(volatile uint8_t *)(address + i * FLASH_BLOCK_SIZE + j);
        }
        crc_val = crc32(crc_val, image_buffer, FLASH_BLOCK_SIZE);
        image_size -= FLASH_BLOCK_SIZE;
    }

    if (image_size > 0)
    {
        for (uint32_t i = 0; i < image_size; ++i)
        {
            image_buffer[i] = *(uint8_t *)(address + block_count * FLASH_BLOCK_SIZE + i);
        }
        crc_val = crc32(crc_val, image_buffer, image_size);
    }

    return crc_val;
}

bool flash_image_check(void)
{
    /* read image header */
    flash_image_header_t header = *(flash_image_header_t *)UPGRADE_IMAGE_HEADER_ADDR;
    if (FLASH_MAGIC != header.magic)
    {
        TRACE("no valid upgrade image");
        return false;
    }

    if (!header.not_obsolete)
    {
        TRACE("upgrade image obsoleted!");
        return false;
    }

    TRACE("valid upgrade image find, size %d", header.image_size);
    return true;
}

void flash_image_header_write(flash_image_header_t *pheader)
{
    FLASH_Unlock();
    flash_page_erase(UPGRADE_IMAGE_HEADER_ADDR);
    pheader->magic = FLASH_MAGIC;
    uint32_t *pdata = (uint32_t *)pheader;
    for (uint8_t i = 0; i < sizeof(flash_image_header_t) / sizeof(uint32_t); ++i)
    {
        FLASH_ProgramWord(UPGRADE_IMAGE_HEADER_ADDR + i * sizeof(uint32_t), *pdata);
        pdata ++;
    }
    FLASH_Lock();
}

bool flash_image_upgrade(void)
{
    TRACE("upgrading...");
    flash_image_header_t *pheader = (flash_image_header_t *)UPGRADE_IMAGE_HEADER_ADDR;
    uint32_t block_count = pheader->image_size / FLASH_BLOCK_SIZE;
    if ((pheader->image_size % FLASH_BLOCK_SIZE) != 0)
    {
        block_count += 1;
    }
    bool ret = true;
    uint32_t addr = 0;
    FLASH_Unlock();
    for (uint32_t i = 0; i < block_count; ++i)
    {
        addr = APP_IMAGE_ADDR + i * FLASH_BLOCK_SIZE;
        TRACE("upgrading block %d, address 0x%08x...", i, addr);
        /* erase current app page */
        if (FLASH_COMPLETE != flash_page_erase(addr))
        {
            TRACE("erase block %d failed!", i);
            ret = false;
            break;
        }
        /* read current page */
        memset(image_buffer, 0, FLASH_BLOCK_SIZE);
        flash_page_read(UPGRADE_IMAGE_ADDR + i * FLASH_BLOCK_SIZE, image_buffer);
        /* write current page */
        if (FLASH_COMPLETE != flash_page_write(APP_IMAGE_ADDR + i * FLASH_BLOCK_SIZE, image_buffer))
        {
            TRACE("write block %d failed!", i);
            ret = false;
            break;
        }
    }
    FLASH_Lock();

    /* check checksum */
    uint32_t checksum = flash_image_checksum_calc(APP_IMAGE_ADDR, pheader->image_size);
    if (checksum != pheader->checksum)
    {
        TRACE("checksum not matched: 0x%08x-0x%08x", checksum, pheader->checksum);
        ret = false;
    }

    if (ret)
    {
        TRACE("upgrade image success!");
        flash_image_header_t header = *pheader;
        header.not_obsolete = 0;
        flash_image_header_write(&header);
    }
    else
    {
        TRACE("upgrade image failed!");
        /* TODO: mark image obsolete? */
    }

    return ret;
}
