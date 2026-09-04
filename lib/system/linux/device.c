/*
 * Copyright (c) 2015, Xilinx Inc. and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file	linux/device.c
 * @brief	Linux libmetal device operations.
 */

#include <metal/device.h>
#include <metal/sys.h>
#include <metal/utilities.h>
#include <metal/irq.h>

#include <stdint.h>
#include <stdbool.h>
#include <dirent.h>

#include "irq.h"

#define MAX_DRIVERS	64
#define METAL_UIO_CLASS_PATH "/sys/class/uio"

struct linux_bus;
struct linux_device;

struct linux_driver {
	const char		*drv_name;
	const char		*mod_name;
	const char		*cls_name;
	struct sysfs_driver	*sdrv;
	int			(*dev_open)(struct linux_bus *lbus,
					    struct linux_device *ldev);
	void			(*dev_close)(struct linux_bus *lbus,
					     struct linux_device *ldev);
	void			(*dev_irq_ack)(struct linux_bus *lbus,
					     struct linux_device *ldev,
					     int irq);
	int			(*dev_dma_map)(struct linux_bus *lbus,
						struct linux_device *ldev,
						uint32_t dir,
						struct metal_sg *sg_in,
						int nents_in,
						struct metal_sg *sg_out);
	void			(*dev_dma_unmap)(struct linux_bus *lbus,
						struct linux_device *ldev,
						uint32_t dir,
						struct metal_sg *sg,
						int nents);
};

struct linux_bus {
	struct metal_bus	bus;
	const char		*bus_name;
	struct linux_driver	drivers[MAX_DRIVERS];
	struct sysfs_bus	*sbus;
};

struct linux_device {
	struct metal_device		device;
	char				dev_name[PATH_MAX];
	char				dev_path[PATH_MAX];
	/*
	 * UIO sysfs class directory, such as /sys/class/uio/uio0. UIO map
	 * attributes are read relative to this path.
	 */
	char				cls_path[PATH_MAX];
	char				uio_name[PATH_MAX];
	char				uio_dev_name[PATH_MAX];
	metal_phys_addr_t		region_phys[METAL_MAX_DEVICE_REGIONS];
	void				*region_map_raw[METAL_MAX_DEVICE_REGIONS];
	size_t				region_map_len[METAL_MAX_DEVICE_REGIONS];
	struct linux_driver		*ldrv;
	struct sysfs_device		*sdev;
	struct sysfs_attribute		*override;
	int				fd;
};

/**
 * @internal
 *
 * @brief UIO map attributes and derived libmetal region information.
 *
 * UIO sysfs reports a full mmap() extent plus a separate offset to the
 * usable resource. This structure keeps those inputs together while converting
 * them into the libmetal physical address, mmap length, and exported region
 * size.
 */
struct metal_uio_map_info {
	const char			*dev_name;
	metal_phys_addr_t		map_addr;
	unsigned long			map_size;
	unsigned long			offset;
	metal_phys_addr_t		*phys;
	size_t				*map_len;
	size_t				*region_size;
};

static struct linux_bus *to_linux_bus(struct metal_bus *bus)
{
	return metal_container_of(bus, struct linux_bus, bus);
}

static struct linux_device *to_linux_device(struct metal_device *device)
{
	return metal_container_of(device, struct linux_device, device);
}

/**
 * @internal
 *
 * @brief Check whether a Linux bus is the synthetic UIO bus.
 *
 * The synthetic UIO bus opens devices directly from /sys/class/uio instead of
 * through a sysfs bus directory and parent-bus drivers.
 *
 * @param[in] lbus Linux bus to check.
 * @return true if the bus is the synthetic UIO bus, otherwise false.
 */
static bool metal_linux_is_uio_bus(const struct linux_bus *lbus)
{
	return strcmp(lbus->bus_name, "uio") == 0;
}

/**
 * @internal
 *
 * @brief Read the first text line from a sysfs file.
 *
 * The trailing newline is stripped so callers can compare sysfs text values as
 * C strings.
 *
 * @param[in] path Path to the sysfs file.
 * @param[out] output Buffer that receives the first line.
 * @param[in] output_len Size of output in bytes.
 * @return 0 on success, or a negative error code on failure.
 */
static int metal_linux_read_first_line(const char *path, char *output,
				       size_t output_len)
{
	FILE *fp;
	char *newline;
	int result = 0;

	if (!path || !output || output_len < 2)
		return -EINVAL;

	fp = fopen(path, "r");
	if (!fp)
		return -errno;

	if (!fgets(output, output_len, fp)) {
		result = ferror(fp) ? -errno : -ENODATA;
		goto close_file;
	}

	newline = strchr(output, '\n');
	if (newline)
		*newline = '\0';

close_file:
	fclose(fp);

	return result;
}

