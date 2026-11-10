/*
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <limits.h>
#include <stddef.h>
#include <metal/sys.h>
#include <metal/log.h>
#include <metal/utilities.h>

static int check_bitmap(const char *operation, unsigned int bit,
			unsigned int max, int actual, int expected)
{
	if (actual == expected)
		return 0;

	metal_err("Bitmap %s: bit %u, size %u, expected %d, got %d\n",
		  operation, bit, max, expected, actual);
	return 1;
}

int test_bitmap(void)
{
	unsigned long bitmap[3] = { 0, 0, ~0UL };
	unsigned int max = METAL_BITS_PER_ULONG + 3;
	unsigned int bits[] = { 0, METAL_BITS_PER_ULONG - 1,
				METAL_BITS_PER_ULONG, max - 1 };
	metal_log_handler saved_handler = metal_get_log_handler();
	enum metal_log_level saved_level = metal_get_log_level();
	unsigned int i;
	int error = 0;

	metal_set_log_handler(metal_default_log_handler);
	metal_set_log_level(METAL_LOG_ERROR);

	for (i = 0; i < metal_dim(bits); i++) {
		if (check_bitmap("is_set", bits[i], max,
				 metal_bitmap_is_set(bitmap, bits[i], max), false)) {
			error = 1;
			goto out;
		}
		if (check_bitmap("is_clear", bits[i], max,
				 metal_bitmap_is_clear(bitmap, bits[i], max), true)) {
			error = 1;
			goto out;
		}
		if (check_bitmap("set", bits[i], max,
				 metal_bitmap_set(bitmap, bits[i], max), 0)) {
			error = 1;
			goto out;
		}
		if (check_bitmap("is_set", bits[i], max,
				 metal_bitmap_is_set(bitmap, bits[i], max), true)) {
			error = 1;
			goto out;
		}
		if (check_bitmap("is_clear", bits[i], max,
				 metal_bitmap_is_clear(bitmap, bits[i], max), false)) {
			error = 1;
			goto out;
		}
	}
	for (i = 0; i < metal_dim(bits); i++) {
		if (check_bitmap("clear", bits[i], max,
				 metal_bitmap_clear(bitmap, bits[i], max), 0)) {
			error = 2;
			goto out;
		}
		if (check_bitmap("is_clear", bits[i], max,
				 metal_bitmap_is_clear(bitmap, bits[i], max), true)) {
			error = 2;
			goto out;
		}
		if (check_bitmap("is_set", bits[i], max,
				 metal_bitmap_is_set(bitmap, bits[i], max), false)) {
			error = 2;
			goto out;
		}
	}
	/* Invalid accesses must neither read nor modify bitmap storage. */
	bits[0] = max;
	bits[1] = max + 1;
	bits[2] = 2 * METAL_BITS_PER_ULONG;
	bits[3] = UINT_MAX;
	for (i = 0; i < metal_dim(bits); i++) {
		if (check_bitmap("set", bits[i], max,
				 metal_bitmap_set(bitmap, bits[i], max), -EINVAL)) {
			error = 3;
			goto out;
		}
		if (check_bitmap("clear", bits[i], max,
				 metal_bitmap_clear(bitmap, bits[i], max), -EINVAL)) {
			error = 3;
			goto out;
		}
		if (check_bitmap("is_set", bits[i], max,
				 metal_bitmap_is_set(bitmap, bits[i], max), false)) {
			error = 3;
			goto out;
		}
		if (check_bitmap("is_clear", bits[i], max,
				 metal_bitmap_is_clear(bitmap, bits[i], max), false)) {
			error = 3;
			goto out;
		}
	}
	for (i = 0; i < metal_dim(bitmap); i++) {
		unsigned long expected = i == 2 ? ~0UL : 0;

		if (bitmap[i] != expected) {
			metal_err("Bitmap word %u: expected %#lx, got %#lx\n",
				  i, expected, bitmap[i]);
			error = 4;
			goto out;
		}
	}
	if (check_bitmap("set", 0, 0,
			 metal_bitmap_set(NULL, 0, 0), -EINVAL)) {
		error = 5;
		goto out;
	}
	if (check_bitmap("clear", 0, 0,
			 metal_bitmap_clear(NULL, 0, 0), -EINVAL)) {
		error = 5;
		goto out;
	}
	if (check_bitmap("is_set", 0, 0,
			 metal_bitmap_is_set(NULL, 0, 0), false)) {
		error = 5;
		goto out;
	}
	if (check_bitmap("is_clear", 0, 0,
			 metal_bitmap_is_clear(NULL, 0, 0), false)) {
		error = 5;
		goto out;
	}
out:
	metal_set_log_level(saved_level);
	metal_set_log_handler(saved_handler);
	return error;
}
