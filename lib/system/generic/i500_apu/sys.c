/*
 * Copyright (c) 2020, BayLibre
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	generic/i500_apu/sys.c
 * @brief	machine specific system primitives implementation.
 */

#include <metal/compiler.h>
#include <metal/io.h>
#include <metal/sys.h>
#include <metal/utilities.h>
#include <stdint.h>

#include <xtensa/hal.h>
#include <xtensa/tie/ipu_if2.h>

#if !defined(I500_CORE_ID)
#error "Undefined core id"
#endif

#if I500_CORE_ID == 0
#define BASE_ADDRESS 0x19180000
#elif I500_CORE_ID == 1
#define BASE_ADDRESS 0x19280000
#else
#error "Unknown core id"
#endif

void i500_write_reg(uint32_t addr, uint32_t value)
{
	unsigned int level;

	level = sys_irq_save_disable();
	IPU_SendAPBWrite(value, BASE_ADDRESS + addr);
	sys_irq_restore_enable(level);
}

uint32_t i500_read_reg(uint32_t addr)
{
	uint32_t value = 0;

	IPU_SendAPBRead(value, BASE_ADDRESS + addr);
	value = IPU_ReadAPB();

	return value;
}
