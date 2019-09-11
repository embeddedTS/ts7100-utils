/* SPDX-License-Identifier: BSD-2-Clause */
/* Copyright (c) 2019, Technologic Systems Inc. */

#include <alloca.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>


#include "i2c-dev.h"

static int i2c_adr;
static int i2c_fd = -1;

int silab_init(const char *i2c_dev, int adr)
{
	int fd;

	if (i2c_fd != -1) {
		return i2c_fd;
	}

	i2c_adr = adr;

	fd = open(i2c_dev, O_RDWR);
	if(fd != -1) {
		if (ioctl(fd, I2C_SLAVE_FORCE, i2c_adr) < 0) {
			return -1;
		}
		i2c_fd = fd;
	}

	return fd;
}

/* Performs 8-bit reads from I2C EEPROM device.
 *
 * The TS supervisory microcontroller is set up to mimic an I2C EEPROM device
 * when reading and writing to provide a standard interface.
 *
 * Args:
 * 	adr:		7-bit I2C address, i.e. bits 8:1 are the device addr
 *	subadr:		16-bit register address
 * 	buf:		Pointer to buffer to write data to
 * 	len:		Number of bytes to read starting at <subadr>
 *
 * Return Value:
 * 	XXX:
 */
int8_t i2c_eeprom_read(uint8_t adr, uint16_t subadr, uint8_t *buf, int len)
{
	struct i2c_rdwr_ioctl_data packets;
	struct i2c_msg msgs[2];
	char busaddr[2];

	assert(i2c_fd != -1);

	/* Write 16-bit register address to device */
	msgs[0].addr  = adr;
	msgs[0].flags = 0;
	msgs[0].len   = 2;
	busaddr[0]    = ((subadr >> 8) & 0xff);
	busaddr[1]    = (subadr & 0xff);
	msgs[0].buf   = busaddr;

	/* Read <len> bytes in to <buf> */
	msgs[1].addr  = adr;
	msgs[1].flags = I2C_M_RD;
	msgs[1].len   = len;
	msgs[1].buf   = (char *)buf;

	/* Set up message packet */
	packets.msgs = msgs;
	packets.nmsgs = 2;

	/* XXX: What does this ioctl actually return? */
	return ioctl(i2c_fd, I2C_RDWR, &packets) < 0;
}

int16_t silab_peek8(uint16_t subadr)
{
	int r;
	uint8_t buf[1];

	r = i2c_eeprom_read(i2c_adr, subadr, buf, 1);
	if (r == -1) return -1;
	else return buf[0];
}

int32_t silab_peek16(uint16_t subadr)
{
	int r;
	uint8_t buf[2];

	r = i2c_eeprom_read(i2c_adr, subadr, buf, 2);
	if (r == -1) return -1;
	else return ((buf[0]<<8)|buf[1]);
}

int8_t silab_peekstream8(uint16_t subadr, uint8_t *buf, int len)
{
	return i2c_eeprom_read(i2c_adr, subadr, buf, len);
}

int8_t i2c_eeprom_write(uint8_t adr, uint16_t subadr, uint8_t *buf, int len)
{
	struct i2c_rdwr_ioctl_data packets;
	struct i2c_msg msg;
	uint8_t *buf2 = (uint8_t *)alloca(len+2);

	assert(i2c_fd != -1);

	buf2[0] = subadr >> 8;
	buf2[1] = subadr & 0xff;
	memcpy(&buf2[2], buf, len);
	msg.addr = adr;
	msg.flags = 0;
	msg.len = 2 + len;
	msg.buf = (char *)buf2;
	packets.msgs = &msg;
	packets.nmsgs = 1;

	/* XXX: what does this ioctl actually return */
	return ioctl(i2c_fd, I2C_RDWR, &packets) < 0;
}


int8_t silab_poke16(uint16_t subadr, uint16_t dat)
{
	uint8_t buf[2];

	buf[0] = dat >> 8;
	buf[1] = dat & 0xff ;
	return i2c_eeprom_write(i2c_adr, subadr, buf, 2);
}

int8_t silab_poke8(uint16_t subadr, uint8_t dat) {
	uint8_t buf[1];

	buf[0] = dat;
	return i2c_eeprom_write(i2c_adr, subadr, buf, 1);
}

int silab_pokestream8(uint16_t subadr, uint8_t *buf, int len)
{
	return i2c_eeprom_write(i2c_adr, subadr, buf, len);
}
