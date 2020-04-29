/*
 * Copyright (c) 2014, Mentor Graphics Corporation
 * All rights reserved.
 * Copyright (c) 2015 Xilinx, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef RSC_TABLE_H_
#define RSC_TABLE_H_

#include <openamp/remoteproc.h>

#if defined __cplusplus
extern "C" {
#endif

#define NO_RESOURCE_ENTRIES         4

#define MB			(1024 * 1024)
#define SROM_SIZE		(1 * MB)
#define SRAM_SIZE		(1 * MB)
#define LOG_BUFFER_SIZE		(1 * MB)

#define SROM_DA			((uint32_t)&_memmap_mem_srom_start)
#define SRAM_DA			((uint32_t)&_memmap_mem_sram_start)
#define LOG_BUFFER_DA		((uint32_t)&_log_start)

#define FW_RSC_ADDR_ANY		(-1)

extern uint32_t _memmap_mem_srom_start;
extern uint32_t _memmap_mem_sram_start;
extern uint32_t _memmap_mem_sram_end;
extern uint32_t _log_start;

/* Resource table for the given remote */
struct remote_resource_table {
	unsigned int version;
	unsigned int num;
	unsigned int reserved[2];
	unsigned int offset[NO_RESOURCE_ENTRIES];

	struct fw_rsc_carveout srom_hdr;
	struct fw_rsc_carveout sram_hdr;
	struct fw_rsc_carveout stack_hdr;
	struct fw_rsc_carveout logbuf_hdr;
	struct fw_rsc_trace trace_hdr;
} __packed __aligned(0x1000);

void *get_resource_table(int rsc_id, int *len);

#if defined __cplusplus
}
#endif

#endif /* RSC_TABLE_H_ */
