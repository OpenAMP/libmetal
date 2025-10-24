/*
 * Copyright (c) 2025, CARV ICS FORTH.
 *
 * SPDX-License-Identifier:    BSD-3-Clause
 */

/*
 * @file		test_all.c
 * @brief		Main cross-platform testing execution and debug printing
 */

#include <stdio.h>

extern int test_alloc(void);
extern int test_assert(void);
extern int test_cache(void);
extern int test_config(void);
extern int test_cpu(void);
extern int test_init(void);
extern int test_log(void);
extern int test_mutex(void);
extern int test_shmem(void);
extern int test_sleep(void);
extern int test_spinlock(void);
extern int test_utilities(void);

struct test_entry {
	char name[20];
	int (*test)(void);
	int ret;
};

struct test_entry tests[] = {
	{"alloc.c",     test_alloc, 0},
	{"assert.c",    test_assert, 0},
	{"cache.c",     test_cache, 0},
	{"config.c",    test_config, 0},
	{"cpu.c",       test_cpu, 0},
	{"init.c",      test_init, 0},
	{"log.c",       test_log, 0},
	{"mutex.c",     test_mutex, 0},
	{"shmem.c",     test_shmem, 0},
	{"sleep.c",     test_sleep, 0},
	{"spinlock.c",  test_spinlock, 0},
	{"utilities.c", test_utilities, 0}
};

#define NUM_TESTS (sizeof(tests) / sizeof(tests[0]))

int main(void)
{
	unsigned long int i;
	char pass[] = "\033[0;32m [PASS] \033[0m";
	char fail[] = "\033[0;31m [FAIL] \033[0m";

	for (i = 0; i < NUM_TESTS; i++) {
		printf("------%s------\n", tests[i].name);
		tests[i].ret = tests[i].test();
	}
	for (i = 0; i < NUM_TESTS; i++) {
		if (tests[i].ret == 0)
			printf("%-12s %s\n", tests[i].name, pass);
		else
			printf("%-12s %s (code %d)\n", tests[i].name, fail, tests[i].ret);
	}
	return 0;
}
