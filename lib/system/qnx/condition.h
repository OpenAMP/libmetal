/*
 * Copyright (c) 2025, BlackBerry Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	qnx/condition.h
 * @brief	QNX condition variable primitives for libmetal.
 */

#ifndef __METAL_CONDITION__H__
#error "Include metal/condition.h instead of metal/qnx/condition.h"
#endif

#ifndef __METAL_QNX_CONDITION__H__
#define __METAL_QNX_CONDITION__H__

#include <pthread.h>
#include <metal/errno.h>
#include <metal/assert.h>

#ifdef __cplusplus
extern "C" {
#endif

struct metal_condition {
	pthread_cond_t cond;
};

/*
 * METAL_CONDITION_INIT - static metal condition variable initialization
 */
#define METAL_CONDITION_INIT { PTHREAD_COND_INITIALIZER }

static inline void metal_condition_init(struct metal_condition *cv)
{
	metal_assert(pthread_cond_init(&cv->cond, NULL) == EOK);
}

static inline int metal_condition_signal(struct metal_condition *cv)
{
	int ret = pthread_cond_signal(&cv->cond);

	return (ret == EOK) ? 0 : -ret;
}

static inline int metal_condition_broadcast(struct metal_condition *cv)
{
	int ret = pthread_cond_broadcast(&cv->cond);

	return (ret == EOK) ? 0 : -ret;
}

#ifdef __cplusplus
}
#endif

#endif /* __METAL_QNX_CONDITION__H__ */
