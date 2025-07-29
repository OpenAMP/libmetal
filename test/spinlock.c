/*
 * Copyright (c) 2025, CARV ICS FORTH.
 *
 * SPDX-License-Identifier:    BSD-3-Clause
 */

/*
 * @file		spinlock.c
 * @brief		Cross-platform tests for spinlock interface, pthread based
 *
 * Simple test that creates an array with values [1, ARRAY_SIZE] and
 * sums their values using threads and libmetal's spinlock interface
 */

#define ARRAY_SIZE 100

static int sum_s;
static struct metal_spinlock slock;
static int idx;

static int array[ARRAY_SIZE];

void *array_sum_s(void *a)
{
	metal_unused(a);
	while (1) {
		metal_spinlock_acquire(&slock);
		if (idx >= ARRAY_SIZE) {
			metal_spinlock_release(&slock);
			break;
		}
		sum_s += array[idx];
		metal_dbg("Sum is now %d after incrementing by %d\n", sum_s, array[idx]);
		idx++;
		metal_spinlock_release(&slock);
	}

	return NULL;
}

int test_spinlock(void)
{
	struct metal_init_params metal_param = METAL_INIT_DEFAULTS;
	int expected = 0;

	for (int i = 0; i < ARRAY_SIZE; i++) {
		array[i] = i + 1;
	}

	idx = 0;

	metal_init(&metal_param);
	metal_spinlock_init(&slock);

	metal_set_log_handler(metal_default_log_handler);
	metal_set_log_level(METAL_LOG_ERROR);

	metal_test_run(2, array_sum_s, NULL);

	metal_info("Sum = %d returned for array with values [%d, %d]\n",
		   sum_s, array[0], array[ARRAY_SIZE - 1]);

	metal_finish();
	expected = (ARRAY_SIZE * (array[0] + array[ARRAY_SIZE - 1])) / 2;
	if (sum_m != expected) {
		/*Sum of array should be n/2 * (first + last numbers)*/
		metal_err("Array sum is %d instead of %d elements\n", sum_s, expected);
		return 1;
	}
	return 0;
}
