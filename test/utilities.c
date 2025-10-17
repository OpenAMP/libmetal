/*
 * Copyright (c) 2025, CARV ICS FORTH.
 *
 * SPDX-License-Identifier:    BSD-3-Clause
 */

/*
 * @file		utilities.c
 * @brief		Cross-platform tests for utilities interface
 */

#include <metal/sys.h>
#include <metal/log.h>
#include <metal/utilities.h>

struct example {
	int num;
	char c[4];
	int *ptr;
};

int test_utilities(void)
{
	struct metal_init_params metal_param = METAL_INIT_DEFAULTS;
	int array[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	char c_array[7] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g' };
	int error = 0;

	metal_init(&metal_param);
	metal_set_log_handler(metal_default_log_handler);
	metal_set_log_level(METAL_LOG_ERROR);

	if (MB != 1048576 || GB != 1073741824) {
		error = 1;
		metal_err("1 MiB is %ld instead of 1048576, 1 GiB is %ld instead of 1073741824\n",
			  MB, GB);
	}
	if (metal_dim(array) != 10) {
		error = 2;
		metal_err("Array has %d instead of 10 elements\n", metal_dim(array));
	}
	if (metal_dim(c_array) != 7) {
		error = 3;
		metal_err("Character array has %d instead of 7 elements\n", metal_dim(c_array));
	}
	if (metal_min(10, 4) != 4 || metal_max(198, 2) != 198) {
		error = 4;
		metal_err("Minimum between 10 and 4 is %d instead of 4 and maximum between 198 and 2 is %d instead of 198\n",
			  metal_min(10, 4), metal_max(198, 2));
	}
	if (metal_min(-10, -4) != -10 || metal_max(-198, -2) != -2) {
		error = 5;
		metal_err("Minimum between -10 and -4 is %d instead of -10 and maximum between -198 and -2 is %d instead of -2\n",
			  metal_min(-10, -4), metal_max(-198, -2));
	}
	if (metal_align_down(10, 4) != 8 || metal_align_up(10, 4) != 12) {
		error = 6;
		metal_err("10 aligned down to ^4 is %d instead of 8, up to ^4 is %d instead of 12\n",
			  metal_align_down(10, 4), metal_align_up(10, 4));
	}
	if (metal_div_round_down(10, 3) != 3 || metal_div_round_up(10, 3) != 4) {
		error = 7;
		metal_err("10 divided by 3 rounded down is %d instead of 3 and rounded up is %d instead of 4\n",
			  metal_div_round_down(10, 3), metal_div_round_up(10, 3));
	}
	if (metal_div_round_down(-10, 3) != -4 || metal_div_round_up(-10, 3) != -3) {
		error = 8;
		metal_err("-10 divided by 3 rounded down is %d instead of -4 and rounded up is %d instead of -3\n",
			  metal_div_round_down(-10, 3), metal_div_round_up(-10, 3));
	}
	if (metal_offset_of(struct example, ptr) != 8) {
		error = 9;
		metal_err("Offset of test struct is %d instead of 8\n",
			  metal_offset_of(struct example, ptr));
	}
	metal_finish();
	return error;
}
