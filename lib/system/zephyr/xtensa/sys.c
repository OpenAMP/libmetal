/*
 * Copyright 2022 NXP
 * Author: Daniel Baluta <daniel.baluta@nxp.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	zephyr/xtensa/sys.c
 * @brief	machine specific system primitives implementation.
 */

#include <metal/io.h>
#include <metal/sys.h>
#include <stdint.h>

/**
 * @brief poll function until some event happens
 */
void metal_weak metal_generic_default_poll(void)
{
	metal_asm __volatile__("waiti 0");
}
