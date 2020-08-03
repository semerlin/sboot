/**
* This file is part of the sboot project.
*
* Copyright 2020, Huang Yang <george_hy@outlook.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#ifndef _TRACE_H_
#define _TRACE_H_

#include "types.h"

BEGIN_DECLS

#ifdef __ENABLE_TRACE
#ifndef __TRACE_MODULE
#define __TRACE_MODULE   "[trace]"
#endif
/**
 * @brief extern function, used to output message.
 *        if you want to use log system, you need to implement this function
 * @param file: file name
 * @param line: line number
 * @param fmt: trace message
 */
/* external function */
/*
  extern void trace(const char *file, long line, const char *fmt, ...);
  #define TRECE(fmt, ...) trace(__FILE__, STR(__LINE__), fmt, ##__VA_ARGS__)
*/
extern void trace(const char *module, const char *fmt, ...);
extern void trace_dump(const char *module, const char *fmt, const uint8_t *pdata, uint8_t len);
#define TRACE(fmt, ...) trace(__TRACE_MODULE, fmt, ##__VA_ARGS__)
#define TRACE_DUMP(fmt, data, len) trace_dump(__TRACE_MODULE, fmt, data, len)
#else
#define TRACE(fmt, ...)
#define TRACE_DUMP(fmt, data, len)
#endif

END_DECLS

#endif /* _TRACE_H_ */
