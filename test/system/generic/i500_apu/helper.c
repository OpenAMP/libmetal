/*
 * Copyright (c) 2020, BayLibre
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <metal/irq.h>
#include <metal/sys.h>

#include "rsc_table.h"

int init_system(void)
{
	return i500_irq_init();
}

void cleanup_system(void)
{
}
