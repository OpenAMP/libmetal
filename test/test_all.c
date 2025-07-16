/*
 * Copyright (c) 2025, CARV ICS FORTH.
 *
 * SPDX-License-Identifier:    BSD-3-Clause
 */

/*
 * @file		test_all.c
 * @brief		Main cross-platform testing execution and debug printing
 */

#include "test_all.h"

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
