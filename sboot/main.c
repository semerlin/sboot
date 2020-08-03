/**
* This file is part of the sboot project.
*
* Copyright 2020, Huang Yang <george_hy@outlook.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#include "stm32f10x.h"

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
    return 0;
}
