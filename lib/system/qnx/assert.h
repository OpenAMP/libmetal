/*
 * Copyright (c) 2025, BlackBerry Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	qnx/assert.h
 * @brief	QNX libmetal assertion support.
 */

#ifndef __METAL_ASSERT__H__
#error "Include metal/assert.h instead of metal/qnx/assert.h"
#endif

#ifndef __METAL_QNX_ASSERT__H__
#define __METAL_QNX_ASSERT__H__

#include <assert.h>

/**
 * @brief Assertion macro for QNX-based applications.
 * @param cond Condition to evaluate.
 */
#define metal_sys_assert(cond) assert(cond)

#endif /* __METAL_QNX_ASSERT__H__ */
