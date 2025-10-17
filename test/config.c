/*
 * Copyright (c) 2025, CARV ICS FORTH.
 *
 * SPDX-License-Identifier:    BSD-3-Clause
 */

/*
 * @file		config.c
 * @brief		Cross-platform tests for config interface
 */

#include <metal/sys.h>
#include <metal/log.h>
#include <metal/config.h>

int test_config(void)
{
	struct metal_init_params metal_param = METAL_INIT_DEFAULTS;

	metal_init(&metal_param);
	metal_set_log_handler(metal_default_log_handler);
	metal_set_log_level(METAL_LOG_INFO);

	metal_info("Metal version: %d.%d.%d\n", METAL_VER_MAJOR, METAL_VER_MINOR, METAL_VER_PATCH);
	metal_info("In string form: %s\n", METAL_VER);

	metal_info("Metal system identifies as: %s\n", METAL_SYSTEM);
	metal_info("Metal processor identifies as: %s\n", METAL_PROCESSOR);
	metal_info("Metal machine identifies as: %s\n", METAL_MACHINE);

	metal_finish();
	return 0;
}