static int metal_uio_read_map_attr(struct linux_device *ldev,
				   unsigned int index,
				   const char *name,
				   unsigned long *value)
{
	const char *cls = ldev->cls_path;
	struct sysfs_attribute *attr;
	char path[SYSFS_PATH_MAX];
	int result;

	result = snprintf(path, sizeof(path), "%s/maps/map%u/%s", cls, index, name);
	if (result >= (int)sizeof(path))
		return -EOVERFLOW;
	attr = sysfs_open_attribute(path);
	if (!attr || sysfs_read_attribute(attr) != 0) {
		sysfs_close_attribute(attr);
		return -errno;
	}

	*value = strtoul(attr->value, NULL, 0);

	sysfs_close_attribute(attr);
	return 0;
}

/**
 * @internal
 *
 * @brief Read a string-valued UIO sysfs attribute.
 *
 * The value is explicitly terminated so callers can compare it as a C string.
 *
 * @param[in] path Path to the UIO sysfs attribute.
 * @param[out] value Buffer that receives the attribute value.
 * @param[in] len Size of value in bytes.
 * @return 0 on success, or a negative error code on failure.
 */
static int metal_uio_read_str_attr(const char *path, char *value, size_t len)
{
	struct sysfs_attribute *attr;
	int result = 0;

	if (!value || !len)
		return -EINVAL;

	attr = sysfs_open_attribute(path);
	if (!attr || sysfs_read_attribute(attr) != 0) {
		result = -errno;
		goto close_attr;
	}

	strncpy(value, attr->value, len - 1);
	value[len - 1] = '\0';

close_attr:
	sysfs_close_attribute(attr);
	return result;
}

/**
 * @internal
 *
 * @brief Validate the sysfs map offset before applying it to the mmap() base.
 *
 * The Linux UIO ABI exposes one mmap slot per page-sized index, so the
 * per-map offset must remain within a single host page.
 *
 * The offset is applied inside one page returned by mmap(). Larger offsets
 * cannot be represented by adjusting the returned mapping.
 *
 * @param[in] dev_name Device name used for error reporting; may be NULL.
 * @param[in] offset Offset to validate, in bytes.
 * @return 0 on success, or -EINVAL if the offset exceeds the host page size.
 */
static int metal_linux_uio_validate_offset(const char *dev_name,
					   unsigned long offset)
{
	const unsigned long page_size = (unsigned long)getpagesize();

	if (offset >= page_size) {
		metal_log(METAL_LOG_ERROR,
			  "device %s has invalid UIO offset 0x%lx (page size 0x%lx)\n",
			  dev_name ? dev_name : "<unknown>", offset, page_size);
		return -EINVAL;
	}

	return 0;
}

/**
 * @internal
 *
 * @brief Translate UIO sysfs map attributes into libmetal map information.
 *
 * This fills in the values required by libmetal: the mmap() length used for
 * cleanup, the usable physical start address, and the usable I/O region size
 * after skipping the map offset.
 *
 * @param[in,out] info Pointer to the map information structure to populate.
 * @return 0 on success, or a negative error code on failure.
 */
static int metal_linux_uio_map_info(struct metal_uio_map_info *info)
{
	int result;

	if (!info || !info->phys || !info->map_len || !info->region_size)
		return -EINVAL;

	result = metal_linux_uio_validate_offset(info->dev_name, info->offset);
	if (result)
		return result;

	if (info->offset >= info->map_size) {
		metal_log(METAL_LOG_ERROR,
			  "device %s has invalid UIO size 0x%lx for offset 0x%lx\n",
			  info->dev_name ? info->dev_name : "<unknown>",
			  info->map_size, info->offset);
		return -EINVAL;
	}

	if (info->map_size > SIZE_MAX) {
		metal_log(METAL_LOG_ERROR,
			  "device %s UIO size 0x%lx overflows size_t\n",
			  info->dev_name ? info->dev_name : "<unknown>",
			  info->map_size);
		return -EOVERFLOW;
	}

	if (info->map_addr + info->offset < info->map_addr) {
		metal_log(METAL_LOG_ERROR,
			  "device %s UIO physical address overflow (addr=0x%lx offset=0x%lx)\n",
			  info->dev_name ? info->dev_name : "<unknown>",
			  (unsigned long)info->map_addr, info->offset);
		return -EOVERFLOW;
	}

	/*
	 * mmap() uses the full page-aligned map. libmetal clients see only the
	 * usable resource that starts at offset bytes into that mapping.
	 */
	*info->phys = info->map_addr + info->offset;
	*info->map_len = (size_t)info->map_size;
	*info->region_size = (size_t)(info->map_size - info->offset);

	return 0;
}

/**
 * @internal
 *
 * @brief Find a UIO class device by its exported name.
 *
 * This scans /sys/class/uio/uioX/name for the requested libmetal device name.
 * The UIO class name must uniquely identify the device because there is no
 * parent platform or PCI sysfs device to bind through first.
 *
 * @param[in] uio_name UIO name to search for.
 * @param[out] ldev Linux device to populate with the resolved paths.
 * @return 0 on success, or a negative error code on failure.
 */
