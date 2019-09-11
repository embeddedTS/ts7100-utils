/* SPDX-License-Identifier: BSD-2-Clause */
/* Copyright (c) 2019, Technologic Systems Inc. */

/* Init I2C bus to microcontroller
 *
 * Args:
 *	i2c-dev:	String, path of linux i2c-dev interface file
 *	i2c_adr:	7-bit I2C address, i.e. bits 8:1 are the device addr
 *
 * Return Value:
 *	Returns FD number. Returns -1 in case of error with errno set
 */
int silab_init(const char *i2c_dev, int i2c_adr);

/* Perform 8-bit/16-bit read on arbitrary I2C register
 *
 * Args:
 *	subadr:		Register address
 *
 * Notes:
 *	On 16-bit operations, the lower I2C reg is returned in the MSB
 *
 * Return Value:
 *	Returns unsigned 8-bit/16-bit register value. Returns -1 in case of
 *	  error with errno set.
 */
int16_t silab_peek8(uint16_t subadr);
int32_t silab_peek16(uint16_t subadr);

/* Perform 8-bit/16-bit write on arbitrary I2C register
 *
 * Args:
 *	subadr:		Register address
 *	dat:		Unsigned 8-bit/16-bit value to write
 *
 * Notes:
 *	On 16-bit operations, the lower I2C reg is written with the MSB of dat
 *
 * Return Value:
 *	XXX:
 */
int8_t silab_poke8(uint16_t subadr, uint8_t dat);
int8_t silab_poke16(uint16_t subadr, uint16_t dat);

/* Read/write a stream of bytes to I2C registers
 *
 * Args:
 *	subadr:		Starting register address
 *	buf:		Buffer to read to/write from
 *	len:		Number of bytes to read/write
 *
 * Return Value:
 *	XXX:
 */
int8_t silab_peekstream8(uint16_t subadr, uint8_t *buf, int len);
int silab_pokestream8(uint16_t subadr, uint8_t *buf, int len);
