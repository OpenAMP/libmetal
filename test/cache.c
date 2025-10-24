/*
 * Copyright (c) 2025, CARV ICS FORTH.
 *
 * SPDX-License-Identifier:    BSD-3-Clause
 */

/*
 * @file		cache.c
 * @brief		Cross-platform tests for cache interface
 */

#include <metal/sys.h>
#include <metal/log.h>
#include <metal/cache.h>

int test_cache(void)
{
	struct metal_init_params metal_param = METAL_INIT_DEFAULTS;

	metal_init(&metal_param);
	metal_set_log_handler(metal_default_log_handler);
	metal_set_log_level(METAL_LOG_ERROR);

	metal_dbg("Flushing entire DCache\n");
	metal_cache_flush(NULL, 0);

	metal_dbg("Invalidating entire DCache\n");
	metal_cache_invalidate(NULL, 0);

	metal_finish();
	return 0;
}
