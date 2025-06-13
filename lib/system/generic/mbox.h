/*
 * Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file	mbox.h
 * @brief	Generic mbox primitives for libmetal.
 *
 * Note that for now these are just stubs.
 */

#ifndef __METAL_MBOX_GENERIC__H__
#define __METAL_MBOX_GENERIC__H__

#include <metal/sys.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct { } metal_mbox_t;
typedef struct { } metal_mbox_init_data_t;
typedef void (*metal_mbox_callback_t)(void);

/** \defgroup mbox mbox Interfaces
 *  @{
 */

/**
 * @brief	Initialize a libmetal mbox.
 * @param[in]	mbox	mbox to initialize.
 * @param[in]	init_data	Data used to initialize mbox.
 *				In this case the driver instance is
 *				already set up at Zephr init.
 * @return	0 on success. Negative value on failure.
 */
extern int __metal_mbox_init(metal_mbox_t *mbox,
			     metal_mbox_init_data_t *init_data);

/**
 * @brief	Deinitialize a libmetal mbox.
 * @param[in]	mbox	mbox to deinitialize.
 * @return	0 on success. Negative value on failure.
 */
extern int __metal_mbox_deinit(metal_mbox_t *mbox);

/**
 * @brief	enable a mbox
 * @param[in]	mbox	mbox to enable
 * @return	0 on success. Negative value on failure.
 */
static inline int __metal_mbox_enable(metal_mbox_t *mbox);

/**
 * @brief	disable a mbox.
 * @param[in]	mbox	mbox to disable
 * @return	0 on success. Negative value on failure.
 */
static inline int __metal_mbox_disable(metal_mbox_t *mbox);

/**
 * @brief	Register callback routine for a mailbox.
 * @param[in]	mbox	mbox that will have registered callback.
 * @param[in]	callback	callback routine that will handle payloads
 * @param[in]	user_data	Application-specific data pointer which
 *				will be passed to the callback function
 *				when executed.
 * @return	0 on success. Negative value on failure.
 */
extern int __metal_mbox_register_cb(metal_mbox_t *mbox,
				    metal_mbox_callback_t cb,
				    void *user_data);
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
extern int __metal_mbox_send(metal_mbox_t *mbox, void *data, unsigned int len);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __METAL_MBOX_GENERIC__H__ */
