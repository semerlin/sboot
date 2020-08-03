/**
* This file is part of the sboot project.
*
* Copyright 2020, Huang Yang <george_hy@outlook.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#include "delay.h"

void delay_us(uint16_t time)
{
    uint16_t i = 0;
    while (time--)
    {
        i = 10;
        while (i--);
    }
}

void delay_ms(uint16_t time)
{
    uint16_t i = 0;
    while (time--)
    {
        i = 12000;
        while (i--);
    }
}