static int metal_uio_find_device_by_name(const char *uio_name,
					 struct linux_device *ldev)
{
	DIR *dir;
	struct dirent *entry;
	char path[PATH_MAX];
	char value[PATH_MAX];
	bool found = false;
	int result = -ENODEV;

	if (!uio_name || !strlen(uio_name) || !ldev)
		return -EINVAL;

	dir = opendir(METAL_UIO_CLASS_PATH);
	if (!dir) {
		result = errno == ENOENT ? -ENODEV : -errno;
		return result;
	}

	/*
	 * Walk every UIO class device and compare its reported name against the
	 * requested libmetal name. Continue after a match so duplicate names can
	 * be detected instead of silently choosing a nondeterministic device.
	 */
	while ((entry = readdir(dir)) != NULL) {
		if (strncmp(entry->d_name, "uio", 3) != 0)
			continue;

		result = snprintf(path, sizeof(path), "%s/%s/name",
				  METAL_UIO_CLASS_PATH, entry->d_name);
		if (result < 0 || result >= (int)sizeof(path)) {
			result = -EOVERFLOW;
			goto out;
		}

		result = metal_linux_read_first_line(path, value,
						     sizeof(value));
		if (result)
			continue;

		if (strcmp(value, uio_name) != 0)
			continue;

		if (found) {
			/* Duplicate names cannot be opened deterministically. */
			result = -EEXIST;
			goto out;
		}
		found = true;

		result = snprintf(ldev->cls_path, sizeof(ldev->cls_path),
				  "%s/%s", METAL_UIO_CLASS_PATH,
				  entry->d_name);
		if (result < 0 || result >= (int)sizeof(ldev->cls_path)) {
			result = -EOVERFLOW;
			goto out;
		}
		/*
		 * Fill the same fields as the parent-bus UIO path so both
		 * open modes can share metal_uio_populate().
		 */
		result = snprintf(ldev->dev_path, sizeof(ldev->dev_path),
				  "/dev/%s", entry->d_name);
		if (result < 0 || result >= (int)sizeof(ldev->dev_path)) {
			result = -EOVERFLOW;
			goto out;
		}
		result = snprintf(ldev->uio_name, sizeof(ldev->uio_name),
				  "%s", value);
		if (result < 0 || result >= (int)sizeof(ldev->uio_name)) {
			result = -EOVERFLOW;
			goto out;
		}
		result = snprintf(ldev->uio_dev_name,
				  sizeof(ldev->uio_dev_name), "%s",
				  entry->d_name);
		if (result < 0 || result >= (int)sizeof(ldev->uio_dev_name)) {
			result = -EOVERFLOW;
			goto out;
		}
	}

	result = found ? 0 : -ENODEV;

out:
	closedir(dir);
	return result;
}

static int metal_uio_dev_bind(struct linux_device *ldev,
			      struct linux_driver *ldrv)
{
	struct sysfs_attribute *attr;
	int result;

	if (strcmp(ldev->sdev->driver_name, ldrv->drv_name) == 0)
		return 0;

	if (strcmp(ldev->sdev->driver_name, SYSFS_UNKNOWN) != 0) {
		metal_log(METAL_LOG_INFO, "device %s in use by driver %s\n",
			  ldev->dev_name, ldev->sdev->driver_name);
		return -EBUSY;
	}

	attr = sysfs_get_device_attr(ldev->sdev, "driver_override");
	if (!attr) {
		metal_log(METAL_LOG_ERROR, "device %s has no override\n",
			  ldev->dev_name);
		return -errno;
	}

	result = sysfs_write_attribute(attr, ldrv->drv_name,
				       strlen(ldrv->drv_name));
	if (result) {
		metal_log(METAL_LOG_ERROR, "failed to set override on %s\n",
			  ldev->dev_name);
		return -errno;
	}
	ldev->override = attr;

	attr = sysfs_get_driver_attr(ldrv->sdrv, "bind");
	if (!attr) {
		metal_log(METAL_LOG_ERROR, "driver %s has no bind\n", ldrv->drv_name);
		return -ENOTSUP;
	}

	result = sysfs_write_attribute(attr, ldev->dev_name,
				       strlen(ldev->dev_name));
	if (result) {
		metal_log(METAL_LOG_ERROR, "failed to bind %s to %s\n",
			  ldev->dev_name, ldrv->drv_name);
		return -errno;
	}

	metal_log(METAL_LOG_DEBUG, "bound device %s to driver %s\n",
		  ldev->dev_name, ldrv->drv_name);

	return 0;
}

