/*
 * Copyright (c) 2025, BlackBerry Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdlib.h>

#include "metal-test.h"
#include <metal/alloc.h>
#include <metal/log.h>

static int alloc(void)
{
	void *ptr;

	ptr = metal_allocate_memory(400);
	if (!ptr) {
		metal_log(METAL_LOG_DEBUG, "failed to allocate memory\n");
		return -errno;
	}

	metal_free_memory(ptr);

	return 0;
}

METAL_ADD_TEST(alloc);
