/*
 * Copyright (c) 2025, CARV ICS FORTH.
 *
 * SPDX-License-Identifier:    BSD-3-Clause
 */

/*
 * @file		time.c
 * @brief		Cross-platform tests for time interface
 */

#define SLEEP_SECONDS 3
#define SLEEP_USECONDS (SLEEP_SECONDS * 1000000)

int test_time(void)
{
	struct metal_init_params metal_param = METAL_INIT_DEFAULTS;
	int ret = 0;
	unsigned long long tmp_time = 0;

	metal_init(&metal_param);
	metal_set_log_handler(metal_default_log_handler);
	metal_set_log_level(METAL_LOG_ERROR);

	metal_log(METAL_LOG_INFO,
		  "Time is currently %ulld, sleeping for %u seconds\n",
		  metal_get_timestamp(), SLEEP_SECONDS);

	tmp_time = metal_get_timestamp();

	ret = metal_sleep_usec(SLEEP_USECONDS);

	if (ret) {
		metal_log(METAL_LOG_ERROR,
			  "Function metal_sleep_usec returned %d\n",
			  ret);
		return 1;
	}

	metal_log(METAL_LOG_DEBUG,
		  "Time is now %ulld\n",
		  metal_get_timestamp());
	metal_finish();
	return 0;
}
