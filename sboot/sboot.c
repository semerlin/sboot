/**
* This file is part of the sboot project.
*
* Copyright 2020, Huang Yang <george_hy@outlook.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#include "sboot.h"
#include "trace.h"
#include "flash_map.h"
#include "stm32f10x.h"
#include "stm32f10x_flash.h"
#include "crc32.h"

#define UPGRADE_MAGIC               0xdeadbeef
#define BLOCK_SIZE                  2048
#define FAILED_TRY_COUNT            3

typedef void (*app_entry_t)(void);
static uint32_t upgrade_buffer[BLOCK_SIZE / 4];
static uint8_t crc_buffer[BLOCK_SIZE];

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
} __PACKED upgrade_image_header_t;

static FLASH_Status sboot_page_erase(uint32_t address)
{
    FLASH_Status status = FLASH_COMPLETE;
    uint8_t try_count;
    for (uint32_t i = 0; i < BLOCK_SIZE / 4; ++i)
    {
        for (try_count = 0; try_count < FAILED_TRY_COUNT; try_count ++)
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

        if (try_count >= FAILED_TRY_COUNT)
        {
            break;
        }
    }

    return status;
}

static void sboot_page_read(uint32_t address)
{
    for (uint32_t i = 0; i < BLOCK_SIZE / 4; ++i)
    {
        upgrade_buffer[i] = *(volatile uint32_t *)(address + i * 4);
    }
}

static FLASH_Status sboot_page_write(uint32_t address)
{
    FLASH_Status status = FLASH_COMPLETE;
    uint8_t try_count;
    for (uint32_t i = 0; i < BLOCK_SIZE / 4; ++i)
    {
        for (try_count = 0; try_count < FAILED_TRY_COUNT; try_count ++)
        {
            status = FLASH_ProgramWord(address + i * 4, upgrade_buffer[i]);
            if (FLASH_COMPLETE != status)
            {
                /* try again */
                TRACE("write address 0x%08x failed: %d, retry %d...", address + i * 4, status, try_count);
                continue;
            }

            break;
        }

        if (try_count >= FAILED_TRY_COUNT)
        {
            break;
        }
    }

    return status;
}

static uint32_t sboot_image_checksum_calc(uint32_t address, uint32_t image_size)
{
    uint32_t block_count = image_size / BLOCK_SIZE;
    uint32_t crc_val = 0;
    for (uint32_t i = 0; i < block_count; ++i)
    {
        for (uint32_t j = 0; j < BLOCK_SIZE; ++j)
        {
            crc_buffer[j] = *(uint8_t *)(address + i * BLOCK_SIZE + j);
        }
        crc_val = crc32(crc_val, crc_buffer, BLOCK_SIZE);
        image_size -= BLOCK_SIZE;
    }

    if (image_size > 0)
    {
        for (uint32_t i = 0; i < BLOCK_SIZE; ++i)
        {
            crc_buffer[i] = *(uint8_t *)(address + block_count * BLOCK_SIZE + i);
        }
        crc_val = crc32(crc_val, crc_buffer, image_size);
    }

    return crc_val;
}

bool sboot_upgrade_image_check(void)
{
    /* read ota image header */
    upgrade_image_header_t header = *(upgrade_image_header_t *)UPGRADE_IMAGE_HEADER_ADDR;
    if (UPGRADE_MAGIC != header.magic)
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

bool sboot_upgrade(void)
{
    TRACE("upgrading...");
    upgrade_image_header_t *pheader = (upgrade_image_header_t *)UPGRADE_IMAGE_HEADER_ADDR;
    uint32_t block_count = pheader->image_size / BLOCK_SIZE + 1;
    bool ret = true;
    FLASH_Unlock();
    for (uint32_t i = 0; i < block_count; ++i)
    {
        TRACE("upgrading block %d...", i);
        /* erase current app page */
        if (FLASH_COMPLETE != sboot_page_erase(APP_IMAGE_ADDR + i * BLOCK_SIZE))
        {
            TRACE("erase block %d failed!", i);
            ret = false;
            break;
        }
        /* read current page */
        sboot_page_read(UPGRADE_IMAGE_ADDR + i * BLOCK_SIZE);
        /* write current page */
        if (FLASH_COMPLETE != sboot_page_write(APP_IMAGE_ADDR + i * BLOCK_SIZE))
        {
            TRACE("write block %d failed!", i);
            ret = false;
            break;
        }
    }
    FLASH_Lock();

    /* check checksum */
    uint32_t checksum = sboot_image_checksum_calc(APP_IMAGE_ADDR, pheader->image_size);
    if (checksum != pheader->checksum)
    {
        TRACE("checksum not matched: 0x%08x-0x%08x", checksum, pheader->checksum);
        ret = false;
    }

    if (ret)
    {
        TRACE("upgrade image success!");
        upgrade_image_header_t header = *pheader;
        header.not_obsolete = 0;
        FLASH_Unlock();
        FLASH_ProgramWord(UPGRADE_IMAGE_HEADER_ADDR + 12, header.flags);
        FLASH_Lock();
    }
    else
    {
        TRACE("upgrade image failed!");
        /* TODO: mark image obsolete? */
    }

    return ret;
}

void sboot_reboot(void)
{
    TRACE("rebooting...");
    __set_FAULTMASK(1);
    NVIC_SystemReset();
}

void sboot_run_app(void)
{
    TRACE("run app...");

    /* Check if valid stack address (RAM address) then jump to user application */
    if (((*(__IO uint32_t *)APP_IMAGE_ADDR) & 0x2FFE0000) == 0x20000000)
    {
        /* disable irq */
        __disable_irq();
        /* get user application */
        uint32_t app_entry_addr = *(__IO uint32_t *)(APP_IMAGE_ADDR + 4);
        app_entry_t app_entry = (app_entry_t)app_entry_addr;
        /* initialize user application's Stack Pointer */
        __set_MSP(*(__IO uint32_t *)APP_IMAGE_ADDR);
        /* jump to user application */
        app_entry();
    }
    else
    {
        TRACE("invalid app image address: 0x%08x", APP_IMAGE_ADDR);
    }
}

