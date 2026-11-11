/*
 * Copyright (c) 2026, BlackBerry Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
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

static int metal_shmem_map_contiguous(int fd, size_t size, struct metal_io_region **result)
{
	struct metal_io_region *io;
	metal_phys_addr_t *phys;
	off64_t offset64;
	struct stat stat;
	void *mem;
	int ret;

	size = metal_align_up(size, _metal.page_size);

	if (fstat(fd, &stat) != 0) {
		metal_log(METAL_LOG_ERROR,
			  "failed to stat shmem fd - %s\n",
			  strerror(errno));
		return -errno;
	}

	if (stat.st_size == 0) {
		ret = shm_ctl(fd, SHMCTL_ANON | SHMCTL_PHYS, 0, size);
		if (ret == -1 && errno != EBUSY) {
			metal_log(METAL_LOG_ERROR,
				  "failed to make shmem contiguous - %s\n",
				  strerror(errno));
			return -errno;
		}
	} else if ((size_t)stat.st_size < size) {
		metal_log(METAL_LOG_ERROR,
			  "existing shmem size %lld is smaller than requested %zu\n",
			  (long long)stat.st_size, size);
		return -EINVAL;
	}

	ret = metal_map(fd, 0, size, 0, 0, &mem);
	if (ret) {
		metal_log(METAL_LOG_WARNING,
			  "failed to mmap shmem %ld - %s\n",
			  size, strerror(-ret));
		return ret;
	}

	phys = malloc(sizeof(*phys));
	io = malloc(sizeof(*io));
	if (!phys || !io) {
		free(phys);
		free(io);
		metal_unmap(mem, size);
		return -ENOMEM;
	}

	if (mem_offset64(mem, NOFD, size, &offset64, NULL) == 0) {
		*phys = (metal_phys_addr_t)offset64;
	} else {
		free(phys);
		phys = NULL;
	}

	metal_io_init(io, mem, phys, size, (unsigned int)-1, 0,
		      &metal_shmem_io_ops);
	if (result) {
		*result = io;
	} else {
		metal_io_finish(io);
		free(io);
	}

	return 0;
}

int metal_shmem_open(const char *name, size_t size,
		     struct metal_io_region **result)
{
	int ret, fd;

	if (!name || !strlen(name) || !size)
		return -EINVAL;

	ret = metal_shmem_open_generic(name, size, result);
	if (ret != -ENOENT)
		return ret;

	ret = metal_open(name, 1);
	if (ret < 0) {
		metal_log(METAL_LOG_ERROR, "failed to open %s shmem\n", name);
		return ret;
	}
	fd = ret;

	ret = metal_shmem_map_contiguous(fd, size, result);
	if (ret) {
		metal_log(METAL_LOG_ERROR, "failed to map %s shmem\n", name);
		close(fd);
		return ret;
	}

	close(fd);
	return 0;
}
