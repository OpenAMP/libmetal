/*
 * Copyright (c) 2017, Xilinx Inc. and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*****************************************************************************
 * ipi_latency_demo.c
 * This demo measures the IPI latency between the APU and RPU.
 * This demo does the follwing steps:
 *
 *  1. Get the shared memory device I/O region.
 *  1. Get the TTC timer device I/O region.
 *  2. Get the IPI device I/O region.
 *  3. Register IPI interrupt handler.
 *  4. Write to shared memory to indicate demo starts
 *  5. Reset the APU to RPU TTC counter and then kick IPI to notify the
 *     remote.
 *  6. When it receives IPI interrupt, the IPI interrupt handler to stop
 *     the RPU to APU TTC counter.
 *  7. Accumulate APU to RPU and RPU to APU counter values.
 *  8. Repeat step 5, 6 and 7 for 1000 times
 *  9. Write shared memory to indicate RPU about demo finishes and kick
 *     IPI to notify.
 * 10. Clean up: disable IPI interrupt, deregister the IPI interrupt handler.
 */

#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <metal/atomic.h>
#include <metal/io.h>
#include <metal/device.h>
#include <metal/irq.h>
#include "common.h"

struct ipi_channel {
	struct metal_device *ipi_dev; /**< ipi metal device */
	struct metal_io_region *ipi_io; /**< ipi metal I/O region */
	int ipi_irq; /**< ipi irq id */
	uint32_t ipi_mask; /**< remote IPI mask */
	metal_irq_handler ipi_kick_cb; /**< IPI kick callback */
	void *ipi_kick_priv; /**< IPI kick callback private data */
};

static struct ipi_channel ipi_chnl;

/**
 * @brief ipi_irq_handler() - IPI interrupt handler
 *        It will clear the notified flag to mark it's got an IPI interrupt.
 *        It will stop the RPU->APU timer and will clear the notified
 *        flag to mark it's got an IPI interrupt
 *
 * @param[in] vect_id - IPI interrupt vector ID
 * @param[in/out] priv - communication channel data for this application.
 *
 * @return - If the IPI interrupt is triggered by its remote, it returns
 *           METAL_IRQ_HANDLED. It returns METAL_IRQ_NOT_HANDLED, if it is
 *           not the interrupt it expected.
 *
 */
static int _ipi_irq_handler (int vect_id, void *priv)
{
	uint32_t val;
	struct ipi_channel *chnl = (struct ipi_channel *)priv;
	struct metal_io_region *io;

	(void)vect_id;

	io = chnl->ipi_io;
	val = metal_io_read32(io, IPI_ISR_OFFSET);
	if (val & chnl->ipi_mask) {
		if (chnl->ipi_kick_cb != NULL)
			chnl->ipi_kick_cb(vect_id, chnl->ipi_kick_priv);
		metal_io_write32(io, IPI_ISR_OFFSET,
				chnl->ipi_mask);
		return METAL_IRQ_HANDLED;
	}
	return METAL_IRQ_NOT_HANDLED;
}

static void _enable_ipi_intr(struct ipi_channel *chnl)
{
	metal_irq_enable(chnl->ipi_irq);
	/* Enable IPI interrupt */
	metal_io_write32(chnl->ipi_io, IPI_IER_OFFSET, chnl->ipi_mask);
}

static void _disable_ipi_intr(struct ipi_channel *chnl)
{
	/* disable IPI interrupt */
	metal_io_write32(chnl->ipi_io, IPI_IDR_OFFSET, chnl->ipi_mask);
	metal_irq_disable(ipi_chnl.ipi_irq);
}

void ipi_kick_register_handler(metal_irq_handler hd, void *priv)
{
	ipi_chnl.ipi_kick_cb = hd;
	ipi_chnl.ipi_kick_priv = priv;
}
int init_ipi(void)
{
	struct metal_device *dev;
	struct metal_io_region *io;
	int ret;

	/* Open IPI device */
	ret = metal_device_open(BUS_NAME, IPI_DEV_NAME, &dev);
	if (ret) {
		LPERROR("Failed to open device %s.\n", IPI_DEV_NAME);
		return ret;
	}

	/* Get IPI device IO region */
	io = metal_device_io_region(dev, 0);
	if (!io) {
		LPERROR("Failed to map io region for %s.\n", dev->name);
		ret = -ENODEV;
		metal_device_close(dev);
		return ret;
	}
	ipi_chnl.ipi_dev = dev;
	ipi_chnl.ipi_io = io;

	/* Get the IPI IRQ from the opened IPI device */
	ipi_chnl.ipi_irq = (intptr_t)dev->irq_info;

	ipi_chnl.ipi_mask = IPI_MASK;
	/* disable IPI interrupt */
	_disable_ipi_intr(&ipi_chnl);
	/* clear old IPI interrupt */
	metal_io_write32(io, IPI_ISR_OFFSET, IPI_MASK);
	/* Register IPI irq handler */
	metal_irq_register(ipi_chnl.ipi_irq, _ipi_irq_handler, &ipi_chnl);
	return 0;
}

void deinit_ipi(void)
{
	/* disable IPI interrupt */
	_disable_ipi_intr(&ipi_chnl);
	/* unregister IPI irq handler by setting the handler to 0 */
	metal_irq_unregister(ipi_chnl.ipi_irq);
	if (ipi_chnl.ipi_dev) {
		metal_device_close(ipi_chnl.ipi_dev);
		ipi_chnl.ipi_dev = NULL;
	}
}

void kick_ipi(void *msg)
{
	(void)msg;
	metal_io_write32(ipi_chnl.ipi_io, IPI_TRIG_OFFSET, ipi_chnl.ipi_mask);
}

void disable_ipi_kick(void)
{
	_disable_ipi_intr(&ipi_chnl);
}
void enable_ipi_kick(void)
{
	_enable_ipi_intr(&ipi_chnl);
}
