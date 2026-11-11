/*
 * Copyright (c) 2026, BlackBerry Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file	qnx/condition.c
 * @brief	QNX libmetal condition variable handling.
 */

#include <metal/condition.h>

int metal_condition_wait(struct metal_condition *cv, metal_mutex_t *m)
{
	int ret;

	/* Check if the mutex has been acquired */
	if (!cv || !m || !metal_mutex_is_acquired(m))
		return -EINVAL;

	ret = pthread_cond_wait(&cv->cond, m);

	return (ret == EOK) ? 0 : -ret;
}
