/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	hosted/cpu.h
 * @brief	Hosted environment CPU specific primitives
 */

#include <metal/sys.h>

#ifndef __METAL_HOSTED_CPU__H__
#define __METAL_HOSTED_CPU__H__

static inline void metal_cpu_yield(void)
{
	metal_wait_usec(10);
}

#endif /* __METAL_HOSTED_CPU__H__ */
