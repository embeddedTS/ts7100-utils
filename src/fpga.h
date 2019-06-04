/* SPDX-License-Identifier: BSD-2-Clause */
/* Copyright (c) 2019, Technologic Systems Inc. */

#ifndef __FPGA_H_
#define __FPGA_H_

void fpga_init(size_t base);
void fpoke16(size_t offs, uint16_t value);
uint16_t fpeek16(size_t offs);
void fpoke32(size_t offs, uint32_t value);
uint32_t fpeek32(size_t offs);

#endif
