/*
 * Copyright (c) 2025, BlackBerry Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <pthread.h>

#include "metal-test.h"
#include <metal/log.h>
#include <metal/mutex.h>

static const int mutex_test_count = 1000;

static void *mutex_thread(void *arg)
{
	metal_mutex_t *l = arg;
	int i;

	for (i = 0; i < mutex_test_count; i++) {
		metal_mutex_acquire(l);
		usleep(1);
		metal_mutex_release(l);
	}

	return NULL;
}

static int mutex(void)
{
	metal_mutex_t lock;
	const int threads = 10;
	int error;

	metal_mutex_init(&lock);

	error = metal_run(threads, mutex_thread, &lock);

	metal_mutex_deinit(&lock);

	return error;
}

METAL_ADD_TEST(mutex);
