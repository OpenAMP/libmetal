/*
 * Copyright (c) 2026, BlackBerry Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file	qnx/irq.c
 * @brief	QNX libmetal irq operations.
 */

#include <pthread.h>
#include <sched.h>
#include <metal/device.h>
#include <metal/irq.h>
#include <metal/irq_controller.h>
#include <metal/sys.h>
#include <metal/mutex.h>
#include <metal/list.h>
#include <metal/utilities.h>
#include <metal/alloc.h>
#include <sys/neutrino.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#define MAX_IRQS	1024  /* maximum number of irqs */

#define INTR_PULSE_CODE _PULSE_CODE_MINAVAIL

#define INTR_PRIORITY SIGEV_PULSE_PRIO_INHERIT

static int chid; /* QNX interrupt handling thread channel id */

static int self_coid; /* QNX interrupt handling thread connection id */

static bool irq_handling_stop; /* stop interrupts handling */

static pthread_t irq_pthread; /* irq handling thread id */

static struct metal_irq irqs[MAX_IRQS]; /* QNX IRQs array */

static int irqs_ids[MAX_IRQS]; /* QNX registered IRQs IDs array */

static metal_mutex_t irq_lock; /* Mutex to protect irqs_ids and critical sections */

/* Static functions */
static void metal_qnx_irq_set_enable(struct metal_irq_controller *irq_cntr,
				     int irq, unsigned int state);

/* QNX IRQ controller */
static METAL_IRQ_CONTROLLER_DECLARE(qnx_irq_cntr,
				    0, MAX_IRQS,
				    NULL,
				    metal_qnx_irq_set_enable, NULL,
				    irqs);

unsigned int metal_irq_save_disable(void)
{
	/* Avoid deadlock if called from within the ISR thread itself */
	if (pthread_self() == irq_pthread)
		return 0;
	metal_mutex_acquire(&irq_lock);
	return 0;
}

void metal_irq_restore_enable(unsigned int flags)
{
	if (pthread_self() != irq_pthread)
		metal_mutex_release(&irq_lock);
	metal_unused(flags);
}

static void metal_qnx_irq_set_enable(struct metal_irq_controller *irq_cntr,
				     int irq, unsigned int state)
{
	if (irq < irq_cntr->irq_base ||
	    irq >= irq_cntr->irq_base + irq_cntr->irq_num) {
		metal_log(METAL_LOG_ERROR, "%s: invalid irq %d\n",
			  __func__, irq);
		return;
	}

	if (state == METAL_IRQ_ENABLE) {
		struct sigevent event;

		SIGEV_PULSE_INIT(&event, self_coid, INTR_PRIORITY, INTR_PULSE_CODE, irq);
		metal_mutex_acquire(&irq_lock);
		if (irqs_ids[irq] == -1) {
			irqs_ids[irq] = InterruptAttachEvent(irq, &event, 0);
		}
		if (irqs_ids[irq] == -1) {
			metal_log(METAL_LOG_ERROR,
				  "%s: unable to enable irq %d\n", __func__, irq);
		}
		metal_mutex_release(&irq_lock);
	} else if (state == METAL_IRQ_DISABLE) {
		metal_mutex_acquire(&irq_lock);
		if (irqs_ids[irq] == -1) {
			metal_log(METAL_LOG_ERROR,
				  "%s: request to disable irq %d which is not enabled\n",
				  __func__, irq);
		} else if (InterruptDetach(irqs_ids[irq]) == -1) {
			metal_log(METAL_LOG_ERROR,
				  "%s: unable to disable irq %d\n",
				  __func__, irq);
		} else {
			irqs_ids[irq] = -1;
		}
		metal_mutex_release(&irq_lock);
	} else {
		metal_log(METAL_LOG_ERROR,
			  "%s: unknown requested interrupt state %d for irq %d\n",
			  __func__, state, irq);
	}
}

/**
 * @brief       IRQ handler
 * @param[in]   args  not used. required for pthread.
 */
