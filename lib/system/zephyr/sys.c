/*
 * Copyright (c) 2022 Qualcomm Innovation Center, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	zephyr/sys.c
 * @brief	Zephyr system primitives for libmetal.
 */

#include <metal/io.h>
#include <metal/sys.h>

#include <zephyr/sys/arch_interface.h>

/**
 * @brief poll function until some event happens
 */
void metal_weak metal_generic_default_poll(void)
{
	arch_cpu_idle();
}
