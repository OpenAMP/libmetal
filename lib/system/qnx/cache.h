/*
 * Copyright (c) 2025, BlackBerry Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	qnx/cache.h
 * @brief	QNX cache operation primitives for libmetal.
 */

#ifndef __METAL_CACHE__H__
#error "Include metal/cache.h instead of metal/qnx/cache.h"
#endif

#ifndef __METAL_QNX_CACHE__H__
#define __METAL_QNX_CACHE__H__

#include <stdlib.h>
#include <sys/mman.h>
#include <sys/cache.h>
#include <metal/assert.h>
#include <metal/sys.h>

#ifdef __cplusplus
extern "C" {
#endif

extern struct cache_ctrl __qnx_cache_control;

static inline void __metal_cache_flush(void *addr, unsigned int len)
{
	CACHE_FLUSH(&__qnx_cache_control, addr, (uint64_t)__qnx_get_physical_address(addr, len), len);
}

static inline void __metal_cache_invalidate(void *addr, unsigned int len)
{
	CACHE_INVAL(&__qnx_cache_control, addr, (uint64_t)__qnx_get_physical_address(addr, len), len);
}

#ifdef __cplusplus
}
#endif

#endif /* __METAL_QNX_CACHE__H__ */
