/*
 * Copyright (c) 2025, CARV ICS FORTH.
 *
 * SPDX-License-Identifier:    BSD-3-Clause
 */

/*
 * @file		mutex.c
 * @brief		Cross-platform tests for mutex interface
 *
 * Simple test that creates an array with values [1, ARRAY_SIZE] and
 * sums their values using threads and libmetal's mutex interface
 */

#include <metal/sys.h>
#include <metal/log.h>
#include <metal/mutex.h>
#include <metal/utilities.h>
#include "metal-test.h"

#define ARRAY_SIZE 100

static int sum_m;
static metal_mutex_t mut;
static int idx;

static int array[ARRAY_SIZE];

void *array_sum_m(void *a)
{
	metal_unused(a);
	while (1) {
		metal_mutex_acquire(&mut);
		if (idx >= ARRAY_SIZE) {
			metal_mutex_release(&mut);
			break;
		}
		sum_m += array[idx];
		metal_dbg("Sum is now %d after incrementing by %d\n", sum_m, array[idx]);
		idx++;
		metal_mutex_release(&mut);
	}

	return NULL;
}

int test_mutex(void)
{
	struct metal_init_params metal_param = METAL_INIT_DEFAULTS;
	int expected = 0;

	for (int i = 0; i < ARRAY_SIZE; i++) {
		array[i] = i + 1;
	}

	idx = 0;

	metal_init(&metal_param);
	metal_mutex_init(&mut);

	metal_set_log_handler(metal_default_log_handler);
	metal_set_log_level(METAL_LOG_ERROR);

	metal_test_run(2, array_sum_m, NULL);

	metal_info("Sum = %d returned for array with values [%d, %d]\n",
		   sum_m, array[0], array[ARRAY_SIZE - 1]);

	expected = (ARRAY_SIZE * (array[0] + array[ARRAY_SIZE - 1])) / 2;
	if (sum_m != expected) {
		/*Sum of array should be n/2 * (first + last numbers)*/
		metal_err("Array sum is %d instead of %d elements\n", sum_m, expected);
		return 1;
	}

	metal_mutex_deinit(&mut);
	metal_finish();

	return 0;
}