/**
 * @internal
 *
 * @brief Populate common UIO device state from resolved UIO paths.
 *
 * Both parent-bus opens and class-name opens share the same mmap, IRQ
 * registration, DMA, and close-time cleanup rules.
 *
 * @param[in] lbus Linux bus used for diagnostics.
 * @param[in,out] ldev Linux device with cls_path and dev_path already set.
 * @return 0 on success, or a negative error code on failure.
 */
static int metal_uio_populate(struct linux_bus *lbus, struct linux_device *ldev)
{
	unsigned long offset = 0, size = 0;
	metal_phys_addr_t addr = 0, *phys;
	struct metal_io_region *io;
	struct metal_uio_map_info map_info;
	size_t map_len, region_size;
	int result, i = 0;
	unsigned int j;
	void *raw, *virt;
	int irq_info;

	do {
		if (!access(ldev->dev_path, F_OK))
			break;
		usleep(10);
		i++;
	} while (i < 1000);
	if (i >= 1000) {
		metal_log(METAL_LOG_ERROR, "failed to open file %s, timeout.\n",
			  ldev->dev_path);
		return -ENODEV;
	}
	result = metal_open(ldev->dev_path, 0);
	if (result < 0) {
		metal_log(METAL_LOG_ERROR, "failed to open device %s: %s\n",
			  ldev->dev_path, strerror(-result));
		return result;
	}
	ldev->fd = result;

	metal_log(METAL_LOG_DEBUG, "opened %s:%s as %s\n",
		  lbus->bus_name, ldev->dev_name, ldev->dev_path);

	for (i = 0; i < METAL_MAX_DEVICE_REGIONS; i++) {
		phys = &ldev->region_phys[ldev->device.num_regions];
		result = metal_uio_read_map_attr(ldev, i, "offset", &offset);
		/*
		 * A missing offset for the next map marks the end of the UIO
		 * map list. Other read errors are real open failures.
		 */
		if (result == -ENOENT)
			break;
		if (result)
			goto fail;
		result = (result ? result :
			 metal_uio_read_map_attr(ldev, i, "addr", &addr));
		result = (result ? result :
			 metal_uio_read_map_attr(ldev, i, "size", &size));
		if (result)
			goto fail;
		/*
		 * UIO sysfs reports addr/size/offset separately. Convert them
		 * before mmap() so the raw mapping and exposed region stay in
		 * sync for both normal access and close-time unmap.
		 */
		map_info.dev_name = ldev->dev_name;
		map_info.map_addr = addr;
		map_info.map_size = size;
		map_info.offset = offset;
		map_info.phys = phys;
		map_info.map_len = &map_len;
		map_info.region_size = &region_size;
		result = metal_linux_uio_map_info(&map_info);
		if (result)
			goto fail;
		result = metal_map(ldev->fd, i * getpagesize(), map_len, 0, 0,
				   &raw);
		if (result) {
			metal_log(METAL_LOG_ERROR,
				  "failed to mmap device %s map%u (len=0x%zx offset=0x%lx): %s\n",
				  ldev->dev_name, i, map_len,
				  (unsigned long)i * (unsigned long)getpagesize(),
				  strerror(-result));
			goto fail;
		}
		virt = (void *)((char *)raw + offset);
		/*
		 * Keep the raw mapping for munmap(); expose the adjusted
		 * address as the usable libmetal I/O region.
		 */
		io = &ldev->device.regions[ldev->device.num_regions];
		metal_io_init(io, virt, phys, region_size, -1, 0, NULL);
		ldev->region_map_raw[ldev->device.num_regions] = raw;
		ldev->region_map_len[ldev->device.num_regions] = map_len;
		ldev->device.num_regions++;
	}

	irq_info = 1;
	if (write(ldev->fd, &irq_info, sizeof(irq_info)) <= 0) {
		metal_log(METAL_LOG_INFO,
			  "No IRQ for device %s.\n", ldev->dev_name);
		ldev->device.irq_num =  0;
		ldev->device.irq_info = (void *)-1;
	} else {
		ldev->device.irq_num =  1;
		ldev->device.irq_info = (void *)(intptr_t)ldev->fd;
		metal_linux_irq_register_dev(&ldev->device, ldev->fd);
	}

	return 0;

fail:
	for (j = 0; j < ldev->device.num_regions; j++) {
		metal_unmap(ldev->region_map_raw[j],
			    ldev->region_map_len[j]);
		ldev->region_map_raw[j] = NULL;
		ldev->region_map_len[j] = 0;
	}
	ldev->device.num_regions = 0;
	ldev->device.irq_num = 0;
	ldev->device.irq_info = (void *)-1;
	if (ldev->fd >= 0) {
		close(ldev->fd);
		ldev->fd = -1;
	}

	return result;
}

static void metal_uio_dev_close(struct linux_bus *lbus,
				struct linux_device *ldev);

/**
 * @internal
 *
 * @brief Open a parent-bus device that exposes a UIO child.
 *
 * This path binds the parent device to a UIO driver before using the common
 * UIO populate logic.
 *
 * @param[in] lbus Linux bus containing the parent device.
 * @param[in,out] ldev Linux device to open.
 * @return 0 on success, or a negative error code on failure.
 */
