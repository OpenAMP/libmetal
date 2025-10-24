/*
 * Copyright (c) 2025, CARV ICS FORTH
 * Copyright (c) 2016, Xilinx Inc. and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <metal/sys.h>
#include <metal/utilities.h>
#include "metal-test.h"

int metal_test_run(int threads, metal_thread_t child, void *arg)
{
	metal_unused(threads);
	metal_unused(child);
	metal_unused(arg);
	metal_err("%s is currently not implemented for Zephyr\n", __func__);
	return 1;
}

int metal_test_run_noblock(int threads, metal_thread_t child,
			   void *arg, void *tids, int *threads_out)
{
	metal_unused(threads);
	metal_unused(child);
	metal_unused(arg);
	metal_unused(tids);
	metal_unused(threads_out);
	metal_err("%s is currently not implemented for Zephyr\n", __func__);
	return 1;
}

void metal_test_finish_threads(int threads, void *tids)
{
	metal_unused(threads);
	metal_unused(tids);
	metal_err("%s is currently not implemented for Zephyr\n", __func__);
}
