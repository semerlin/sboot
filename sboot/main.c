/**
* This file is part of the sboot project.
*
* Copyright 2020, Huang Yang <george_hy@outlook.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#include "stm32f10x.h"
#include "dbg.h"
#include "sboot.h"
#include "upgrade_flash.h"

/**
 * @brief config board hardware
 */
static void board_cfg(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    /* Configure HCLK clock as SysTick clock source. */
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
}

int main(int argc, char **argv)
{
    board_cfg();
    dbg_init();

    /* check image */
    if (flash_image_check())
    {
        if (flash_image_upgrade())
        {
            sboot_reboot();
        }
        else
        {
            sboot_run_app();
        }
    }
    else
    {
        sboot_run_app();
    }

    return 0;
}
