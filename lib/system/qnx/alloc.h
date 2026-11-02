/*
 * Copyright (c) 2025, BlackBerry Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	qnx/alloc.h
 * @brief	QNX libmetal memory allocation definitions.
 */

#ifndef __METAL_ALLOC__H__
#error "Include metal/alloc.h instead of metal/qnx/alloc.h"
#endif

#ifndef __METAL_QNX_ALLOC__H__
#define __METAL_QNX_ALLOC__H__

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline void *__metal_allocate_memory(unsigned int size)
{
	return malloc(size);
}

static inline void __metal_free_memory(void *ptr)
{
	free(ptr);
}

#ifdef __cplusplus
}
#endif

#endif /* __METAL_QNX_ALLOC__H__ */
