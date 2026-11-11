/*
 * Copyright (c) 2026, BlackBerry Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file	qnx/log.h
 * @brief	QNX libmetal log handler definition.
 */

#ifndef __METAL_METAL_LOG__H__
#error "Include metal/log.h instead of metal/qnx/log.h"
#endif

#ifndef __METAL_QNX_LOG__H__
#define __METAL_QNX_LOG__H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief QNX libmetal log handler.
 *
 * @param[in] level log level
 * @param[in] format format string
 */
void metal_qnx_log_handler(enum metal_log_level level, const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif /* __METAL_QNX_LOG__H__ */
