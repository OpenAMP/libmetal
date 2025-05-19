/*
 * Copyright (c) 2025, CARV ICS FORTH.
 *
 * SPDX-License-Identifier:    BSD-3-Clause
 */

/*
 * @file		config.c
 * @brief		Cross-platform tests for config interface
 */

int test_config(void)
{
	struct metal_init_params metal_param = METAL_INIT_DEFAULTS;

	metal_init(&metal_param);
	metal_set_log_handler(metal_default_log_handler);
	metal_set_log_level(METAL_LOG_INFO);

	metal_log(METAL_LOG_INFO,
		  "Metal version is: %d.%d.%d\n",
		  METAL_VER_MAJOR, METAL_VER_MINOR, METAL_VER_PATCH);

	metal_log(METAL_LOG_INFO,
		  "In string form: %s\n",
		  METAL_VER);

	metal_log(METAL_LOG_INFO,
		  "Metal system identifies as: %s\n",
		  METAL_SYSTEM);

	metal_log(METAL_LOG_INFO,
		  "Metal processor identifies as: %s\n",
		  METAL_PROCESSOR);

	metal_log(METAL_LOG_INFO,
		  "Metal machine identifies as: %s\n",
		  METAL_MACHINE);

	metal_finish();
	return 0;
}
