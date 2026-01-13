/*
 * Copyright (c) 2025, BlackBerry Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <pthread.h>

#include "metal-test.h"
#include <metal/log.h>
#include <metal/spinlock.h>

static const int spinlock_test_count = 1000;
static unsigned int total;

static void *spinlock_thread(void *arg)
{
	struct metal_spinlock *l = arg;
	int i;

	for (i = 0; i < spinlock_test_count; ++i) {
		metal_spinlock_acquire(l);
		total++;
		metal_spinlock_release(l);
	}

	return NULL;
}

static int spinlock(void)
{
	struct metal_spinlock lock = METAL_SPINLOCK_INIT;
	const int threads = 10;
	int value, error;

	error = metal_run(threads, spinlock_thread, &lock);
	if (!error) {
		value = total;
		value -= spinlock_test_count * threads;
		if (value) {
			metal_log(METAL_LOG_DEBUG, "counter mismatch, detla = %d\n",
				  value);
			error = -EINVAL;
		}
	}

	return error;
}

METAL_ADD_TEST(spinlock);
