/*
 * Copyright (c) 2016, Xilinx Inc. and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file	freertos/time.c
 * @brief	FreeRTOS libmetal time handling.
 */

#include <FreeRTOS.h>
#include <task.h>
#include <metal/time.h>

unsigned long long metal_get_timestamp(void)
{
	unsigned long long ticks = (unsigned long long)xTaskGetTickCount();

	return (ticks * 1000000ULL) / configTICK_RATE_HZ;
}
