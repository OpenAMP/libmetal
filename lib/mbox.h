/*
 * Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	mbox.h
 * @brief	mbox primitives for libmetal.
 *
 * This file provides interface for mailbox operations.
 * This will involve capabilities to register a callback to process a
 * payload, send message and to enable/disable a mailbox.
 */

#ifndef __METAL_MBOX__H__
#define __METAL_MBOX__H__

#include <metal/system/@PROJECT_SYSTEM@/mbox.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup mbox mbox Interfaces
 *  @{
 */

/**
 * @brief	Initialize a libmetal mbox.
 * @param[in]	mbox	mbox to initialize.
 * @param[in]	init_data	data used to initialize mbox
 * @return	0 on success. Negative value on failure.
 */
static inline int metal_mbox_init(metal_mbox_t *mbox,
				  metal_mbox_init_data_t *init_data)
{
	return __metal_mbox_init(mbox, init_data);
}

/**
 * @brief	Deinitialize a libmetal mbox.
 * @param[in]	mbox	mbox to deinitialize.
 * @return	0 on success. Negative value on failure.
 */
static inline int metal_mbox_deinit(metal_mbox_t *mbox)
{
	return __metal_mbox_deinit(mbox);
}

/**
 * @brief	enable a mbox
 * @param[in]	mbox	mbox to enable
 * @return	0 on success. Negative value on failure.
 */
static inline int metal_mbox_enable(metal_mbox_t *mbox)
{
	return __metal_mbox_enable(mbox);
}

/**
 * @brief	disable a mbox.
 * @param[in]	mbox	mbox to disable
 * @return	0 on success. Negative value on failure.
 */
static inline int metal_mbox_disable(metal_mbox_t *mbox)
{
	return __metal_mbox_disable(mbox);
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
static inline int metal_mbox_register_cb(metal_mbox_t *mbox,
					 metal_mbox_callback_t cb,
					 void *user_data)
{
	return __metal_mbox_register_cb(mbox, cb, user_data);
}

/**
 * @brief	Try to send a message over the MBOX device.
 *
 * @param[in]	mbox	mbox that will send message
 * @param[in]	data	payload to send via mbox
 * @param[in]	len	length of payload
 *
 * @return	0 on success. Negative value on failure.
 */
static inline int metal_mbox_send(metal_mbox_t *mbox, void *data, unsigned int len)
{
	return __metal_mbox_send(mbox, data, len);
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __METAL_MBOX__H__ */
