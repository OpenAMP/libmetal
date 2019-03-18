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

#include <errno.h>
#include <fcntl.h>
#include <linux/futex.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/eventfd.h>
#include <sys/types.h>
#include <unistd.h>
#include <metal/atomic.h>
#include <metal/irq.h>
#include <metal/log.h>
#include <metal/sys.h>
#include "common.h"

#define RPROC_SYSFS "/sys/class/remoteproc/remoteproc0"
#define RPROC_SYSFS_KICK RPROC_SYSFS"/kick"
#define RPROC_SYSFS_REMOTE_KICK RPROC_SYSFS"/remote_kick"

struct ipi_channel {
	char *kickf; /**< IPI kick sysfs path */
	char *rkickf; /**< IPI remote kick sysfs path */
	int rkick_fd; /**< IPI remote kick file descriptor */
	atomic_int ipi_enabled; /**< flag to indicate if IPI is enabled */
	atomic_int ipi_handling_wakeup; /**< IPI handling wakeup */
	metal_irq_handler ipi_kick_cb; /**< IPI kick callback */
	void *ipi_kick_priv; /**< IPI kick callback private data */
};

static struct ipi_channel ipi_chnl;
static pthread_t ipi_pthread;
static int ipi_handling_stop_fd;

/**
 * @brief ipi_kick_handling() - IPI kick handling
 *
 * It will listening to the remote kick sysfs file.
 * It will call the registered handler if there is a kick from the
 * remote.
 *
 * @param[in] args - argument of pthread.
 * @return NULL.
 *
 */
static void *_ipi_kick_handling (void *args)
{
	int rkick_fd;
	sigset_t set;

	(void)args;
	/* unblock all signals */
	sigfillset(&set);
	pthread_sigmask(SIG_UNBLOCK, &set, NULL);
	rkick_fd = ipi_chnl.rkick_fd;
	while(1) {
		char val[2] = {0, 0};
		int ret;

		lseek(rkick_fd, 0, SEEK_SET);
		ret = read(rkick_fd, &val, sizeof(val));
		if (ret < 0) {
			LPERROR("failed to read %s.\n", ipi_chnl.rkickf);
			return NULL;
		}
		if (val[0] == '1') {
			if (atomic_load(&ipi_chnl.ipi_enabled) == 0) {
				int v;

				v = atomic_load(&ipi_chnl.ipi_handling_wakeup);
				syscall(SYS_futex,
					&ipi_chnl.ipi_handling_wakeup,
					FUTEX_WAIT, v, NULL, NULL, 0);
			}
			if (ipi_chnl.ipi_kick_cb) {
				ipi_chnl.ipi_kick_cb(rkick_fd,
						     ipi_chnl.ipi_kick_priv);
			}
			lseek(rkick_fd, 0, SEEK_SET);
			ret = write(rkick_fd, &val, sizeof(val));
			if (ret < 0) {
				LPERROR("failed to write %s.\n",
					ipi_chnl.rkickf);
				return NULL;
			}
		} else {
			struct pollfd fds[2];

			fds[0].fd = rkick_fd;
			fds[0].events = POLLPRI;
			fds[1].fd = ipi_handling_stop_fd;
			fds[1].events = POLLIN;
			ret = poll(fds, 2, -1);
			if (ret == 0) {
				LPERROR("polling %s times out.\n",
					ipi_chnl.rkickf);
				return NULL;
			} else if (fds[1].revents & POLLIN) {
				return NULL;

			} else if (ret < 0) {
				LPERROR("polling %s failed, %s.\n",
					ipi_chnl.rkickf, strerror(ret));
				return NULL;
			}
		}
	}
}

static void _enable_ipi_intr(struct ipi_channel *chnl)
{
	int exp = 0;

	atomic_compare_exchange_strong(&chnl->ipi_enabled, &exp, 1);
	if (exp == 0) {
		atomic_fetch_add(&chnl->ipi_handling_wakeup, 1);
		syscall(SYS_futex, &chnl->ipi_handling_wakeup, FUTEX_WAKE,
			1, NULL, NULL, 0);
	}
}

static void _disable_ipi_intr(struct ipi_channel *chnl)
{
	atomic_store(&chnl->ipi_enabled, 0);
}

void ipi_kick_register_handler(metal_irq_handler hd, void *priv)
{
	ipi_chnl.ipi_kick_cb = hd;
	ipi_chnl.ipi_kick_priv = priv;
}

int init_ipi(void)
{
	int ret;
	int val = 0;

	atomic_init(&ipi_chnl.ipi_enabled, 0);
	atomic_init(&ipi_chnl.ipi_handling_wakeup, 0);
	ipi_chnl.kickf = RPROC_SYSFS_KICK;
	ipi_chnl.rkickf = RPROC_SYSFS_REMOTE_KICK;

	ret = open(ipi_chnl.rkickf, O_RDWR);
	if (ret < 0) {
		LPERROR("failed to open %s, %s.\n",
			ipi_chnl.rkickf, strerror(ret));
		return ret;
	}
	ipi_chnl.rkick_fd = ret;
	/** Write to the remote kick fd to clear pending IPI interrupt */
	ret = write(ipi_chnl.rkick_fd, &val, sizeof(val));
	if (ret < 0) {
		LPERROR("failed to write to %s, %s.\n",
			ipi_chnl.rkickf, strerror(ret));
		close(ipi_chnl.rkick_fd);
		return ret;
	}

	ipi_handling_stop_fd = eventfd(0,0);
	if (ipi_handling_stop_fd < 0) {
		LPERROR("failed to create ipi handling stop fd.\n");
		close(ipi_chnl.rkick_fd);
		return ipi_handling_stop_fd;
	}

	ret = pthread_create(&ipi_pthread, NULL, _ipi_kick_handling, NULL);
	if (ret < 0) {
		LPERROR("failed to create IPI handling thread.\n");
		close(ipi_chnl.rkick_fd);
		close(ipi_handling_stop_fd);
		return ret;
	}
	return 0;
}

void deinit_ipi(void)
{
	int ret;
	uint64_t val = 1;

	ret = write(ipi_handling_stop_fd, &val, sizeof(val));
	if (ret < 0) {
		LPERROR("failed to notify deinit ipi, %s.\n", strerror(ret));
	}
	atomic_fetch_add(&ipi_chnl.ipi_handling_wakeup, 1);
	syscall(SYS_futex, &ipi_chnl.ipi_handling_wakeup, FUTEX_WAKE,
		1, NULL, NULL, 0);
	ret = pthread_join(ipi_pthread, NULL);
	if (ret) {
		LPERROR("failed to join IPI thread: %d.\n", ret);
	}
	close(ipi_handling_stop_fd);
	close(ipi_chnl.rkick_fd);
}

void kick_ipi(void *msg)
{
	int fd, ret;
	int buf = -1;

	(void)msg;
	fd = open(ipi_chnl.kickf, O_WRONLY);
	if (fd < 0) {
		LPERROR("failed to open %s.\n", ipi_chnl.kickf);
		return;
	}

	ret = write(fd, &buf, sizeof(buf));
	if (ret <= 0)
		LPERROR("failed to write to %s, %s.\n",
			ipi_chnl.kickf, strerror(ret));
	close(fd);
}

void disable_ipi_kick(void)
{
	_disable_ipi_intr(&ipi_chnl);
}

void enable_ipi_kick(void)
{
	_enable_ipi_intr(&ipi_chnl);
}
