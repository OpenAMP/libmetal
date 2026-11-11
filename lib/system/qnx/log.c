/*
 * Copyright (c) 2026, BlackBerry Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file	qnx/log.c
 * @brief	QNX libmetal log handler.
 */

#include <stdarg.h>
#include <syslog.h>
#include <metal/log.h>

void metal_qnx_log_handler(enum metal_log_level level, const char *format, ...)
{
	int syslog_level;
	va_list args;

	switch (level) {
	case METAL_LOG_EMERGENCY:
		syslog_level = LOG_EMERG;
		break;
	case METAL_LOG_ALERT:
		syslog_level = LOG_ALERT;
		break;
	case METAL_LOG_CRITICAL:
		syslog_level = LOG_CRIT;
		break;
	case METAL_LOG_ERROR:
		syslog_level = LOG_ERR;
		break;
	case METAL_LOG_WARNING:
		syslog_level = LOG_WARNING;
		break;
	case METAL_LOG_NOTICE:
		syslog_level = LOG_NOTICE;
		break;
	case METAL_LOG_INFO:
		syslog_level = LOG_INFO;
		break;
	case METAL_LOG_DEBUG:
	default:
		syslog_level = LOG_DEBUG;
		break;
	}

	va_start(args, format);
	vsyslog(syslog_level, format, args);
	va_end(args);
}
