/*
 * Copyright (c) 2025, CARV ICS FORTH.
 *
 * SPDX-License-Identifier:    BSD-3-Clause
 */

/*
 * @file		init.c
 * @brief		Cross-platform tests for init interface
 */

#include <metal/sys.h>
#include <metal/log.h>

int test_init(void)
{
	struct metal_init_params metal_param = METAL_INIT_DEFAULTS;
	int init_ret;

	init_ret = metal_init(&metal_param);
	metal_set_log_handler(metal_default_log_handler);
	metal_set_log_level(METAL_LOG_ERROR);

	if (init_ret)
		metal_err("metal_init() returned %d\n", init_ret);

	metal_finish();
	return init_ret;
}
