/*
 * Copyright (c) 2026, BlackBerry Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file	qnx/sleep.h
 * @brief	QNX sleep primitives for libmetal.
 */

#ifndef __METAL_SLEEP__H__
#error "Include metal/sleep.h instead of metal/qnx/sleep.h"
#endif

#ifndef __METAL_QNX_SLEEP__H__
#define __METAL_QNX_SLEEP__H__

#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline int __metal_sleep_usec(unsigned int usec)
{
	return usleep(usec);
}

#ifdef __cplusplus
}
#endif

#endif /* __METAL_QNX_SLEEP__H__ */
