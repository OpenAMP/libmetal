/*
 * Copyright (c) 2025, CARV ICS FORTH.
 *
 * SPDX-License-Identifier:    BSD-3-Clause
 */

/*
 * @file		spinlock.c
 * @brief		Cross-platform tests for spinlock interface, pthread based
 */

#define ARRAY_SIZE 100

int sum_s;
struct metal_spinlock slock;

struct args  {
	int *arr;
	int len1;
	int len2;
};

void *array_sum_s(void *a)
{
	struct args *arg = (struct args *)a;
	int len1 = arg->len1;
	int len2 = arg->len2;
	int mid = (len2 - len1) / 2;

	for (int i = 0; i <= mid; i++) {
		metal_spinlock_acquire(&slock);

		if ((len2 - len1 + 1) % 2 && i == mid) {
			sum_s += arg->arr[len1 + i];
			metal_log(METAL_LOG_DEBUG,
				  "Sum is now %d from adding %d\n",
				  sum_s, arg->arr[len1 + i]);
			metal_spinlock_release(&slock);
			break;
		}
		sum_s += arg->arr[len1 + i] + arg->arr[len2 - i];
		metal_log(METAL_LOG_DEBUG,
			  "Sum is now %d from adding %d,%d\n",
			  sum_s, arg->arr[len1 + i], arg->arr[len2 - i]);
		metal_spinlock_release(&slock);
		usleep(1);
	}

	return NULL;
}

int test_spinlock(void)
{
	struct metal_init_params metal_param = METAL_INIT_DEFAULTS;
	int array[ARRAY_SIZE];

	for (int i = 0; i < ARRAY_SIZE; i++) {
		array[i] = i + 1;
	}

	int length = sizeof(array) / sizeof(int);
	struct args args1 = {array, length / 2, length - 1};
	struct args args2 = {array, 0, length / 2 - 1};
	pthread_t tid1, tid2;

	metal_init(&metal_param);
	metal_spinlock_init(&slock);

	metal_set_log_handler(metal_default_log_handler);
	metal_set_log_level(METAL_LOG_ERROR);

	pthread_create(&tid1,
		       NULL,
		       array_sum_s,
		       &args1);

	pthread_create(&tid2,
		       NULL,
		       array_sum_s,
		       &args2);

	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);

	metal_log(METAL_LOG_INFO,
		  "Sum = %d returned for array with values [%d, %d]\n",
		  sum_s, array[0], array[ARRAY_SIZE - 1]);

	metal_finish();
	return 0;
}
