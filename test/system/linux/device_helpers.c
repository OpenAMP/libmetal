/*
 * Copyright (c) 2026, Advanced Micro Devices, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define METAL_INTERNAL

#include "metal-test.h"
#include <metal/device.h>
#include <metal/errno.h>
#include <metal/irq.h>
#include <metal/log.h>
#include <metal/sys.h>
#include <metal/system/linux/irq.h>

#include <string.h>
#include <sys/eventfd.h>
#include <unistd.h>

static const char test_bus_name[] = "linux-device-helper-test-bus";
static const char empty_bus_name[] = "linux-device-helper-empty-bus";
static const char test_dev_name[] = "linux-device-helper-test-device";

static struct metal_device test_device = {
	.name = test_dev_name,
};

static int test_dev_close_count;

static int test_dev_open(struct metal_bus *bus, const char *dev_name,
			 struct metal_device **device)
{
	if (!bus || !dev_name || !device)
		return -EINVAL;
	if (strcmp(dev_name, test_dev_name))
		return -ENODEV;

	test_device.bus = bus;
	*device = &test_device;
	return 0;
}

static void test_dev_close(struct metal_bus *bus, struct metal_device *device)
{
	if (bus && device == &test_device)
		test_dev_close_count++;
}

static int test_irq_handler(int irq, void *arg)
{
	(void)irq;
	(void)arg;

	return METAL_IRQ_HANDLED;
}

static int test_open_from_bus(void)
{
	struct metal_device *device = NULL;
	struct metal_bus test_bus = {
		.name = test_bus_name,
		.ops = {
			.dev_open = test_dev_open,
			.dev_close = test_dev_close,
		},
	};
	struct metal_bus empty_bus = {
		.name = empty_bus_name,
	};
	int ret;

	test_dev_close_count = 0;

	ret = metal_bus_register(&test_bus);
	if (ret)
		return ret;

	ret = metal_device_open_from_bus(test_bus_name, test_dev_name, &device);
	if (ret || device != &test_device) {
		metal_log(METAL_LOG_ERROR,
			  "metal_device_open_from_bus() returned %d, device %p\n",
			  ret, device);
		ret = ret ? ret : -EINVAL;
		goto out_unregister_test_bus;
	}

	metal_device_close(device);
	if (test_dev_close_count != 1) {
		metal_log(METAL_LOG_ERROR,
			  "metal_device_close() close count %d\n",
			  test_dev_close_count);
		ret = -EINVAL;
		goto out_unregister_test_bus;
	}

	device = NULL;
	ret = metal_device_open(test_bus_name, test_dev_name, &device);
	if (ret || device != &test_device) {
		metal_log(METAL_LOG_ERROR,
			  "metal_device_open() returned %d, device %p\n",
			  ret, device);
		ret = ret ? ret : -EINVAL;
		goto out_unregister_test_bus;
	}

	if (metal_device_open_from_bus(NULL, test_dev_name, &device) !=
	    -EINVAL ||
	    metal_device_open_from_bus(test_bus_name, NULL, &device) !=
	    -EINVAL ||
	    metal_device_open_from_bus(test_bus_name, test_dev_name, NULL) !=
	    -EINVAL ||
	    metal_device_open_from_bus("missing-bus", test_dev_name, &device) !=
	    -ENOENT ||
	    metal_device_open_from_bus(test_bus_name, "missing-dev", &device) !=
	    -ENODEV) {
		metal_log(METAL_LOG_ERROR,
			  "metal_device_open_from_bus() argument checks failed\n");
		ret = -EINVAL;
		goto out_unregister_test_bus;
	}

	ret = metal_bus_register(&empty_bus);
	if (ret)
		goto out_unregister_test_bus;

	ret = metal_device_open_from_bus(empty_bus_name, test_dev_name, &device);
	if (ret != -ENODEV) {
		metal_log(METAL_LOG_ERROR,
			  "metal_device_open_from_bus() without dev_open returned %d\n",
			  ret);
		ret = -EINVAL;
		goto out_unregister_empty_bus;
	}

	ret = 0;

out_unregister_empty_bus:
	metal_bus_unregister(&empty_bus);
out_unregister_test_bus:
	metal_bus_unregister(&test_bus);
	return ret;
}

static int test_uio_offset_validation(void)
{
	enum metal_log_level log_level = metal_get_log_level();
	unsigned long page_size = (unsigned long)getpagesize();
	int ret = 0;

	if (metal_linux_uio_validate_offset("test-uio", 0, 0))
		return -EINVAL;

	if (metal_linux_uio_validate_offset("test-uio", 0, page_size - 1))
		return -EINVAL;

	metal_set_log_level(METAL_LOG_CRITICAL);
	if (metal_linux_uio_validate_offset("test-uio", 0, page_size) !=
	    -EINVAL)
		ret = -EINVAL;
	if (metal_linux_uio_validate_offset(NULL, 0, page_size) != -EINVAL)
		ret = -EINVAL;
	metal_set_log_level(log_level);

	if (ret)
		metal_log(METAL_LOG_ERROR, "invalid UIO offset accepted\n");

	return ret;
}

static int test_irq_bookkeeping(void)
{
	struct metal_device dev = {
		.name = "linux-irq-test-device",
	};
	int fd;
	int ret;

	fd = eventfd(0, EFD_CLOEXEC);
	if (fd < 0)
		return -errno;

	ret = metal_irq_register(fd, test_irq_handler, &dev);
	if (ret)
		goto out_close;

	metal_linux_irq_register_dev(&dev, fd);
	if (metal_linux_irq_get_dev(fd) != &dev) {
		metal_log(METAL_LOG_ERROR,
			  "IRQ device registration not visible\n");
		ret = -EINVAL;
		goto out_unregister_irq;
	}

	metal_irq_enable(fd);
	if (!metal_linux_irq_is_enabled(fd)) {
		metal_log(METAL_LOG_ERROR, "IRQ enable state not visible\n");
		ret = -EINVAL;
		goto out_unregister_dev;
	}

	metal_linux_irq_unregister_dev(fd);
	if (metal_linux_irq_get_dev(fd) || metal_linux_irq_is_enabled(fd)) {
		metal_log(METAL_LOG_ERROR, "IRQ device state not cleared\n");
		ret = -EINVAL;
		goto out_unregister_irq;
	}

	ret = 0;
	goto out_close;

out_unregister_dev:
	metal_linux_irq_unregister_dev(fd);
out_unregister_irq:
	metal_irq_unregister(fd);
out_close:
	close(fd);
	return ret;
}

static int linux_device_helpers(void)
{
	int ret;

	ret = test_open_from_bus();
	if (ret)
		return ret;

	ret = test_uio_offset_validation();
	if (ret)
		return ret;

	return test_irq_bookkeeping();
}

METAL_ADD_TEST(linux_device_helpers);
