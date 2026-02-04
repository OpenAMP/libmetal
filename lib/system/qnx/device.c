/*
 * Copyright (c) 2025, BlackBerry Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	qnx/device.c
 * @brief	QNX libmetal device operations.
 */

#include <metal/device.h>

int metal_generic_dev_sys_open(struct metal_device *dev)
{
	struct metal_io_region *io;
	unsigned int i, ret;

	/* map I/O memory regions */
	for (i = 0; i < dev->num_regions; i++) {
		io = &dev->regions[i];
		ret = metal_sys_io_mem_map(io);
		if (ret != 0)
			return ret;
	}

	return 0;
}

