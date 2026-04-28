/*
 * Copyright (c) 2018, Pinecone Inc. and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file	nuttx/irq.h
 * @brief	NuttX libmetal irq definitions.
 */

#ifndef __METAL_IRQ__H__
#error "Do not include this file directly, include <metal/irq.h> instead"
#endif

#ifndef __METAL_NUTTX_IRQ__H__
#define __METAL_NUTTX_IRQ__H__

#ifdef __cplusplus
extern "C" {
#endif

int metal_cntr_irq_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __METAL_NUTTX_IRQ__H__ */
