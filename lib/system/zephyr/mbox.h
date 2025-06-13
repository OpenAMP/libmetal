/*
 * Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	mbox.h
 * @brief	Zephyr mbox primitives for libmetal.
 *
 */

#ifndef __METAL_MBOX_ZEPHYR__H__
#define __METAL_MBOX_ZEPHYR__H__

#include <metal/assert.h>
#include <metal/sys.h>
#include <zephyr/drivers/mbox.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief	mailbox structure for libmetal. This will contain a
 *		single direction channel.
 *
 * @param[in]	m internal zephyr mailbox structure.
 */
typedef struct {
	const struct mbox_dt_spec *m;
} metal_mbox_t;

/**
 * @brief	Callback to handle mbox payloads
 * @return	irq handled status
 */
typedef mbox_callback_t metal_mbox_callback_t;

/**
 * @brief Structure to contain data that will describe the mailbox
 * based on Zephyr mailbox driver.
 *
 * Because the node identified used to get mailbox cannot be set in
 * typecast instead just pass the mailbox so that libmetal can
 * manage it.
 */
typedef const struct mbox_dt_spec metal_mbox_init_data_t;

/** \defgroup mbox mbox Interfaces
 *  @{
 */

/**
 * @brief	Initialize a libmetal mbox.
 * @param[in]	mbox	mbox to initialize.
 * @param[in]	init_data	Data used to initialize mbox.
 *				In this case the driver instance is
 *				already set up at Zephyr init.
 *
 *				For Zephyr OS, instead just set the
 *				libmetal structure to point to the
 *				driver instance. This is because
 *				Zephyr OS does not allow type casting
 *				for the node identifier used to find the
 *				mbox.
 *
 * @return	0 on success. Negative value on failure.
 */
static inline int __metal_mbox_init(metal_mbox_t *mbox,
				    metal_mbox_init_data_t *init_data)
{
	metal_assert(mbox);
	metal_assert(init_data);
	mbox->m = (const struct mbox_dt_spec *)init_data;
	return 0;
}

/**
 * @brief	Deinitialize a libmetal mbox.
 * @param[in]	mbox	mbox to deinitialize.
 * @return	0 on success. Negative value on failure.
 */
static inline int __metal_mbox_deinit(metal_mbox_t *mbox)
{
	metal_assert(mbox && mbox->m);
	/* disable before wiping libmetal pointer. */
	mbox_set_enabled_dt(mbox->m, 0);
	mbox->m = NULL;
	return 0;
}

/**
 * @brief	enable a mbox
 * @param[in]	mbox	mbox to enable
 * @return	0 on success. Negative value on failure.
 */
static inline int __metal_mbox_enable(metal_mbox_t *mbox)
{
	metal_assert(mbox && mbox->m);
	return mbox_set_enabled_dt(mbox->m, 1);
}

/**
 * @brief	disable a mbox.
 * @param[in]	mbox	mbox to disable
 * @return	0 on success. Negative value on failure.
 */
static inline int __metal_mbox_disable(metal_mbox_t *mbox)
{
	metal_assert(mbox && mbox->m);
	return mbox_set_enabled_dt(mbox->m, 0);
}

/**
 * @brief	Register callback routine for a mailbox.
 * @param[in]	mbox	mbox that will have registered callback.
 * @param[in]	callback	callback routine that will handle payloads
 * @param[in]	user_data	Application-specific data pointer which
 *				will be passed to the callback function
 *				when executed.
 * @return	0 on success. Negative value on failure.
 */
static inline int __metal_mbox_register_cb(metal_mbox_t *mbox,
					   metal_mbox_callback_t cb,
					   void *user_data)
{
	metal_assert(mbox && mbox->m);
	return mbox_register_callback_dt(mbox->m, cb, user_data);

}

/**
 * @brief	Try to send a message over the MBOX device.
 *
 * Send a message over a struct mbox_channel. The msg parameter must
 * be NULL when the driver is used for signalling. If the msg parameter
 * is not NULL, this data is expected to be delivered on the receiving
 * side using the data parameter of the receiving callback.
 *
 * @param[in]	mbox	mbox that will send message
 * @param[in]	data	payload to send via mbox
 * @param[in]	len	length of payload
 *
 * @return	0 on success. Negative value on failure.
 *
 * Note that if channel is RX this will error out and not send message.
 */
int __metal_mbox_send(metal_mbox_t *mbox, void *data, unsigned int len);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __METAL_MBOX_ZEPHYR__H__ */
