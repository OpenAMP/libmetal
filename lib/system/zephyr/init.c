/*
 * Copyright (c) 2017, Linaro Limited. and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	zephyr/init.c
 * @brief	Zephyr libmetal initialization.
 */

#include <metal/device.h>
#include <metal/sys.h>
#include <metal/utilities.h>

#include "log.h"

extern int metal_irq_init(void);
extern void metal_irq_deinit(void);

struct metal_state _metal;

int metal_sys_init(const struct metal_init_params *params)
{
	if (params->log_handler == NULL)
		metal_set_log_handler(metal_zephyr_log_handler);

	metal_bus_register(&metal_generic_bus);
	return metal_irq_init();
}

void metal_sys_finish(void)
{
	metal_irq_deinit();
	metal_bus_unregister(&metal_generic_bus);
}
