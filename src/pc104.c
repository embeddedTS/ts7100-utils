#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <ucontext.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include "pc104.h"

#define ISA_PATH "/sys/devices/soc0/soc/2100000.aips-bus/21b8000.weim/21b8000.weim:fpga@50000000/50004000.syscon/50004050.fpgaisa/"

static int io8fd;
static int io16fd;
static int io16altfd;
static int mem8fd;
static int mem16fd;
static int mem16altfd;
static ssize_t bus_space_sz = 0x200000;
static uint8_t *bus_space;

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

static inline void set_reg(ucontext_t *ctx, uint8_t rd, uint32_t val) {
	switch (rd & 0xf) {
	case 0: ctx->uc_mcontext.arm_r0 = val; break;
	case 1: ctx->uc_mcontext.arm_r1 = val; break;
	case 2: ctx->uc_mcontext.arm_r2 = val; break;
	case 3: ctx->uc_mcontext.arm_r3 = val; break;
	case 4: ctx->uc_mcontext.arm_r4 = val; break;
	case 5: ctx->uc_mcontext.arm_r5 = val; break;
	case 6: ctx->uc_mcontext.arm_r6 = val; break;
	case 7: ctx->uc_mcontext.arm_r7 = val; break;
	case 8: ctx->uc_mcontext.arm_r8 = val; break;
	case 9: ctx->uc_mcontext.arm_r9 = val; break;
	case 10: ctx->uc_mcontext.arm_r10 = val; break;
	case 11: ctx->uc_mcontext.arm_fp = val; break;
	case 12: ctx->uc_mcontext.arm_ip = val; break;
	case 13: ctx->uc_mcontext.arm_sp = val; break;
	case 14: ctx->uc_mcontext.arm_lr = val; break;
	case 15: ctx->uc_mcontext.arm_pc = val; break;
	}
}

static inline uint32_t get_reg(ucontext_t *ctx, uint8_t rd) {
	switch (rd & 0xf) {
	case 0: return ctx->uc_mcontext.arm_r0;
	case 1: return ctx->uc_mcontext.arm_r1;
	case 2: return ctx->uc_mcontext.arm_r2;
	case 3: return ctx->uc_mcontext.arm_r3;
	case 4: return ctx->uc_mcontext.arm_r4;
	case 5: return ctx->uc_mcontext.arm_r5;
	case 6: return ctx->uc_mcontext.arm_r6;
	case 7: return ctx->uc_mcontext.arm_r7;
	case 8: return ctx->uc_mcontext.arm_r8;
	case 9: return ctx->uc_mcontext.arm_r9;
	case 10: return ctx->uc_mcontext.arm_r10;
	case 11: return ctx->uc_mcontext.arm_fp;
	case 12: return ctx->uc_mcontext.arm_ip;
	case 13: return ctx->uc_mcontext.arm_sp;
	case 14: return ctx->uc_mcontext.arm_lr;
	case 15: return ctx->uc_mcontext.arm_pc;
	}
}

static void fault(int signum, siginfo_t *info, void *vcontext) {
	ucontext_t *context = (ucontext_t *)vcontext;
	uint32_t opcode = *(uint32_t *)context->uc_mcontext.arm_pc;
	size_t adr = context->uc_mcontext.fault_address;
	uint8_t rd = ((opcode >> 12) & 0xf);

	if (adr < (size_t)bus_space || adr >= (size_t)bus_space + bus_space_sz)
		raise(SIGKILL);

	context->uc_mcontext.arm_pc += 4;
	adr -= (size_t)bus_space;

	if(adr > 0x100000) { /* Mem access */
		if ((opcode & 0xc500000) == 0x4500000) { /* ldrb, 8-bit */
			set_reg(context, rd, pc104_mem_8_read(adr));
		} else if ((opcode & 0xe1000f0) == 0x1000b0) { /* ldrh, 16-bit */
			set_reg(context, rd, pc104_mem_16_read(adr));
		} else if ((opcode & 0xc500000) == 0x4100000) { /* ldr, 32-bit */
			uint32_t val = pc104_mem_16_read(adr);
			val |= ((uint32_t)pc104_mem_16_read(adr + 2)) << 16;
			set_reg(context, rd, val);
		} else if ((opcode & 0xc500000) == 0x4400000) { /* strb, 8-bit */
			pc104_mem_8_write(adr, get_reg(context, rd));
		} else if ((opcode & 0xc500000) == 0x4000000) { /* str, 32-bit */
			uint32_t val = get_reg(context, rd);
			pc104_mem_16_write(adr, (uint16_t)val);
			pc104_mem_16_write(adr + 2, (uint16_t)(val >> 16));
		} else if ((opcode & 0xe1000f0) == 0xb0) { /* strh, 16-bit */
			pc104_mem_16_write(adr, get_reg(context, rd));
		} else {
			fprintf(stderr, "this must be built with -marm\n");
			raise(SIGKILL);
		}
	} else {
		if ((opcode & 0xc500000) == 0x4500000) { /* ldrb, 8-bit */
			set_reg(context, rd, pc104_io_8_read(adr));
		} else if ((opcode & 0xe1000f0) == 0x1000b0) { /* ldrh, 16-bit */
			set_reg(context, rd, pc104_io_16_read(adr));
		} else if ((opcode & 0xc500000) == 0x4100000) { /* ldr, 32-bit */
			uint32_t val = pc104_io_16_read(adr);
			val |= ((uint32_t)pc104_io_16_read(adr + 2)) << 16;
			set_reg(context, rd, val);
		} else if ((opcode & 0xc500000) == 0x4400000) { /* strb, 8-bit */
			pc104_io_8_write(adr, get_reg(context, rd));
		} else if ((opcode & 0xc500000) == 0x4000000) { /* str, 32-bit */
			uint32_t val = get_reg(context, rd);
			pc104_io_16_write(adr, (uint16_t)val);
			pc104_io_16_write(adr + 2, (uint16_t)(val >> 16));
		} else if ((opcode & 0xe1000f0) == 0xb0) { /* strh, 16-bit */
			pc104_io_16_write(adr, get_reg(context, rd));
		} else {
			fprintf(stderr, "this must be built with -marm\n");
			raise(SIGKILL);
		}
	}
}

void *pc104_mmap_init() {
	struct sigaction act;

	pc104_init();

	memset(&act, 0, sizeof act);
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_SIGINFO;
	act.sa_sigaction = fault;
	if (sigaction(SIGSEGV, &act, NULL) == -1 ||
	    sigaction(SIGBUS, &act, NULL) == -1) {
		const int retval = errno;
		fprintf(stderr, "Cannot install fault signal handlers: %s.\n",
		strerror(retval));
		return NULL;
	}

	bus_space = mmap(NULL, bus_space_sz, PROT_NONE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (bus_space == MAP_FAILED) {
		const int retval = errno;
		fprintf(stderr, "Unable to create fault address space\n");
		return NULL;
	}
	return bus_space;
}
