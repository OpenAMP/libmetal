/*
 * Copyright (c) 2020, BayLibre
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	generic/i500_apu/sys.h
 * @brief	generic i500_apu system primitives for libmetal.
 */

#ifndef __METAL_GENERIC_SYS__H__
#error "Include metal/sys.h instead of metal/system/generic/@PROJECT_MACHINE@/sys.h"
#endif

#include <metal/system/@PROJECT_SYSTEM@/xtensa_common/sys.h>

#ifndef __METAL_I500_APU_SYS__H__
#define __METAL_I500_APU_SYS__H__

#ifdef __cplusplus
extern "C" {
#endif

void i500_write_reg(uint32_t addr, uint32_t value);
uint32_t i500_read_reg(uint32_t addr);

int i500_irq_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __METAL_I500_APU_SYS__H__ */
