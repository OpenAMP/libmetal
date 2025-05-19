/*
 * Copyright (c) 2025, CARV ICS FORTH.
 *
 * SPDX-License-Identifier:    BSD-3-Clause
 */

/*
 * @file		test_all.h
 * @brief		Includes, defines and testing suite structs for test_all.c
 */

#include <stdio.h>
#include <pthread.h>
#include <metal/alloc.h>
#include <metal/assert.h>
#include <metal/atomic.h>
#include <metal/cache.h>
#include <metal/config.h>
#include <metal/cpu.h>
#include <metal/device.h>
#include <metal/dma.h>
#include <metal/irq.h>
#include <metal/log.h>
#include <metal/mutex.h>
#include <metal/shmem.h>
#include <metal/sleep.h>
#include <metal/spinlock.h>
#include <metal/sys.h>
#include <metal/time.h>
#include <metal/utilities.h>

#include "alloc.c"
#include "assert.c"
#include "cache.c"
#include "config.c"
#include "cpu.c"
#include "init.c"
#include "log.c"
#include "mutex.c"
#include "shmem.c"
#include "sleep.c"
#include "spinlock.c"
#include "time.c"
#include "utilities.c"

struct test_entry {
	char name[20];
	int (*test)(void);
	int ret;
};

struct test_entry tests[] = {
	{"alloc.c",     test_alloc, 0},
	{"assert.c",    test_assert, 0},
	{"cache.c",     test_cache, 0},
	{"config.c",    test_config, 0},
	{"cpu.c",       test_cpu, 0},
	{"init.c",      test_init, 0},
	{"log.c",       test_log, 0},
	{"mutex.c",     test_mutex, 0},
	{"shmem.c",     test_shmem, 0},
	{"sleep.c",     test_sleep, 0},
	{"spinlock.c",  test_spinlock, 0},
	{"time.c",      test_time, 0},
	{"utilities.c", test_utilities, 0}
};

#define NUM_TESTS (sizeof(tests) / sizeof(tests[0]))
