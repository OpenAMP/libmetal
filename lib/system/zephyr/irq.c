/*
 * Copyright (c) 2017, Linaro Limited. and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	zephyr/irq.c
 * @brief	Zephyr libmetal irq definitions.
 */

#include <metal/errno.h>
#include <metal/irq_controller.h>
#include <metal/irq.h>
#include <metal/sys.h>
#include <metal/log.h>
#include <metal/mutex.h>
#include <metal/list.h>
#include <metal/utilities.h>
#include <metal/alloc.h>

#ifdef CONFIG_DYNAMIC_INTERRUPTS
#define MAX_IRQS CONFIG_NUM_IRQS

static struct metal_irq irqs[MAX_IRQS];
#endif /* CONFIG_DYNAMIC_INTERRUPTS */

unsigned int metal_irq_save_disable(void)
{
	return irq_lock();
}

void metal_irq_restore_enable(unsigned int flags)
{
	irq_unlock(flags);
}

#ifdef CONFIG_DYNAMIC_INTERRUPTS
static void metal_zephyr_irq_set_enable(struct metal_irq_controller *irq_cntr,
				     int irq, unsigned int state)
{
	if (irq < irq_cntr->irq_base ||
	    irq >= irq_cntr->irq_base + irq_cntr->irq_num) {
		metal_log(METAL_LOG_ERROR,
			  "%s: invalid irq %d\n", __func__, irq);
		return;
	} else if (state == METAL_IRQ_ENABLE) {
		irq_enable(irq);
	} else {
		irq_disable(irq);
	}
}

static void metal_zephyr_irq_isr(const void *arg)
{
	unsigned int vector = (unsigned int) arg;

	if (vector >= MAX_IRQS) {
		return;
	}
	(void) metal_irq_handle(&irqs[vector], (int)vector);
}

static int metal_zephyr_irq_register(struct metal_irq_controller *cntr, int irq,
				     metal_irq_handler hd, void *arg)
{
	irqs[irq].hd = hd;
	irqs[irq].arg = arg;
	irq_connect_dynamic(irq, 0, metal_zephyr_irq_isr, (void *)irq, 0);
	return 0;
}

static METAL_IRQ_CONTROLLER_DECLARE(zephyr_irq_cntr,
				    0, MAX_IRQS,
				    NULL,
				    metal_zephyr_irq_set_enable,
				    metal_zephyr_irq_register,
				    irqs);
#endif	/* CONFIG_DYNAMIC_INTERRUPTS */

int metal_zephyr_irq_init(void)
{
#ifdef CONFIG_DYNAMIC_INTERRUPTS
	int ret;

	ret =  metal_irq_register_controller(&zephyr_irq_cntr);
	if (ret < 0) {
		metal_log(METAL_LOG_ERROR,
			  "%s: register irq controller failed.\n", __func__);
		return ret;
	}
#endif	/* CONFIG_DYNAMIC_INTERRUPTS */
	return 0;
}
