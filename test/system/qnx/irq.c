/*
 * Copyright (c) 2025, BlackBerry Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdlib.h>
#include <metal/errno.h>

#define METAL_INTERNAL

#include "metal-test.h"
#include <metal/irq.h>
#include <metal/log.h>
#include <metal/list.h>
#define BASE 1000

static int irq_handler(int irq, void *arg)
{
	(void)irq;
	(void)arg;

	return 0;
}

static int irq(void)
{
	int ret;
	char *err_msg = "";
	enum metal_log_level mll = metal_get_log_level();
	int i, tst_irq[2];

	for (i = 0; i < 2; i++) {
		tst_irq[i] = BASE + i;
		metal_log(METAL_LOG_DEBUG, "%s: %d interrupt associated with irq %d\n",
			__func__, i, tst_irq[i]);
	}

	ret = metal_irq_register(tst_irq[0], irq_handler, (void *)1);
	if (ret) {
		err_msg = "register irq 0 fail drv_id\n";
		goto out;
	}
	ret = metal_irq_register(tst_irq[1], irq_handler, (void *)1);
	if (ret) {
		err_msg = "register irq 1 fail drv_id\n";
		goto out;
	}

	metal_irq_unregister(tst_irq[0]);
	ret = metal_irq_register(tst_irq[0], irq_handler, (void *)1);
	if (ret) {
		err_msg = "register irq 0 fail drv_id\n";
		goto out;
	}
	metal_irq_unregister(tst_irq[0]);
	metal_irq_unregister(tst_irq[1]);

out:
	metal_set_log_level(mll);
	if ((err_msg[0] != '\0') && !ret)
		ret = -EINVAL;
	if (ret)
		metal_log(METAL_LOG_DEBUG, "%s", err_msg);
	return ret;
}

METAL_ADD_TEST(irq);
