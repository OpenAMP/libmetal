/*
 * Copyright (c) 2026, BlackBerry Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file	qnx/time.c
 * @brief	QNX libmetal time handling.
 */

#include <time.h>
#include <metal/time.h>

#define NS_PER_S 1000000000ULL

unsigned long long metal_get_timestamp(void)
{
	unsigned long long time = 0;
	struct timespec ts;

	if (clock_gettime(CLOCK_MONOTONIC, &ts) == -1) {
		metal_log(METAL_LOG_ERROR, "%s failed!\n", __func__);
		return time;
	}

	time = ts.tv_sec * NS_PER_S;
	time += ts.tv_nsec;
	return time;
}

