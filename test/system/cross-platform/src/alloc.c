/*
 * Copyright (c) 2025, CARV ICS FORTH.
 *
 * SPDX-License-Identifier:    BSD-3-Clause
 */

/*
 * @file		alloc.c
 * @brief		Cross-platform tests for alloc interface
 */

int test_alloc(void)
{
	struct metal_init_params metal_param = METAL_INIT_DEFAULTS;
	int *mem_ptr;

	metal_init(&metal_param);
	metal_set_log_handler(metal_default_log_handler);
	metal_set_log_level(METAL_LOG_ERROR);

	mem_ptr = metal_allocate_memory(16);
	if (!mem_ptr) {
		metal_log(METAL_LOG_ERROR, "metal_allocate_memory(16) returned null\n");
		return 1;
	}

	metal_log(METAL_LOG_DEBUG,
		  "Writing -2147483648, -1, 0, 2147483647 to allocated memory\n");

	mem_ptr[0] = -2147483648;
	mem_ptr[1] = -1;
	mem_ptr[2] = 0;
	mem_ptr[3] = 2147483647;

	if (mem_ptr[0] != -2147483648 || mem_ptr[1] != -1 ||
	    mem_ptr[2] != 0 || mem_ptr[3] != 2147483647) {

		metal_log(METAL_LOG_ERROR,
			  "Allocated memory has values %d, %d, %d, %d instead of -2147483648, -1 , 0, 2147483647\n",
			  mem_ptr[0], mem_ptr[1], mem_ptr[2], mem_ptr[3]);
		return 1;
	}

	metal_free_memory(mem_ptr);

	metal_finish();
	return 0;
}
