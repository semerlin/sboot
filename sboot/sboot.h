/**
* This file is part of the sboot project.
*
* Copyright 2020, Huang Yang <george_hy@outlook.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#ifndef _SBOOT_H_
#define _SBOOT_H_

#include "types.h"

BEGIN_DECLS

bool sboot_upgrade_image_check(void);
void sboot_upgrade(void);
void sboot_reboot(void);
void sboot_run_app(void);


END_DECLS

#endif /* _SBOOT_H_ */


