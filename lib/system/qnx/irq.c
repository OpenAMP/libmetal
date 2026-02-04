/*
 * Copyright (c) 2025, BlackBerry Limited. All rights reserved.
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
#include <sys/time.h>
#include <sched.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <poll.h>
#include <unistd.h>

#define MAX_IRQS	1024  /* maximum number of irqs */

#define INTR_PULSE_CODE _PULSE_CODE_MINAVAIL

#define INTR_PRIORITY SIGEV_PULSE_PRIO_INHERIT

static int chid; /* QNX interrupt handling thread channel id */

static int self_coid; /* QNX interrupt handling thread connectionn id */

static bool irq_handling_stop; /* stop interrupts handling */

static pthread_t irq_pthread; /* irq handling thread id */

static struct metal_irq irqs[MAX_IRQS]; /* QNX IRQs array */

static int irqs_ids[MAX_IRQS]; /* QNX registered IRQs IDs array */

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
	InterruptDisable();
	return 0;
}

void metal_irq_restore_enable(unsigned int flags)
{
	InterruptEnable();
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
		irqs_ids[irq] = InterruptAttachEvent(irq, &event, _NTO_INTR_FLAGS_NO_UNMASK);
		if (irqs_ids[irq] == -1) {
			metal_log(METAL_LOG_ERROR,
				  "%s: unable to enable irq %d\n", __func__, irq);
		}
	} else if (state == METAL_IRQ_DISABLE) {
		if (irqs_ids[irq] == -1) {
			metal_log(METAL_LOG_ERROR,
				  "%s: request to disable irq %d which is not enabled\n",
				  __func__, irq);
		} else if (InterruptDetach(irqs_ids[irq]) == -1) {
			metal_log(METAL_LOG_ERROR,
				  "%s: unable to disable irq %d\n",
				  __func__, irq);
		}
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

	metal_unused(args);

	while (1) {
		/* block here waiting for interrupt pulse */
		int rcvid = MsgReceivePulse(chid, &msg, sizeof(msg), NULL);

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

		int irq = msg.value.sival_int;
		int irq_id = irqs_ids[irq];

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
			int ret = __QNX__ < 800 ? InterruptUnmask(irq, irq_id) : InterruptUnmask(0, irq_id);

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

	memset(&irqs, 0, sizeof(irqs));

	for (int i = 0; i < MAX_IRQS; i++) {
		irqs_ids[i] = -1;
	}

	chid = ChannelCreate(_NTO_CHF_DISCONNECT | _NTO_CHF_UNBLOCK);
	if (chid == -1) {
		metal_log(METAL_LOG_ERROR,
			  "Unable to create channel: chid %d: %s\n",
			  chid, strerror(errno));
		return -EAGAIN;
	}

	self_coid = ConnectAttach(0, 0, chid, _NTO_SIDE_CHANNEL, _NTO_COF_CLOEXEC);
	if (self_coid == -1) {
		metal_log(METAL_LOG_ERROR,
			  "Unable to attach connection: coid %d: %s\n",
			  self_coid, strerror(errno));
		return -EAGAIN;
	}

	irq_handling_stop = false;
	ret = metal_irq_register_controller(&qnx_irq_cntr);
	if (ret < 0) {
		metal_log(METAL_LOG_ERROR,
			  "QNX IRQ controller failed to register.\n");
		return -EINVAL;
	}

	ret = pthread_create(&irq_pthread, NULL, metal_qnx_irq_handling, NULL);
	if (ret != EOK) {
		metal_log(METAL_LOG_ERROR, "Failed to create IRQ thread: %d.\n",
			  ret);
		return -EAGAIN;
	}

	return 0;
}

/**
 * @brief irq handling shutdown
 */
void metal_qnx_irq_shutdown(void)
{
	int ret;

	metal_log(METAL_LOG_DEBUG, "%s\n", __func__);

	irq_handling_stop = true;

	for (int irq = 0; irq < MAX_IRQS; irq++) {
		if (irqs_ids[irq] != -1) {
			ret = InterruptDetach(irqs_ids[irq]);
			if (ret == -1) {
				metal_log(METAL_LOG_ERROR,
					  "Unable to detach connection: coid %d: %s\n",
					  self_coid, strerror(errno));
			}
		}
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
}
