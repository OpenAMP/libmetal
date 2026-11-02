/*
 * Copyright (c) 2025, BlackBerry Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	qnx/sys.h
 * @brief	QNX system primitives for libmetal.
 */

#ifndef __METAL_SYS__H__
#error "Include metal/sys.h instead of metal/qnx/sys.h"
#endif

#ifndef __METAL_QNX_SYS__H__
#define __METAL_QNX_SYS__H__

#include <fcntl.h>
#include <libgen.h>
#include <limits.h>
#include <metal/cpu.h>
#include <metal/errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <syslog.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

#define METAL_INVALID_VADDR     NULL

#define metal_yield() metal_cpu_yield()

#define __qnx_get_physical_address(addr, len) ({ \
	off64_t qnx_offset = 0; \
	metal_assert(mem_offset64(addr, NOFD, len, &qnx_offset, NULL) == 0); \
	(qnx_offset); \
})

struct metal_device;

/** Structure of qnx specific libmetal runtime state. */
struct metal_state {

	/** Common (system independent) data. */
	struct metal_common_state common;

	/** system page size. */
	unsigned long		page_size;

	/** system page shift. */
	unsigned long		page_shift;
};

#ifdef METAL_INTERNAL
extern int metal_open(const char *path, int shm);
extern int metal_map(int fd, off_t offset, size_t size, int expand,
		     int flags, void **result);
extern int metal_unmap(void *mem, size_t size);

#endif /* METAL_INTERNAL */

#ifdef __cplusplus
}
#endif

#endif /* __METAL_QNX_SYS__H__ */
