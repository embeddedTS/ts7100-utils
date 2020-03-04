#ifndef __PC104_H__
#define __PC104_H__

#include <stdint.h>

void pc104_init(void);

void pc104_io_8_write(uint16_t addr, uint8_t val);
void pc104_io_16_write(uint16_t addr, uint16_t val);
void pc104_io_16_alt_write(uint16_t addr, uint16_t val);

uint8_t pc104_io_8_read(uint16_t addr);
uint16_t pc104_io_16_read(uint16_t addr);
uint16_t pc104_io_16_alt_read(uint16_t addr);

void pc104_mem_8_write(uint16_t addr, uint8_t val);
void pc104_mem_16_write(uint16_t addr, uint16_t val);
void pc104_mem_16_alt_write(uint16_t addr, uint16_t val);

uint8_t pc104_mem_8_read(uint16_t addr);
uint16_t pc104_mem_16_read(uint16_t addr);
uint16_t pc104_mem_16_alt_read(uint16_t addr);

#endif // __PC104_H__