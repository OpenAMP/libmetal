/*
 * Copyright (c) 2025, BlackBerry Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	qnx/io.c
 * @brief	QNX libmetal io operations.
 */

#include <metal/io.h>
#include <metal/sys.h>

int metal_sys_io_mem_map(struct metal_io_region *io)
{
	if (io->virt)
		return 0;

	void *addr;
	int flags, prot;

	flags = (io->mem_flags & ~PROT_MASK) | MAP_PHYS;
	prot = (io->mem_flags & PROT_MASK) | PROT_READ | PROT_WRITE;
	addr = mmap64(NULL, io->size, prot, flags, NOFD, *((off64_t *)io->physmap));
	if (addr == MAP_FAILED)
		return -errno;

	io->virt = addr;
	return 0;
}

int metal_sys_io_mem_unmap(struct metal_io_region *io)
{
	int ret;

	if (!io->virt)
		return 0;

	ret = munmap(io->virt, io->size);
	if (ret != 0)
		return -errno;

	return 0;
}
