/*
 * Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	mbox.c
 * @brief	Zephyr mbox primitives for libmetal.
 *
 */

#include <metal/mbox.h>

int __metal_mbox_send(metal_mbox_t *mbox, void *data, unsigned int len)
{
	metal_assert(mbox && mbox->m);
	struct mbox_msg msg;
	const int max_transfer_size_bytes = mbox_mtu_get_dt(mbox->m);

	/* If data is NOT NULL, then pack message. */
	if (data) {
		/* Validate payload and mailbox channel. */
		if (max_transfer_size_bytes < 0) {
			metal_err("%s: mbox_mtu_get_dt failed.\n", __func__);
			return -EINVAL;
		}
		if (len >= max_transfer_size_bytes) {
			metal_err("%s: len > max_transfer_size_bytes. %x > %x\n",
				  __func__, len, max_transfer_size_bytes);
			return -EMSGSIZE;
		}

		msg.data = (int *)data;
		msg.size = len;

		return mbox_send_dt(mbox->m, &msg);
	}

	(void)len;
	return mbox_send_dt(mbox->m, NULL);
}
