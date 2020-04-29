/*
 * Copyright (c) 2020, BayLibre
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	generic/i500_apu/sys.c
 * @brief	machine specific system primitives implementation.
 */

#include <metal/irq.h>
#include <metal/irq_controller.h>
#include <metal/sys.h>
#include <metal/utilities.h>

#include <xtensa/xtruntime.h>

#define MAX_IRQS 25
#define VPU_INT_TO_XTENSA_CLR_REG	0x0000011C
#define VPU_INT_MASK_REG		0x0000012C

static struct metal_irq irqs[MAX_IRQS];

static void i500_irq_enable(int32_t number)
{
	uint32_t value;

	sys_irq_enable(number);
	value = i500_read_reg(VPU_INT_MASK_REG);
	value &= ~(1 << number);
	i500_write_reg(VPU_INT_MASK_REG, value);
}

static void i500_irq_disable(uint32_t number)
{
	uint32_t value;

	value = i500_read_reg(VPU_INT_MASK_REG);
	value |= (1 << number);
	i500_write_reg(VPU_INT_MASK_REG, value);
	sys_irq_disable(number);
}

static void i500_irq_clear(uint32_t number)
{
	i500_write_reg(VPU_INT_TO_XTENSA_CLR_REG, 1 << number);
}

static void i500_irq_handler(void *arg)
{
	uint32_t number = (uint32_t)arg;

	if (irqs[number].hd)
		irqs[number].hd((int)number, irqs[number].arg);
	i500_irq_clear(number);
}

static void i500_irq_set_enable(struct metal_irq_controller *cntr,
				  int irq, unsigned int state)
{
	if (irq < cntr->irq_base ||
	    irq >= cntr->irq_base + cntr->irq_num) {
		metal_log(METAL_LOG_ERROR, "%s: invalid irq %d\n",
			  __func__, irq);
		return;
	} else if (state == METAL_IRQ_ENABLE) {
		i500_irq_enable(irq);
		sys_irq_enable((unsigned int)irq);
	} else {
		sys_irq_disable((unsigned int)irq);
		i500_irq_disable(irq);
	}
}

static int i500_irq_register(struct metal_irq_controller *cntr,
			       int irq, metal_irq_handler hd, void *arg)
{
	metal_unused(arg);
	xtos_handler old;

	if (irq < cntr->irq_base ||
	    irq >= cntr->irq_base + cntr->irq_num)
		return -EINVAL;

	if (!hd) {
		xtos_set_interrupt_handler(irq, NULL, NULL, &old);
	} else {
		irqs[irq].hd = hd;
		irqs[irq].arg = arg;
		xtos_set_interrupt_handler(irq, i500_irq_handler,
					   (void *)irq, &old);
	}

	return 0;
}


static METAL_IRQ_CONTROLLER_DECLARE(i500_irq_controller, 0, MAX_IRQS, NULL,
				    i500_irq_set_enable, i500_irq_register,
				    irqs);

int i500_irq_init(void)
{
	i500_write_reg(VPU_INT_MASK_REG, 0xFFFF);

	return metal_irq_register_controller(&i500_irq_controller);
}
