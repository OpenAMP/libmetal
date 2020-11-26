/*
 * Copyright (c) 2020, BayLibre
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	generic/xtensa_common/sys.h
 * @brief	generic xtensa_common system primitives for libmetal.
 */

#ifndef __METAL_GENERIC_SYS__H__
#error "Include metal/sys.h instead of metal/system/generic/xtensa_common/sys.h"
#endif

#include <xtensa/xtruntime.h>
#include <xtensa/config/core-isa.h>

#ifndef __METAL_XTENSA_SYS__H__
#define __METAL_XTENSA_SYS__H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef METAL_INTERNAL

static inline void sys_irq_enable(unsigned int vector)
{
	_xtos_interrupt_enable(vector);
}

static inline void sys_irq_disable(unsigned int vector)
{
	_xtos_interrupt_disable(vector);
}

static inline void sys_irq_restore_enable(unsigned int flags)
{
	XTOS_RESTORE_INTLEVEL(flags);
}

static inline unsigned int sys_irq_save_disable(void)
{
	return XTOS_DISABLE_EXCM_INTERRUPTS;
}

#endif /* METAL_INTERNAL */

#ifdef __cplusplus
}
#endif

#endif /* __METAL_XTENSA_SYS__H__ */
