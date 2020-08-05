/**
* This file is part of the sboot project.
*
* Copyright 2020, Huang Yang <george_hy@outlook.com>. All rights reserved.
*
* See the COPYING file for the terms of usage and distribution.
*/
#ifndef _FLASH_MAP_H_
#define _FLASH_MAP_H_

#define SBOOT_IMAGE_ADDR                        0x08000000
#define SBOOT_IMAGE_SIZE                        0x00004000
#define APP_IMAGE_ADDR                          0x08004000
#define APP_IMAGE_SIZE                          0x0003E000
#define UPGRADE_IMAGE_HEADER_ADDR               0x08042000
#define UPGRADE_IMAGE_HEADER_SIZE               0x00000008
#define UPGRADE_IMAGE_ADDR                      0x08042008
#define UPGRADE_IMAGE_SIZE                      0x0003DFF8


#endif /* _FLASH_MAP_H_ */
