/*
 * Copyright (c) 2025, BlackBerry Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	qnx/io.h
 * @brief	QNX specific io definitions.
 */

#ifndef __METAL_IO__H__
#error "Include metal/io.h instead of metal/qnx/io.h"
#endif

#ifndef __METAL_QNX_IO__H__
#define __METAL_QNX_IO__H__

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef METAL_INTERNAL

/**
 * @brief memory mapping for an I/O region
 * @param[in] io metal_io_region to map in process address space
 */
int metal_sys_io_mem_map(struct metal_io_region *io);

/**
 * @brief unmapping for an I/O region
 * @param[in] io metal_io_region to unmap
 */
int metal_sys_io_mem_unmap(struct metal_io_region *io);

#endif

#ifdef __cplusplus
}
#endif

#endif /* __METAL_QNX_IO__H__ */
