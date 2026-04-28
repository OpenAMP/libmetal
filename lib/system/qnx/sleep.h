/*
 * Copyright (c) 2025, BlackBerry Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	qnx/sleep.h
 * @brief	QNX sleep primitives for libmetal.
 */

#ifndef __METAL_SLEEP__H__
#error "Include metal/sleep.h instead of metal/qnx/sleep.h"
#endif

#ifndef __METAL_QNX_SLEEP__H__
#define __METAL_QNX_SLEEP__H__

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NS_PER_US (1000)

static inline int __metal_sleep_usec(unsigned int usec)
{
	struct timespec ts;

	ts.tv_sec = 0;
	ts.tv_nsec = usec * NS_PER_US;
	return nanosleep(&ts, NULL);
}

#ifdef __cplusplus
}
#endif

#endif /* __METAL_QNX_SLEEP__H__ */
