/*
 * Copyright (c) 2015, Xilinx Inc. and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <string.h>
#include <metal/sys.h>

int metal_init(const struct metal_init_params *params)
{
	int error = 0;

	if (atomic_fetch_add(&_metal.common.ref_count, 1) == 0) {
		memset(&_metal.common + 1, 0,
		       sizeof(_metal) - sizeof(_metal.common));

		_metal.common.log_handler = params->log_handler;
		_metal.common.log_level   = params->log_level;

		metal_list_init(&_metal.common.bus_list);
		metal_list_init(&_metal.common.generic_shmem_list);
		metal_list_init(&_metal.common.generic_device_list);

		error = metal_sys_init(params);
		if (error)
			atomic_fetch_sub(&_metal.common.ref_count, 1);
	}

	return error;
}

void metal_finish(void)
{
	if (atomic_fetch_sub(&_metal.common.ref_count, 1) == 1) {
		metal_sys_finish();
		memset(&_metal.common + 1, 0,
		       sizeof(_metal) - sizeof(_metal.common));
	}
}
