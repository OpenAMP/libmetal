/*
 * Copyright (c) 2025, CARV ICS FORTH.
 *
 * SPDX-License-Identifier:    BSD-3-Clause
 */

/*
 * @file		mutex.c
 * @brief		Cross-platform tests for mutex interface, pthread based
 */

#define ARRAY_SIZE 100

int sum_m;
metal_mutex_t mut;

struct args_m  {
	int *arr;
	int len1;
	int len2;
};

void *array_sum_m(void *a)
{
	struct args_m *arg = (struct args_m *)a;
	int len1 = arg->len1;
	int len2 = arg->len2;
	int mid = (len2 - len1) / 2;

	for (int i = 0; i <= mid; i++) {
		metal_mutex_acquire(&mut);

		if ((len2 - len1 + 1) % 2 && i == mid) {
			sum_m += arg->arr[len1 + i];
			metal_log(METAL_LOG_DEBUG,
				  "Sum is now %d from adding %d\n",
				  sum_m, arg->arr[len1 + i]);
			metal_mutex_release(&mut);
			break;
		}
		sum_m += arg->arr[len1 + i] + arg->arr[len2 - i];
		metal_log(METAL_LOG_DEBUG,
			  "Sum is now %d from adding %d,%d\n",
			  sum_m, arg->arr[len1 + i], arg->arr[len2 - i]);
		metal_mutex_release(&mut);
		usleep(1);
	}

	return NULL;
}

int test_mutex(void)
{
	struct metal_init_params metal_param = METAL_INIT_DEFAULTS;
	int array[ARRAY_SIZE];

	for (int i = 0; i < ARRAY_SIZE; i++) {
		array[i] = i + 1;
	}

	int length = sizeof(array) / sizeof(int);
	struct args_m args1 = {array, length / 2, length - 1};
	struct args_m args2 = {array, 0, length / 2 - 1};
	pthread_t tid1, tid2;

	metal_init(&metal_param);
	metal_mutex_init(&mut);

	metal_set_log_handler(metal_default_log_handler);
	metal_set_log_level(METAL_LOG_ERROR);

	pthread_create(&tid1,
		       NULL,
		       array_sum_m,
		       &args1);

	pthread_create(&tid2,
		       NULL,
		       array_sum_m,
		       &args2);

	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);

	metal_log(METAL_LOG_INFO,
		  "Sum = %d returned for array with values [%d, %d]\n",
		  sum_m, array[0], array[ARRAY_SIZE - 1]);

	metal_mutex_deinit(&mut);
	metal_finish();

	return 0;
}
