/*
 * Copyright (c) 2020, BayLibre
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	cpu.h
 * @brief	CPU specific primatives
 */

#ifndef __METAL_XTENSA_CPU__H__
#define __METAL_XTENSA_CPU__H__

#define metal_cpu_yield()

/*
 * The dummy implementation is enough here since xtensa dsp don't
 * support the out of order and multi core support is not implemented.
 */
static inline void __sync_synchronize(void) {}

#endif /* __METAL_XTENSA_CPU__H__ */
