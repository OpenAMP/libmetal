/*
 * Copyright (c) 2025, CARV ICS FORTH.
 *
 * SPDX-License-Identifier:    BSD-3-Clause
 */

/*
 * @file		sleep.c
 * @brief		Cross-platform tests for sleep interface
 */

#define SLEEP_SECONDS 3
#define SLEEP_USECONDS (SLEEP_SECONDS * 1000000)

int test_sleep(void)
{
	struct metal_init_params metal_param = METAL_INIT_DEFAULTS;
	int ret = 0;
	int tmp_time = 0;

	metal_init(&metal_param);
	metal_set_log_handler(metal_default_log_handler);
	metal_set_log_level(METAL_LOG_ERROR);

	metal_log(METAL_LOG_INFO,
		  "Time is currently %d, sleeping for %u seconds\n",
		  time(0), SLEEP_SECONDS);

	tmp_time = time(0);

	ret = metal_sleep_usec(SLEEP_USECONDS);
	if (ret)
		metal_log(METAL_LOG_ERROR,
			  "Function metal_sleep_usec returned %d\n",
			  ret);

	metal_log(METAL_LOG_INFO,
		  "Time is now %d\n",
		  time(0));
	metal_finish();
	return 0;
}
