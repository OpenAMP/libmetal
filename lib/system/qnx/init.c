/*
 * Copyright (c) 2025, BlackBerry Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	qnx/init.c
 * @brief	QNX libmetal initialization.
 */

#include <sys/types.h>
#include <metal/sys.h>
#include <metal/device.h>
#include <metal/irq.h>
#include <metal/cache.h>
#include <metal/utilities.h>

#include "irq.h"

struct metal_state _metal;
struct cache_ctrl __qnx_cache_control;
static int GENERIC_BUS_REGISTER;

static void metal_init_page_sizes(void)
{
	_metal.page_size = sysconf(_SC_PAGESIZE);
	_metal.page_shift = metal_log2(_metal.page_size);
}

int metal_sys_init(const struct metal_init_params *params)
{
	int ret;

	/* Initialize page size */
	metal_init_page_sizes();

	/* Initialize IRQ */
	ret = metal_qnx_irq_init();
	if (ret != 0) {
		metal_log(METAL_LOG_ERROR, "irq init failed - %s\n",
			  strerror(-ret));
		return ret;
	}

	/* Initialize cache */
	memset(&__qnx_cache_control, 0, sizeof(__qnx_cache_control));
	__qnx_cache_control.fd = NOFD;

	ret = cache_init(0, &__qnx_cache_control, NULL);
	if (ret == -1) {
		metal_log(METAL_LOG_ERROR, "cache init failed - %s\n",
			  strerror(errno));
		return -errno;
	}

	/* Initialize generic bus */
	ret = metal_bus_register(&metal_generic_bus);
	if (ret != 0) {
		metal_log(METAL_LOG_DEBUG, "generic bus init failed - %s\n",
			  strerror(-ret));
	} else {
		GENERIC_BUS_REGISTER = 1;
	}

	metal_unused(params);

	return 0;
}

void metal_sys_finish(void)
{
	cache_fini(&__qnx_cache_control);
	metal_qnx_irq_shutdown();
	if (GENERIC_BUS_REGISTER) {
		metal_bus_unregister(&metal_generic_bus);
	}
}