static int metal_uio_dev_open(struct linux_bus *lbus, struct linux_device *ldev)
{
	char *instance, path[SYSFS_PATH_MAX];
	struct linux_driver *ldrv = ldev->ldrv;
	struct dlist *dlist;
	int result;

	ldev->fd = -1;
	ldev->device.irq_info = (void *)-1;

	ldev->sdev = sysfs_open_device(lbus->bus_name, ldev->dev_name);
	if (!ldev->sdev) {
		metal_log(METAL_LOG_ERROR, "device %s:%s not found\n",
			  lbus->bus_name, ldev->dev_name);
		return -ENODEV;
	}
	metal_log(METAL_LOG_DEBUG, "opened sysfs device %s:%s\n",
		  lbus->bus_name, ldev->dev_name);
	/*
	 * Error paths after this point clean up locally. The common open loop
	 * may call dev_close() again, so close must tolerate partial cleanup.
	 */

	/*
	 * Parent-bus opens still need the requested platform or PCI device
	 * bound to the selected UIO driver before a /dev/uioX node can exist.
	 */
	result = metal_uio_dev_bind(ldev, ldrv);
	if (result)
		goto fail;

	/*
	 * A bound parent device exposes one UIO child below its sysfs device
	 * directory. Use that child name to derive both sysfs and /dev paths.
	 */
	result = snprintf(path, sizeof(path), "%s/uio", ldev->sdev->path);
	if (result < 0 || result >= (int)sizeof(path)) {
		result = -EOVERFLOW;
		goto fail;
	}
	dlist = sysfs_open_directory_list(path);
	if (!dlist) {
		metal_log(METAL_LOG_ERROR, "failed to scan class path %s\n",
			  path);
		result = -errno;
		goto fail;
	}

	dlist_for_each_data(dlist, instance, char) {
		/*
		 * The first UIO child is the device node this parent-bus open
		 * will use for mmap, IRQ, and DMA operations.
		 */
		result = snprintf(ldev->cls_path, sizeof(ldev->cls_path),
				  "%s/%s", path, instance);
		if (result < 0 || result >= (int)sizeof(ldev->cls_path)) {
			result = -EOVERFLOW;
			goto close_list;
		}
		result = snprintf(ldev->dev_path, sizeof(ldev->dev_path),
				  "/dev/%s", instance);
		if (result < 0 || result >= (int)sizeof(ldev->dev_path)) {
			result = -EOVERFLOW;
			goto close_list;
		}
		result = snprintf(path, sizeof(path), "%s/name", ldev->cls_path);
		if (result < 0 || result >= (int)sizeof(path)) {
			result = -EOVERFLOW;
			goto close_list;
		}
		ldev->uio_name[0] = '\0';
		metal_uio_read_str_attr(path, ldev->uio_name,
					sizeof(ldev->uio_name));
		result = snprintf(ldev->uio_dev_name,
				  sizeof(ldev->uio_dev_name), "%s", instance);
		if (result < 0 || result >= (int)sizeof(ldev->uio_dev_name)) {
			result = -EOVERFLOW;
			goto close_list;
		}
		break;
	}

	sysfs_close_list(dlist);
	result = 0;

	/* Refuse to continue if the selected UIO class path disappeared. */
	if (sysfs_path_is_dir(ldev->cls_path) != 0) {
		metal_log(METAL_LOG_ERROR, "invalid device class path %s\n",
			  ldev->cls_path);
		result = -ENODEV;
		goto fail;
	}

	/*
	 * Once cls_path and dev_path are resolved, the rest of the open flow is
	 * shared with the synthetic UIO class-name path.
	 */
	result = metal_uio_populate(lbus, ldev);
	if (result)
		goto fail;

	return 0;

close_list:
	sysfs_close_list(dlist);
fail:
	metal_uio_dev_close(lbus, ldev);
	return result;
}

/**
 * @internal
 *
 * @brief Open a UIO class device through the synthetic UIO bus.
 *
 * The device name is matched against /sys/class/uio/uioX/name. No parent
 * sysfs device is available on this path.
 *
 * @param[in] lbus Synthetic UIO bus.
 * @param[in,out] ldev Linux device to open.
 * @return 0 on success, or a negative error code on failure.
 */
static int metal_uio_class_dev_open(struct linux_bus *lbus,
				    struct linux_device *ldev)
{
	int result;

	ldev->fd = -1;
	ldev->device.irq_info = (void *)-1;

	result = metal_uio_find_device_by_name(ldev->dev_name, ldev);
	if (result) {
		metal_log(METAL_LOG_ERROR, "UIO device %s not found\n",
			  ldev->dev_name);
		return result;
	}

	return metal_uio_populate(lbus, ldev);
}

