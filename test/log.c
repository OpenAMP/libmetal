/*
 * Copyright (c) 2025, CARV ICS FORTH.
 *
 * SPDX-License-Identifier:    BSD-3-Clause
 */

/*
 * @file		log.c
 * @brief		Cross-platform tests for log interface
 */

#include <metal/sys.h>
#include <metal/log.h>

int test_log(void)
{
	struct metal_init_params metal_param = METAL_INIT_DEFAULTS;

	metal_init(&metal_param);

	metal_set_log_handler(metal_default_log_handler);
	metal_set_log_level(METAL_LOG_WARNING);

	metal_dbg("Metal log level is: %d\n", metal_get_log_level());

	for (int i = 0; i <= METAL_LOG_DEBUG; i++)
		metal_log((enum metal_log_level)i, "Level %d log example\n", i);

	metal_finish();
	return 0;
}
