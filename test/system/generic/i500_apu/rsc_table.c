/*
 * Copyright (c) 2019 - 2020, BayLibre SAS
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "rsc_table.h"

#define IOMMU_READ		(1 << 0)
#define IOMMU_WRITE		(1 << 1)
#define IOMMU_CACHE		(1 << 2)
#define IOMMU_NOEXEC		(1 << 3)
#define IOMMU_MMIO		(1 << 4)

struct remote_resource_table s_table
	__attribute__((section(".resource_table"))) = {
	.version = 1,
	.num = NO_RESOURCE_ENTRIES,
	.reserved = {0, 0},
	.offset = {
		offsetof(struct remote_resource_table, srom_hdr),
		offsetof(struct remote_resource_table, sram_hdr),
		offsetof(struct remote_resource_table, logbuf_hdr),
		offsetof(struct remote_resource_table, trace_hdr),
	},
	.srom_hdr = {
		.type = RSC_CARVEOUT,
		.da = SROM_DA,
		.pa = FW_RSC_ADDR_ANY,
		.len = SROM_SIZE,
		.flags = IOMMU_READ|IOMMU_WRITE,
		.name = "System ROM",
		.reserved = 0,
	},
	.sram_hdr = {
		.type = RSC_CARVEOUT,
		.da = SRAM_DA,
		.pa = FW_RSC_ADDR_ANY,
		.len = SRAM_SIZE,
		.flags = IOMMU_READ|IOMMU_WRITE|IOMMU_NOEXEC,
		.name = "System RAM",
		.reserved = 0,
	},
	.logbuf_hdr = {
		.type = RSC_CARVEOUT,
		.da = LOG_BUFFER_DA,
		.pa = FW_RSC_ADDR_ANY,
		.len = LOG_BUFFER_SIZE,
		.flags = IOMMU_READ|IOMMU_WRITE|IOMMU_NOEXEC,
		.name = "Log buffer",
		.reserved = 0,
	},
	.trace_hdr = {
		.type = RSC_TRACE,
		.da = LOG_BUFFER_DA, /* Log Buffer address */
		.len = LOG_BUFFER_SIZE,
		.name = "Trace",
		.reserved = 0,
	},
};

void *get_resource_table(int rsc_id, int *len)
{
	(void) rsc_id;
	*len = sizeof(s_table);
	return &s_table;
}
