#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "pc104.h"

#define ISA_PATH "/sys/devices/soc0/soc/2100000.aips-bus/21b8000.weim/21b8000.weim:fpga@50000000/50004000.syscon/50004050.fpgaisa/"

static int io8fd;
static int io16fd;
static int io16altfd;
static int mem8fd;
static int mem16fd;
static int mem16altfd;

void pc104_init(void)
{
	io8fd = open(ISA_PATH "io8", O_RDWR|O_SYNC);
	assert(io8fd != -1);
	io16fd = open(ISA_PATH "io16", O_RDWR|O_SYNC);
	assert(io16fd != -1);
	io16altfd = open(ISA_PATH "ioalt16", O_RDWR|O_SYNC);
	assert(io16altfd != -1);
	mem8fd = open(ISA_PATH "mem8", O_RDWR|O_SYNC);
	assert(mem8fd != -1);
	mem16fd = open(ISA_PATH "mem16", O_RDWR|O_SYNC);
	assert(mem16fd != -1);
	mem16altfd = open(ISA_PATH "memalt16", O_RDWR|O_SYNC);
	assert(mem16altfd != -1);
}

uint8_t pc104_io_8_read(uint16_t addr)
{
	uint8_t val;
	int ret;

	ret = lseek(io8fd, addr, SEEK_SET);
	assert(ret != -1);

	ret = read(io8fd, &val, 1);
	assert(ret == 1);

	return val;
}

void pc104_io_8_write(uint16_t addr, uint8_t val)
{
	int ret;

	ret = lseek(io8fd, addr, SEEK_SET);
	assert(ret != -1);

	ret = write(io8fd, &val, 1);
	assert(ret == 1);
}

uint16_t pc104_io_16_read(uint16_t addr)
{
	uint16_t val;
	int ret;

	ret = lseek(io16fd, addr, SEEK_SET);
	assert(ret != -1);

	ret = read(io16fd, &val, 2);
	assert(ret == 2);

	return val;
}

void pc104_io_16_write(uint16_t addr, uint16_t val)
{
	int ret;

	ret = lseek(io16fd, addr, SEEK_SET);
	assert(ret != -1);

	ret = write(io16fd, &val, 2);
	assert(ret == 2);
}

uint16_t pc104_io_16_alt_read(uint16_t addr)
{
	uint16_t val;
	int ret;

	ret = lseek(io16altfd, addr, SEEK_SET);
	assert(ret != -1);

	ret = read(io16altfd, &val, 2);
	assert(ret == 2);

	return val;
}

void pc104_io_16_alt_write(uint16_t addr, uint16_t val)
{
	int ret;

	ret = lseek(io16altfd, addr, SEEK_SET);
	assert(ret != -1);

	ret = write(io16altfd, &val, 2);
	assert(ret == 2);
}

uint8_t pc104_mem_8_read(uint16_t addr)
{
	uint8_t val;
	int ret;

	ret = lseek(mem8fd, addr, SEEK_SET);
	assert(ret != -1);

	ret = read(mem8fd, &val, 1);
	assert(ret == 1);

	return val;
}

void pc104_mem_8_write(uint16_t addr, uint8_t val)
{
	int ret;

	ret = lseek(mem8fd, addr, SEEK_SET);
	assert(ret != -1);

	ret = write(mem8fd, &val, 1);
	assert(ret == 1);
}

uint16_t pc104_mem_16_read(uint16_t addr)
{
	uint16_t val;
	int ret;

	ret = lseek(mem16fd, addr, SEEK_SET);
	assert(ret != -1);

	ret = read(mem16fd, &val, 2);
	assert(ret == 2);

	return val;
}

void pc104_mem_16_write(uint16_t addr, uint16_t val)
{
	int ret;

	ret = lseek(mem16fd, addr, SEEK_SET);
	assert(ret != -1);

	ret = write(mem16fd, &val, 2);
	assert(ret == 2);
}

uint16_t pc104_mem_16_alt_read(uint16_t addr)
{
	uint16_t val;
	int ret;

	ret = lseek(mem16altfd, addr, SEEK_SET);
	assert(ret != -1);

	ret = read(mem16altfd, &val, 2);
	assert(ret == 2);

	return val;
}

void pc104_mem_16_alt_write(uint16_t addr, uint16_t val)
{
	int ret;

	ret = lseek(mem16altfd, addr, SEEK_SET);
	assert(ret != -1);

	ret = write(mem16altfd, &val, 2);
	assert(ret == 2);
}
