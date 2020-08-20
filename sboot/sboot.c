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
#include "upgrade_flash.h"
#include "stm32f10x.h"
#include "stm32f10x_flash.h"
#include "crc32.h"

typedef void (*app_entry_t)(void);

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

