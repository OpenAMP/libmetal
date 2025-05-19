/*
 * Copyright (c) 2025, CARV ICS FORTH.
 *
 * SPDX-License-Identifier:    BSD-3-Clause
 */

/*
 * @file		log.c
 * @brief		Cross-platform tests for log interface
 */

int test_log(void)
{
	struct metal_init_params metal_param = METAL_INIT_DEFAULTS;

	metal_init(&metal_param);

	metal_set_log_handler(metal_default_log_handler);
	metal_set_log_level(METAL_LOG_WARNING);

	metal_log(METAL_LOG_DEBUG,
		  "Metal log level is: %d\n",
		  metal_get_log_level());

	metal_log(METAL_LOG_EMERGENCY, "Emergency log example\n");
	metal_log(METAL_LOG_ALERT, "Alert log example\n");
	metal_log(METAL_LOG_CRITICAL, "Critical log example\n");
	metal_log(METAL_LOG_ERROR, "Error log example\n");
	metal_log(METAL_LOG_WARNING, "Warning log example\n");
	metal_log(METAL_LOG_NOTICE, "Notice log example\n");
	metal_log(METAL_LOG_INFO, "Info log example\n");
	metal_log(METAL_LOG_DEBUG, "Debug log example\n");

	metal_finish();
	return 0;
}
