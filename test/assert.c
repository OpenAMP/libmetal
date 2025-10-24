/*
 * Copyright (c) 2025, CARV ICS FORTH.
 *
 * SPDX-License-Identifier:    BSD-3-Clause
 */

/*
 * @file		assert.c
 * @brief		Cross-platform tests for assert interface
 */

#include <metal/assert.h>

int test_assert(void)
{
	metal_assert(1);

	return 0;
}