static void metal_uio_dev_close(struct linux_bus *lbus,
				struct linux_device *ldev)
{
	(void)lbus;
	unsigned int i;

	if (ldev->fd >= 0) {
		/*
		 * Disable first so unregister only removes device bookkeeping;
		 * IRQ handler teardown remains in the generic IRQ path.
		 */
		metal_irq_disable(ldev->fd);
		metal_linux_irq_unregister_dev(ldev->fd);
	}
	for (i = 0; i < ldev->device.num_regions; i++) {
		metal_unmap(ldev->region_map_raw[i],
			    ldev->region_map_len[i]);
		ldev->region_map_raw[i] = NULL;
		ldev->region_map_len[i] = 0;
	}
	ldev->device.num_regions = 0;
	if (ldev->override) {
		sysfs_write_attribute(ldev->override, "", 1);
		ldev->override = NULL;
	}
	if (ldev->sdev) {
		sysfs_close_device(ldev->sdev);
		ldev->sdev = NULL;
	}
	if (ldev->fd >= 0) {
		close(ldev->fd);
		ldev->fd = -1;
	}
}

static void metal_uio_dev_irq_ack(struct linux_bus *lbus,
				 struct linux_device *ldev,
				 int irq)
{
	(void)lbus;
	(void)irq;
	int irq_info = 1;
	unsigned int val;
	int ret;

	ret = read(ldev->fd, (void *)&val, sizeof(val));
	if (ret < 0) {
		metal_log(METAL_LOG_ERROR, "%s, read uio irq fd %d failed: %d.\n",
						__func__, ldev->fd, ret);
		return;
	}
	ret = write(ldev->fd, &irq_info, sizeof(irq_info));
	if (ret < 0) {
		metal_log(METAL_LOG_ERROR, "%s, write uio irq fd %d failed: %d.\n",
						__func__, ldev->fd, errno);
	}
}

static int metal_uio_dev_dma_map(struct linux_bus *lbus,
				 struct linux_device *ldev,
				 uint32_t dir,
				 struct metal_sg *sg_in,
				 int nents_in,
				 struct metal_sg *sg_out)
{
	int i, j;
	void *vaddr_sg_lo, *vaddr_sg_hi, *vaddr_lo, *vaddr_hi;
	struct metal_io_region *io;

	(void)lbus;
	(void)dir;

	/* Check if the the input virt address is MMIO address */
	for (i = 0; i < nents_in; i++) {
		vaddr_sg_lo = sg_in[i].virt;
		vaddr_sg_hi = vaddr_sg_lo + sg_in[i].len;
		for (j = 0, io = ldev->device.regions;
		     j < (int)ldev->device.num_regions; j++, io++) {
			vaddr_lo = io->virt;
			vaddr_hi = vaddr_lo + io->size;
			if (vaddr_sg_lo >= vaddr_lo &&
			    vaddr_sg_hi <= vaddr_hi) {
				break;
			}
		}
		if (j == (int)ldev->device.num_regions) {
			metal_log(METAL_LOG_WARNING,
			  "%s,%s: input address isn't MMIO addr: 0x%x,%d.\n",
			__func__, ldev->dev_name, vaddr_sg_lo, sg_in[i].len);
			return -EINVAL;
		}
	}
	if (sg_out != sg_in)
		memcpy(sg_out, sg_in, nents_in*(sizeof(struct metal_sg)));
	return nents_in;
}

static void metal_uio_dev_dma_unmap(struct linux_bus *lbus,
				    struct linux_device *ldev,
				    uint32_t dir,
				    struct metal_sg *sg,
				    int nents)
{
	(void) lbus;
	(void) ldev;
	(void) dir;
	(void) sg;
	(void) nents;
}

static struct linux_bus linux_bus[] = {
	{
		.bus_name	= "uio",
		.drivers = {
			{
				.drv_name  = "uio",
				.mod_name  = "uio",
				.cls_name  = "uio",
				.dev_open  = metal_uio_class_dev_open,
				.dev_close = metal_uio_dev_close,
				.dev_irq_ack  = metal_uio_dev_irq_ack,
				.dev_dma_map = metal_uio_dev_dma_map,
				.dev_dma_unmap = metal_uio_dev_dma_unmap,
			},
			{ 0 /* sentinel */ }
		}
	},
	{
		.bus_name	= "platform",
		.drivers = {
			{
				.drv_name  = "uio_pdrv_genirq",
				.mod_name  = "uio_pdrv_genirq",
				.cls_name  = "uio",
				.dev_open  = metal_uio_dev_open,
				.dev_close = metal_uio_dev_close,
				.dev_irq_ack  = metal_uio_dev_irq_ack,
				.dev_dma_map = metal_uio_dev_dma_map,
				.dev_dma_unmap = metal_uio_dev_dma_unmap,
			},
			{
				.drv_name  = "uio_dmem_genirq",
				.mod_name  = "uio_dmem_genirq",
				.cls_name  = "uio",
				.dev_open  = metal_uio_dev_open,
				.dev_close = metal_uio_dev_close,
				.dev_irq_ack  = metal_uio_dev_irq_ack,
				.dev_dma_map = metal_uio_dev_dma_map,
				.dev_dma_unmap = metal_uio_dev_dma_unmap,
			},
			{ 0 /* sentinel */ }
		}
	},
	{
		.bus_name	= "pci",
		.drivers = {
			{
				.drv_name  = "vfio-pci",
				.mod_name  = "vfio-pci",
			},
			{
				.drv_name  = "uio_pci_generic",
				.mod_name  = "uio_pci_generic",
				.cls_name  = "uio",
				.dev_open  = metal_uio_dev_open,
				.dev_close = metal_uio_dev_close,
				.dev_irq_ack  = metal_uio_dev_irq_ack,
				.dev_dma_map = metal_uio_dev_dma_map,
				.dev_dma_unmap = metal_uio_dev_dma_unmap,
			},
			{ 0 /* sentinel */ }
		}
	},
	{
		/* sentinel */
		.bus_name = NULL,
	},
};

