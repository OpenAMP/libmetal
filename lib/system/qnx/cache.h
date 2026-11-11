/*
 * Copyright (c) 2026, BlackBerry Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file	qnx/cache.h
 * @brief	QNX cache operation primitives for libmetal.
 */

#ifndef __METAL_CACHE__H__
#error "Include metal/cache.h instead of metal/qnx/cache.h"
#endif

#ifndef __METAL_QNX_CACHE__H__
#define __METAL_QNX_CACHE__H__

#include <sys/mman.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline void __metal_cache_flush(void *addr, unsigned int len)
{
	if (!addr || !len)
		return;

	msync(addr, len, MS_CACHE_ONLY | MS_SYNC);
}

static inline void __metal_cache_invalidate(void *addr, unsigned int len)
{
	if (!addr || !len)
		return;

	msync(addr, len, MS_CACHE_ONLY | MS_INVALIDATE);
}

#ifdef __cplusplus
}
#endif

#endif /* __METAL_QNX_CACHE__H__ */
