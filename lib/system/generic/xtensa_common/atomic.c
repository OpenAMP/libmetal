/*
 * Copyright (c) 2020, BayLibre
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	generic/i500_apu/atomic.c
 * @brief	machine specific atomic primitives implementation.
 */

#include <metal/atomic.h>
#include <metal/irq.h>

#include <stdint.h>
#include <stdbool.h>

unsigned int __atomic_fetch_add_4(volatile void *ptr, unsigned int val,
				  int memorder)
{
	unsigned int tmp;
	unsigned int flags;
	(void)memorder;

	flags = metal_irq_save_disable();
	tmp = *((unsigned int *)ptr);
	*((unsigned int *)ptr) = tmp + val;
	metal_irq_restore_enable(flags);

	return tmp;
}

bool __atomic_compare_exchange_4(volatile void *ptr, void *expected,
				 unsigned int desired, bool weak,
				 int success_memorder, int failure_memorder)
{
	bool ret;
	unsigned int flags;

	flags = metal_irq_save_disable();

	(void)success_memorder;
	(void)failure_memorder;
	(void)weak;
	if (*((unsigned int *)ptr) == *((unsigned int *)expected)) {
		*((unsigned int *)ptr) = desired;
		ret = true;
	} else {
		*((unsigned int *)expected) = *((unsigned int *)ptr);
		ret = false;
	}
	metal_irq_restore_enable(flags);

	return ret;
}