#define for_each_linux_bus(lbus)					\
	for ((lbus) = linux_bus; (lbus)->bus_name; (lbus)++)
#define for_each_linux_driver(lbus, ldrv)			\
	for ((ldrv) = lbus->drivers; (ldrv)->drv_name; (ldrv)++)


static int metal_linux_dev_open(struct metal_bus *bus,
				const char *dev_name,
				struct metal_device **device)
{
	struct linux_bus *lbus = to_linux_bus(bus);
	struct linux_device *ldev = NULL;
	struct linux_driver *ldrv;
	int error = -ENODEV;
	int ret;

	ldev = malloc(sizeof(*ldev));
	if (!ldev)
		return -ENOMEM;

	for_each_linux_driver(lbus, ldrv) {

		/* Check if we have a viable driver. */
		if (!ldrv->dev_open ||
		    (!metal_linux_is_uio_bus(lbus) && !ldrv->sdrv))
			continue;

		/* Reset device data. */
		memset(ldev, 0, sizeof(*ldev));
		error = snprintf(ldev->dev_name, sizeof(ldev->dev_name),
				 "%s", dev_name);
		if (error < 0 || error >= (int)sizeof(ldev->dev_name)) {
			error = -EOVERFLOW;
			goto out;
		}
		ldev->fd = -1;
		ldev->ldrv = ldrv;
		ldev->device.bus = bus;

		/* Try and open the device. */
		ret = ldrv->dev_open(lbus, ldev);
		if (ret) {
			/*
			 * Preserve the first useful errno while still allowing
			 * clean backend misses to try the same device.
			 */
			if (ldrv->dev_close)
				ldrv->dev_close(lbus, ldev);
			if (error == -ENODEV)
				error = ret;
			if (ret != -ENODEV)
				goto out;
			continue;
		}

		*device = &ldev->device;
		(*device)->name = ldev->dev_name;

		metal_list_add_tail(&bus->devices, &(*device)->node);
		return 0;
	}

out:
	free(ldev);

	return error;
}

static void metal_linux_dev_close(struct metal_bus *bus,
				  struct metal_device *device)
{
	struct linux_device *ldev = to_linux_device(device);
	struct linux_bus *lbus = to_linux_bus(bus);

	ldev->ldrv->dev_close(lbus, ldev);
	metal_list_del(&device->node);
	free(ldev);
}

/**
 * @internal
 *
 * @brief Close a Linux bus and any probed sysfs driver handles.
 *
 * The synthetic UIO bus has no sysfs bus handle, so the bus handle is closed
 * only when one was opened during probing.
 *
 * @param[in] bus Metal bus to close.
 */
static void metal_linux_bus_close(struct metal_bus *bus)
{
	struct linux_bus *lbus = to_linux_bus(bus);
	struct linux_driver *ldrv;

	for_each_linux_driver(lbus, ldrv) {
		if (ldrv->sdrv)
			sysfs_close_driver(ldrv->sdrv);
		ldrv->sdrv = NULL;
	}

	if (lbus->sbus)
		sysfs_close_bus(lbus->sbus);
	lbus->sbus = NULL;
}

static void metal_linux_dev_irq_ack(struct metal_bus *bus,
			     struct metal_device *device,
			     int irq)
{
	struct linux_device *ldev = to_linux_device(device);
	struct linux_bus *lbus = to_linux_bus(bus);

	return ldev->ldrv->dev_irq_ack(lbus, ldev, irq);
}

static int metal_linux_dev_dma_map(struct metal_bus *bus,
			     struct metal_device *device,
			     uint32_t dir,
			     struct metal_sg *sg_in,
			     int nents_in,
			     struct metal_sg *sg_out)
{
	struct linux_device *ldev = to_linux_device(device);
	struct linux_bus *lbus = to_linux_bus(bus);

