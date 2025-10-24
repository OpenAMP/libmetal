/*
 * Copyright (c) 2025, CARV ICS FORTH.
 *
 * SPDX-License-Identifier:    BSD-3-Clause
 */

/*
 * @file		sleep.c
 * @brief		Cross-platform tests for sleep and time interfaces
 */

#include <metal/sys.h>
#include <metal/log.h>
#include <metal/time.h>
#include <metal/sleep.h>

#define SLEEP_SECONDS 3
#define SLEEP_USECONDS (SLEEP_SECONDS * 1000000)

int test_sleep(void)
{
	struct metal_init_params metal_param = METAL_INIT_DEFAULTS;
	int ret = 0;

	metal_init(&metal_param);
	metal_set_log_handler(metal_default_log_handler);
	metal_set_log_level(METAL_LOG_ERROR);

	metal_info("Time is currently %llu, sleeping for %llu seconds\n",
		   metal_get_timestamp(), SLEEP_SECONDS);

	ret = metal_sleep_usec(SLEEP_USECONDS);

	if (ret) {
		metal_err("Function metal_sleep_usec returned %d\n", ret);
		return 1;
	}

	metal_dbg("Time is now %llu\n", metal_get_timestamp());

	metal_finish();

	return 0;
}
