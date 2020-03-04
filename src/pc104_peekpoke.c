#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <assert.h>

#include "pc104.h"
#include "helpers.h"

void usage(char *name)
{
	fprintf(stderr, "Usage %s <io/mem> <8/16/alt16> <address> [value]\n", name);
	fprintf(stderr, "\tEg: %s io 8 0x140\n", name);
}

int main(int argc, char **argv) {
	int sz;
	uint32_t off, val;
	int is_io = 0;

	if(get_model() != 0x7250) {
		fprintf(stderr, "Only supported on the TS-7250-V3\n");
		return 1;
	}

	if(argc != 4 && argc != 5) {
		usage(argv[0]);
		return 1;
	}

	if(strstr(argv[1], "io"))
		is_io = 1;

	if(argv[2][0] == '8')
		sz = 1;
	else if (argv[2][0] == '1') /* 16-bit */
		sz = 2;
	else if (argv[2][0] == 'a') /* 16-bit alt pinout */
		sz = 3;
	else {
		fprintf(stderr, "Invalid bus width\n");
		return 1;
	}

	off = strtoul(argv[3], NULL, 0);
	if(argc == 5) val = strtoul(argv[4], NULL, 0);

	pc104_init();

	if (argc == 4) {
		if(is_io) {
			if (sz == 1)
				val = pc104_io_8_read(off);
			else if (sz == 2)
				val = pc104_io_16_read(off);
			else if (sz == 3)
				val = pc104_io_16_alt_read(off);
		} else {
			if (sz == 1)
				val = pc104_mem_8_read(off);
			else if (sz == 2)
				val = pc104_mem_16_read(off);
			else if (sz == 3)
				val = pc104_mem_16_alt_read(off);
		}
		printf("0x%X\n", val);
	} else {
		if(is_io) {
			if (sz == 1)
				pc104_io_8_write(off, val);
			else if (sz == 2)
				pc104_io_16_write(off, val);
			else if (sz == 3)
				pc104_io_16_alt_write(off, val);
		} else {
			if (sz == 1)
				pc104_mem_8_write(off, val);
			else if (sz == 2)
				pc104_mem_16_write(off, val);
			else if (sz == 3)
				pc104_mem_16_alt_write(off, val);
		}
	}

	return 0;
}
