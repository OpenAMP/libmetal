/*
 * Copyright (c) 2015, Xilinx Inc. and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	atomic.h
 * @brief	Atomic primitives for libmetal.
 */

#ifndef __METAL_ATOMIC__H__
#define __METAL_ATOMIC__H__

#include <metal/config.h>

#if defined(HAVE_STDATOMIC_H) && !defined(__STDC_NO_ATOMICS__) && \
	!defined(__cplusplus)
# include <stdint.h>
# include <stdatomic.h>
#elif defined(__cplusplus)
# include <atomic>
# define _Atomic(T) std::atomic<T>
using std::memory_order;
using std::atomic_flag;
using std::atomic_bool;
using std::atomic_uchar;
using std::atomic_ushort;
using std::atomic_uint;
using std::atomic_ulong;
using std::atomic_ullong;
#elif defined(__GNUC__)
# include <metal/compiler/gcc/atomic.h>
#else
# include <metal/processor/@PROJECT_PROCESSOR@/atomic.h>
#endif

#endif /* __METAL_ATOMIC__H__ */
