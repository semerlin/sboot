/**
* This file is part of the sboot project.
*
* Copyright 2020, Huang Yang <george_hy@outlook.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#ifndef _DELAY_H_
#define _DELAY_H_

#include "types.h"

BEGIN_DECLS

void delay_us(uint16_t time);
void delay_ms(uint16_t time);

END_DECLS

#endif /* _DELAY_H_ */