	return ldev->ldrv->dev_dma_map(lbus, ldev, dir, sg_in,
				       nents_in, sg_out);
}

static void metal_linux_dev_dma_unmap(struct metal_bus *bus,
				      struct metal_device *device,
				      uint32_t dir,
				      struct metal_sg *sg,
				      int nents)
{
	struct linux_device *ldev = to_linux_device(device);
	struct linux_bus *lbus = to_linux_bus(bus);

	ldev->ldrv->dev_dma_unmap(lbus, ldev, dir, sg,
				       nents);
}

static const struct metal_bus_ops metal_linux_bus_ops = {
	.bus_close	= metal_linux_bus_close,
	.dev_open	= metal_linux_dev_open,
	.dev_close	= metal_linux_dev_close,
	.dev_irq_ack	= metal_linux_dev_irq_ack,
	.dev_dma_map	= metal_linux_dev_dma_map,
	.dev_dma_unmap	= metal_linux_dev_dma_unmap,
};

static int metal_linux_register_bus(struct linux_bus *lbus)
{
	lbus->bus.name = lbus->bus_name;
	lbus->bus.ops  = metal_linux_bus_ops;
	return metal_bus_register(&lbus->bus);
}

static int metal_linux_probe_driver(struct linux_bus *lbus,
				    struct linux_driver *ldrv)
{
	char command[256];
	int ret;

	ldrv->sdrv = sysfs_open_driver(lbus->bus_name, ldrv->drv_name);

	/* Try probing the module and then open the driver. */
	if (!ldrv->sdrv) {
		ret = snprintf(command, sizeof(command),
			       "modprobe %s > /dev/null 2>&1", ldrv->mod_name);
		if (ret >= (int)sizeof(command))
			return -EOVERFLOW;
		ret = system(command);
		if (ret < 0) {
			metal_log(METAL_LOG_WARNING,
				  "%s: executing system command '%s' failed.\n",
				  __func__, command);
		}
		ldrv->sdrv = sysfs_open_driver(lbus->bus_name, ldrv->drv_name);
	}

	/* If all else fails... */
	return ldrv->sdrv ? 0 : -ENODEV;
}

/**
 * @internal
 *
 * @brief Probe and register a Linux bus.
 *
 * The synthetic UIO bus is registered only when the /sys/class/uio class
 * exists and does not use normal sysfs bus or driver probing.
 *
 * @param[in,out] lbus Linux bus to probe.
 * @return 0 on success, or a negative error code on failure.
 */
static int metal_linux_probe_bus(struct linux_bus *lbus)
{
	struct linux_driver *ldrv;
	int ret, error = -ENODEV;

	if (metal_linux_is_uio_bus(lbus)) {
		if (sysfs_path_is_dir(METAL_UIO_CLASS_PATH) != 0)
			return -ENODEV;
		return metal_linux_register_bus(lbus);
	}

	lbus->sbus = sysfs_open_bus(lbus->bus_name);
	if (!lbus->sbus)
		return -ENODEV;

	for_each_linux_driver(lbus, ldrv) {
		ret = metal_linux_probe_driver(lbus, ldrv);
		/* Clear the error if any driver is available */
		if (!ret)
			error = ret;
	}

	if (error) {
		metal_linux_bus_close(&lbus->bus);
		return error;
	}

	error = metal_linux_register_bus(lbus);
	if (error)
		metal_linux_bus_close(&lbus->bus);

	return error;
}

int metal_linux_bus_init(void)
{
	struct linux_bus *lbus;
	int valid = 0;

	for_each_linux_bus(lbus)
		valid += metal_linux_probe_bus(lbus) ? 0 : 1;

	return valid ? 0 : -ENODEV;
}

void metal_linux_bus_finish(void)
{
	struct linux_bus *lbus;
	struct metal_bus *bus;

	for_each_linux_bus(lbus) {
		if (metal_bus_find(lbus->bus_name, &bus) == 0)
			metal_bus_unregister(bus);
	}
}

int metal_generic_dev_sys_open(struct metal_device *dev)
{
	(void)dev;
	return 0;
}

int metal_linux_get_device_property(struct metal_device *device,
				    const char *property_name,
				    void *output, int len)
{
	int fd = 0;
	int status = 0;
	const int flags = O_RDONLY;
	const int mode = S_IRUSR | S_IRGRP | S_IROTH;
	struct linux_device *ldev = to_linux_device(device);
	char path[PATH_MAX];

	snprintf(path, sizeof(path), "%s/of_node/%s",
			 ldev->sdev->path, property_name);
	fd = open(path, flags, mode);
	if (fd < 0)
		return -errno;
	if (read(fd, output, len) < 0) {
		status = -errno;
		close(fd);
		return status;
	}

	status = close(fd);
	return status < 0 ? -errno : 0;
}
