/*
 * Copyright (c) 2022-2023 Advanced Micro Devices, Inc. All Rights Reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	generic/xlnx/sys_devicetree.h
 * @brief	Xilinx-AMD Specific symbols for System Device Tree Flow
 */

#ifndef __METAL_GENERIC_SYS_XLNX_SYS_DEVICETREE_H__
#define __METAL_GENERIC_SYS_XLNX_SYS_DEVICETREE_H__

/* The Xilinx-AMD System Device Tree (SDT) Flow BSP provides this file. */
#include "bspconfig.h"

#ifndef XPAR_SCUGIC_0_DEVICE_ID
#define XPAR_SCUGIC_0_DEVICE_ID XPAR_SCUGIC_SINGLE_DEVICE_ID
#endif

#ifndef XPAR_SCUGIC_0_DIST_BASEADDR
#define XPAR_SCUGIC_0_DIST_BASEADDR XPAR_SCUGIC_DIST_BASEADDR
#endif
#endif /* __METAL_GENERIC_SYS_XLNX_SYS_DEVICETREE_H__ */
