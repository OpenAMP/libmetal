/*
 * Copyright (c) 2025, CARV ICS FORTH.
 *
 * SPDX-License-Identifier:    BSD-3-Clause
 */

/*
 * @file		shmem.c
 * @brief		Cross-platform tests for shmem interface
 */

int test_shmem(void)
{
	struct metal_init_params metal_param = METAL_INIT_DEFAULTS;
	struct metal_io_region **res;
	int ret = 0;

	metal_init(&metal_param);
	metal_set_log_handler(metal_default_log_handler);
	metal_set_log_level(METAL_LOG_ERROR);

	ret = metal_shmem_open("/foo", 128, res);
	if (ret < 0) {
		metal_log(METAL_LOG_ERROR,
			  "metal_shmem_open failed with code: %d\n",
			  ret);
		return ret;
	}

	metal_finish();
	return 0;
}
