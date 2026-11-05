/*
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <limits.h>
#include <stddef.h>
#include <metal/utilities.h>

int test_bitmap(void)
{
	unsigned long bitmap[3] = { 0, 0, ~0UL };
	unsigned int max = METAL_BITS_PER_ULONG + 3;
	unsigned int bits[] = { 0, METAL_BITS_PER_ULONG - 1,
				METAL_BITS_PER_ULONG, max - 1 };
	unsigned int i;

	for (i = 0; i < metal_dim(bits); i++) {
		if (metal_bitmap_is_set(bitmap, bits[i], max) != 0 ||
		    metal_bitmap_is_clear(bitmap, bits[i], max) != 1 ||
		    metal_bitmap_set(bitmap, bits[i], max) != 0 ||
		    metal_bitmap_is_set(bitmap, bits[i], max) != 1 ||
		    metal_bitmap_is_clear(bitmap, bits[i], max) != 0)
			return 1;
	}
	for (i = 0; i < metal_dim(bits); i++) {
		if (metal_bitmap_clear(bitmap, bits[i], max) != 0 ||
		    metal_bitmap_is_clear(bitmap, bits[i], max) != 1)
			return 2;
	}
	/* Invalid accesses must neither read nor modify bitmap storage. */
	bits[0] = max;
	bits[1] = max + 1;
	bits[2] = 2 * METAL_BITS_PER_ULONG;
	bits[3] = UINT_MAX;
	for (i = 0; i < metal_dim(bits); i++) {
		if (metal_bitmap_set(bitmap, bits[i], max) != -EINVAL ||
		    metal_bitmap_clear(bitmap, bits[i], max) != -EINVAL ||
		    metal_bitmap_is_set(bitmap, bits[i], max) != -EINVAL ||
		    metal_bitmap_is_clear(bitmap, bits[i], max) != -EINVAL)
			return 3;
	}
	if (bitmap[0] != 0 || bitmap[1] != 0 || bitmap[2] != ~0UL)
		return 4;
	if (metal_bitmap_set(NULL, 0, 0) != -EINVAL ||
	    metal_bitmap_clear(NULL, 0, 0) != -EINVAL ||
	    metal_bitmap_is_set(NULL, 0, 0) != -EINVAL ||
	    metal_bitmap_is_clear(NULL, 0, 0) != -EINVAL)
		return 5;
	return 0;
}
