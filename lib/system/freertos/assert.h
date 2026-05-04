/*
 * Copyright (c) 2018, Xilinx Inc. and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file	freertos/assert.h
 * @brief	FreeRTOS assertion support.
 */

#ifndef __METAL_ASSERT__H__
#error "Do not include this file directly, include <metal/assert.h> instead"
#endif

#ifndef __METAL_FREERTOS_ASSERT__H__
#define __METAL_FREERTOS_ASSERT__H__

#include <assert.h>

/**
 * @brief Assertion macro for FreeRTOS applications.
 * @param cond Condition to evaluate.
 */
#define metal_sys_assert(cond) assert(cond)

#endif /* __METAL_FREERTOS_ASSERT__H__ */