static void *metal_qnx_irq_handling(void *args)
{
	struct _pulse msg;
	int rcvid, irq, irq_id, ret;

	metal_unused(args);

	while (1) {
		/* block here waiting for interrupt pulse */
		rcvid = MsgReceivePulse(chid, &msg, sizeof(msg), NULL);

		if (rcvid == -1) {
			if (!irq_handling_stop) {
				metal_log(METAL_LOG_ERROR,
					  "MsgReceivePulse failed: %s\n", strerror(errno));
			}
			break;
		}

		if (msg.code == _PULSE_CODE_DISCONNECT) {
			ConnectDetach(msg.scoid);
			continue;
		}

		if (msg.code != INTR_PULSE_CODE) {
			metal_log(METAL_LOG_WARNING,
				  "Unhandled pulse code: %d\n", msg.code);
			continue;
		}

		irq = msg.value.sival_int;
		metal_mutex_acquire(&irq_lock);
		irq_id = irqs_ids[irq];

		if (irq_id != -1) {
			metal_log(METAL_LOG_DEBUG,
				  "Received interrupt on vector %d (0x%.4hX)\n",
				  irq, (uint16_t)irq);

			/* call handler for received irq if enabled */
			if (metal_irq_handle(&irqs[irq], irq) != METAL_IRQ_HANDLED) {
				metal_log(METAL_LOG_WARNING,
					  "IRQ %d (0x%.4hX) unhandled\n",
					  irq, (uint16_t)irq);
			}

			/* enable receiving of new interrupts */
			ret = __QNX__ < 800 ? InterruptUnmask(irq, irq_id)
					    : InterruptUnmask(0, irq_id);

			if (ret == -1) {
				metal_log(METAL_LOG_ERROR,
					  "Unable to unmask the interrupt %d (0x%.4hX)\n",
					  irq, (uint16_t)irq);
			}
		} else {
			metal_log(METAL_LOG_ERROR,
				  "Received interrupt on unregistered vector %d (0x%.4hX)\n",
				  irq, (uint16_t)irq);
		}
		metal_mutex_release(&irq_lock);
	}

	return NULL;
}

/**
 * @brief irq handling initialization
 * @return 0 on success, non-zero on failure
 */
int metal_qnx_irq_init(void)
{
	int ret;

	metal_mutex_init(&irq_lock);
	memset(&irqs, 0, sizeof(irqs));

	for (int i = 0; i < MAX_IRQS; i++) {
		irqs_ids[i] = -1;
	}

	chid = ChannelCreate(_NTO_CHF_DISCONNECT | _NTO_CHF_UNBLOCK);
	if (chid == -1) {
		metal_log(METAL_LOG_ERROR,
			  "Unable to create channel: chid %d: %s\n",
			  chid, strerror(errno));
		ret = -EAGAIN;
		goto err_mutex;
	}

	self_coid = ConnectAttach(0, 0, chid, _NTO_SIDE_CHANNEL, _NTO_COF_CLOEXEC);
	if (self_coid == -1) {
		metal_log(METAL_LOG_ERROR,
			  "Unable to attach connection: coid %d: %s\n",
			  self_coid, strerror(errno));
		ret = -EAGAIN;
		goto err_chid;
	}

	irq_handling_stop = false;
	ret = metal_irq_register_controller(&qnx_irq_cntr);
	if (ret < 0) {
		metal_log(METAL_LOG_ERROR,
			  "QNX IRQ controller failed to register.\n");
		ret = -EINVAL;
		goto err_coid;
	}

	ret = pthread_create(&irq_pthread, NULL, metal_qnx_irq_handling, NULL);
	if (ret != EOK) {
		metal_log(METAL_LOG_ERROR, "Failed to create IRQ thread: %d.\n",
			  ret);
		ret = -EAGAIN;
		goto err_coid;
	}

	return 0;

err_coid:
	ConnectDetach(self_coid);
err_chid:
	ChannelDestroy(chid);
err_mutex:
	metal_mutex_deinit(&irq_lock);
	return ret;
}

/**
 * @brief irq handling shutdown
 */
void metal_qnx_irq_shutdown(void)
{
	int ret;

	metal_log(METAL_LOG_DEBUG, "%s\n", __func__);

	metal_mutex_acquire(&irq_lock);
	irq_handling_stop = true;

	for (int irq = 0; irq < MAX_IRQS; irq++) {
		if (irqs_ids[irq] != -1) {
			ret = InterruptDetach(irqs_ids[irq]);
			if (ret == -1) {
				metal_log(METAL_LOG_ERROR,
					  "Unable to detach interrupt %d (id %d): %s\n",
					  irq, irqs_ids[irq], strerror(errno));
			} else {
				irqs_ids[irq] = -1;
			}
		}
	}
	metal_mutex_release(&irq_lock);

	ret = ConnectDetach(self_coid);
	if (ret == -1) {
		metal_log(METAL_LOG_ERROR,
			  "Failed to destroy connection: self_coid %d: %s\n",
			  self_coid, strerror(errno));
	}

	ret = ChannelDestroy(chid);
	if (ret == -1) {
		metal_log(METAL_LOG_ERROR,
			  "Failed to destroy channel: chid %d: %s\n",
			  chid, strerror(errno));
	}

	ret = pthread_join(irq_pthread, NULL);
	if (ret != EOK) {
		metal_log(METAL_LOG_ERROR,
			  "Failed to join IRQ thread: %d.\n", ret);
	}

	metal_mutex_deinit(&irq_lock);
}
