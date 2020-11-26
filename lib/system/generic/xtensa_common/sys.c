/*
 * Copyright (c) 2020, BayLibre
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	generic/xtensa_common/sys.c
 * @brief	machine specific system primitives implementation.
 */

#include <metal/compiler.h>
#include <metal/io.h>
#include <metal/sys.h>
#include <metal/utilities.h>

#include <xtensa/hal.h>

void metal_machine_cache_flush(void *addr, unsigned int len)
{
	xthal_dcache_block_writeback(addr, len);
}

void metal_machine_cache_invalidate(void *addr, unsigned int len)
{
	xthal_dcache_block_invalidate(addr, len);
}

void metal_generic_default_poll(void)
{
	XT_WAITI(0);
}

void *metal_machine_io_mem_map(void *va, metal_phys_addr_t pa,
			       size_t size, unsigned int flags)
{
	metal_unused(pa);
	metal_unused(size);
	metal_unused(flags);

	return va;
}
