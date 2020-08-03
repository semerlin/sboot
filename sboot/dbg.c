/**
* This file is part of the sboot project.
*
* Copyright 2020, Huang Yang <george_hy@outlook.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "stm32f10x.h"
#include "dbg.h"


void dbg_init(void)
{
}

static void dbg_putchar(char data)
{
}

static void dbg_putstring(const char *string, uint32_t length)
{
}

#ifdef USE_FULL_ASSERT
void assert_failed(const char *file, const char *line)
{
    dbg_putstring("assert failed: <", 16);
    dbg_putstring((const char *)file, strlen((const char *)file));
    dbg_putstring(" : ", 3);
    dbg_putstring(line, strlen(line));
    dbg_putstring(">\n", 2);
    while (1);
}
#endif

#ifdef __ENABLE_TRACE
void trace(const char *module, const char *fmt, ...)
{
    char buf[80];
    va_list argptr;
    int cnt;
    va_start(argptr, fmt);
    cnt = vsprintf(buf, fmt, argptr);
    va_end(argptr);
    dbg_putstring(buf, cnt);
    dbg_putchar('\n');
}

const uint8_t trace_hex_table[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
void trace_dump(const char *module, const char *fmt, const uint8_t *pdata, uint8_t len)
{
    dbg_putstring(fmt, strlen(fmt));
    for (uint8_t i = 0; i < len; ++i)
    {
        dbg_putchar(trace_hex_table[pdata[i] >> 4]);
        dbg_putchar(trace_hex_table[pdata[i] & 0x0f]);
        dbg_putchar(' ');
    }
    dbg_putchar('\n');
}
#endif
