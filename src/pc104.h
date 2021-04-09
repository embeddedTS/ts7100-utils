#ifndef __PC104_H__
#define __PC104_H__

#include <stdint.h>

/*
 * This sets up a mmap to the pc104 space.
 * 0-0xFFFFF           IO
 * 0x100000-0x1FFFFF   MEM
 *
 * This calls pc104_init()
 * To use this option the code must be compiled with -marm.
 */
void *pc104_mmap_init();

/* This must be run before any of the below pc104 calls */
void pc104_init(void);

/* These all directly access the kernel driver to create 8,
 * 16-bit, and alt 16-bit accesses to the PC104 bus.
 *
 * All addresses are byte addresses, and 16-bit accesses must
 * be on even addresses. */
void pc104_io_8_write(uint32_t addr, uint8_t val);
void pc104_io_16_write(uint32_t addr, uint16_t val);
void pc104_io_16_alt_write(uint32_t addr, uint16_t val);

uint8_t pc104_io_8_read(uint32_t addr);
uint16_t pc104_io_16_read(uint32_t addr);
uint16_t pc104_io_16_alt_read(uint32_t addr);

void pc104_mem_8_write(uint32_t addr, uint8_t val);
void pc104_mem_16_write(uint32_t addr, uint16_t val);
void pc104_mem_16_alt_write(uint32_t addr, uint16_t val);

uint8_t pc104_mem_8_read(uint32_t addr);
uint16_t pc104_mem_16_read(uint32_t addr);
uint16_t pc104_mem_16_alt_read(uint32_t addr);

#endif // __PC104_H__