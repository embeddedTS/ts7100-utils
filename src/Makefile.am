GITCOMMIT:= $(shell git describe --abbrev=12 --dirty --always)

CFLAGS=-Wall -fno-tree-cselim

silabs_SOURCES = silabs.c

tshwctl_SOURCES = tshwctl.c fpga.c eval_cmdline.c helpers.c
tshwctl_CPPFLAGS = -DGITCOMMIT="\"${GITCOMMIT}\""

lcdmesg_SOURCES = lcdmesg.c helpers.c fpga.c
lcdmesg_LDADD = -lgpiod

keypad_SOURCES = keypad.c helpers.c
keypad_LDADD = -lgpiod

pc104_peekpoke_SOURCES = pc104_peekpoke.c helpers.c pc104.c

bin_PROGRAMS = silabs tshwctl lcdmesg pc104_peekpoke keypad
