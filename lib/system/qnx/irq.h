/*
 * Copyright (c) 2025, BlackBerry Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	qnx/irq.h
 * @brief	QNX libmetal irq definitions.
 */

#ifndef __METAL_IRQ__H__
#error "Include metal/irq.h instead of metal/qnx/irq.h"
#endif

#ifndef __METAL_QNX_IRQ__H__
#ifdef METAL_INTERNAL

#include <metal/device.h>

#endif /* METAL_INTERNAL */
#define __METAL_QNX_IRQ__H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef METAL_INTERNAL
/**
 * @brief irq initialization
 */
int metal_qnx_irq_init(void);

/**
 * @brief irq shutdown
 */
void metal_qnx_irq_shutdown(void);

#endif /* METAL_INTERNAL */

#ifdef __cplusplus
}
#endif

#endif /* __METAL_QNX_IRQ__H__ */
