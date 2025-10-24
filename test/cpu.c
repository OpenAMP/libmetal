/*
 * Copyright (c) 2025, CARV ICS FORTH.
 *
 * SPDX-License-Identifier:    BSD-3-Clause
 */

/*
 * @file		cpu.c
 * @brief		Cross-platform tests for cpu interface
 */

#include <metal/sys.h>
#include <metal/log.h>
#include <metal/cpu.h>

int test_cpu(void)
{
	struct metal_init_params metal_param = METAL_INIT_DEFAULTS;

	metal_init(&metal_param);
	metal_set_log_handler(metal_default_log_handler);
	metal_set_log_level(METAL_LOG_ERROR);

	metal_dbg("CPU Yielding\n");
	metal_cpu_yield();

	metal_finish();
	return 0;
}
