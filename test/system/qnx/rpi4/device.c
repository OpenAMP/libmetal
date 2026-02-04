/*
 * Copyright (c) 2025, BlackBerry Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <unistd.h>

#include "metal-test.h"
#include <metal/device.h>

#define SHM_NAME "shmram"
#define SHM_BASE_ADDR 0x3b3f0000
#define EXPECTED_ID 0x10

#define DEFAULT_PAGE_SHIFT 12
#define DEFAULT_PAGE_MASK (-1UL)

const metal_phys_addr_t shm_phys = {
	SHM_BASE_ADDR,
};

static struct metal_device metal_dev_table[] = {
	{
		/* Shared memory device */
		.name = "shmram",
		.bus = NULL,
		.num_regions = 1,
		.regions = {
			{
				.virt = NULL,
				.physmap = &shm_phys,
				.size = 0x10000,
				.page_shift = DEFAULT_PAGE_SHIFT,
				.page_mask = DEFAULT_PAGE_MASK,
				.mem_flags = MAP_SHARED,
				.ops = {NULL},
			},
		},
		.node = {NULL},
		.irq_num = 0,
		.irq_info = NULL,
	},
};

static int device(void)
{
	struct metal_device *shm_dev = &metal_dev_table[0];
	struct metal_io_region *io;
	int error, idcode;

	error = metal_register_generic_device(shm_dev);
	if (error) {
		metal_log(METAL_LOG_DEBUG, "device registration failed - %s\n",
			strerror(-error));
		return error;
	}

	error = metal_device_open("generic", "shmram", &shm_dev);
	if (error) {
		metal_log(METAL_LOG_DEBUG, "device opening failed - %s\n",
			strerror(-errno));
		return error;
	}

	io = metal_device_io_region(shm_dev, 0);
	if (!io) {
		metal_device_close(shm_dev);
		return -ENODEV;
	}

	idcode = metal_io_read32(io, 0);
	if (idcode != EXPECTED_ID) {
		metal_log(METAL_LOG_DEBUG, "Read id code %d but expected %d\n",
			  idcode, EXPECTED_ID);
  }

	metal_device_close(shm_dev);

	return 0;
}

METAL_ADD_TEST(device)
