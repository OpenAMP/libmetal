/*
 * Copyright (c) 2018, Linaro Limited. and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file	generic/cache.h
 * @brief	Generic cache operation primitives for libmetal.
 */

#ifndef __METAL_CACHE__H__
#error "Do not include this file directly, include <metal/cache.h> instead"
#endif

#ifndef __METAL_GENERIC_CACHE__H__
#define __METAL_GENERIC_CACHE__H__

#ifdef __cplusplus
extern "C" {
#endif

void metal_machine_cache_flush(void *addr, unsigned int len);
void metal_machine_cache_invalidate(void *addr, unsigned int len);

static inline void __metal_cache_flush(void *addr, unsigned int len)
{
	metal_machine_cache_flush(addr, len);
}

static inline void __metal_cache_invalidate(void *addr, unsigned int len)
{
	metal_machine_cache_invalidate(addr, len);
}

#ifdef __cplusplus
}
#endif

#endif /* __METAL_GENERIC_CACHE__H__ */
