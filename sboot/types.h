/**
* This file is part of the sboot project.
*
* Copyright 2020, Huang Yang <george_hy@outlook.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#ifndef _TYPES_H_
#define _TYPES_H_

#include "macros.h"
#include <stdbool.h>
#include <stdint.h>

#ifndef FALSE
#define FALSE    (0)
#endif

#ifndef TRUE
#define TRUE     (!FALSE)
#endif

#ifndef NULL
#ifdef __cplusplus
#define NULL    (0L)
#else
#define NULL    ((void *)0)
#endif
#endif


#endif /* _TYPES_H_ */
