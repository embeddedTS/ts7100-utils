/* SPDX-License-Identifier: BSD-2-Clause */
/* Copyright (c) 2019, Technologic Systems Inc. */

/* This file provides a handful of self-contained memory-mapped FPGA access
 * routines.
 *
 * This implementation assumes all accesses must be 32 or 16 bit aligned, and
 * 32 or 16 bits wide.
 */

#include <assert.h>
#include <errno.h>
#include <error.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

static volatile void *fpgaregs = NULL;
static int devmemfd;

void fpga_init(size_t base)
{
	if (fpgaregs != NULL) {
		return;
	}

	devmemfd = open("/dev/mem", O_RDWR|O_SYNC);
	if (devmemfd == -1) {
		error(errno, errno, "Unable to open /dev/mem for FPGA access");
	}

	fpgaregs = mmap(0, getpagesize(),
          PROT_READ | PROT_WRITE, MAP_SHARED, devmemfd, base);
	if ((int32_t)fpgaregs == -1) {
		close(devmemfd);
		error(errno, errno, "Unable to map address space for FPGA");
	}
}

void fpoke16(size_t offs, uint16_t value)
{
	assert(fpgaregs != NULL);
	assert(offs < getpagesize());
	assert((offs & 0x1) == 0);

	*(volatile uint16_t *)(fpgaregs+offs) = value;
}

uint16_t fpeek16(size_t offs)
{
	assert(fpgaregs != NULL);
	assert(offs < getpagesize());
	assert((offs & 0x1) == 0);

	return *(volatile uint16_t *)(fpgaregs+offs);
}

void fpoke32(size_t offs, uint32_t value)
{
	assert(fpgaregs != NULL);
	assert(offs < getpagesize());
	assert((offs & 0x3) == 0);

	*(volatile uint32_t *)(fpgaregs+offs) = value;
}

uint32_t fpeek32(size_t offs)
{
	assert(fpgaregs != NULL);
	assert(offs < getpagesize());
	assert((offs & 0x3) == 0);

	return *(volatile uint32_t *)(fpgaregs+offs);
}
