/*
 * Copyright (c) 2025, BlackBerry Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	qnx/shmem.c
 * @brief	QNX libmetal shared memory handling.
 */

#include <metal/shmem.h>
#include <metal/utilities.h>

static void metal_shmem_io_close(struct metal_io_region *io)
{
	metal_unmap(io->virt, io->size);
	free((void *)io->physmap);
}

static const struct metal_io_ops metal_shmem_io_ops = {
	NULL, NULL, NULL, NULL, NULL, metal_shmem_io_close, NULL, NULL
};

static int metal_shmem_try_map(int fd, size_t size, struct metal_io_region **result)
{
	size_t pages, page, phys_size;
	struct metal_io_region *io;
	metal_phys_addr_t *phys;
	void *mem;
	uint32_t *virt;
	int ret;

	size = metal_align_up(size, _metal.page_size);
	pages = size / _metal.page_size;

	ret = metal_map(fd, 0, size, 1, 0, &mem);
	if (ret) {
		metal_log(METAL_LOG_WARNING,
			  "failed to mmap shmem %ld - %s\n",
			  size, strerror(-ret));
		return ret;
	}

	ret = mlock(mem, size);
	if (ret == -1) {
		metal_log(METAL_LOG_WARNING, "failed to mlock shmem - %s\n",
				strerror(errno));
	}

	phys_size = sizeof(*phys) * pages;
	phys = malloc(phys_size);
	if (!phys) {
		metal_unmap(mem, size);
		return -ENOMEM;
	}
	io = malloc(sizeof(*io));
	if (!io) {
		free(phys);
		metal_unmap(mem, size);
		return -ENOMEM;
	}

	for (virt = mem, page = 0; page < pages; ++page) {
		size_t offset = page * _metal.page_size;

		ret = mem_offset64(virt + offset, NOFD, size, (off64_t *)&phys[page], NULL);
		if (ret) {
			phys[page] = METAL_BAD_OFFSET;
		}
	}
	metal_io_init(io, mem, phys, size, _metal.page_shift, 0,
				&metal_shmem_io_ops);
	*result = io;

	return 0;
}

int metal_shmem_open(const char *name, size_t size,
		     struct metal_io_region **result)
{
	int ret, fd;

	ret = metal_shmem_open_generic(name, size, result);
	if (ret != -ENOENT)
		return ret;

	ret = metal_open(name, 1);
	if (ret < 0) {
		metal_log(METAL_LOG_ERROR, "failed to open %s\n shmem", name);
		return ret;
	}
	fd = ret;

	ret = metal_shmem_try_map(fd, size, result);
	if (ret) {
		metal_log(METAL_LOG_ERROR, "failed to map %s shmem\n", name);
		return ret;
	}

	close(fd);
	return 0;
}
