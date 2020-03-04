GITCOMMIT:= $(shell git describe --abbrev=12 --dirty --always)

silabs_SOURCES = silabs.c
silabs_CFLAGS = -Wall

tshwctl_SOURCES = tshwctl.c fpga.c eval_cmdline.c helpers.c
tshwctl_CFLAGS = -Wall -fno-tree-cselim
tshwctl_CPPFLAGS = -DGITCOMMIT="\"${GITCOMMIT}\""

lcdmesg_SOURCES = lcdmesg.c helpers.c
lcdmesg_CFLAGS = -lgpiod -Wall

pc104_peekpoke_SOURCES = pc104_peekpoke.c helpers.c pc104.c
pc104_peekpoke_CFLAGS = -Wall

bin_PROGRAMS = silabs tshwctl lcdmesg pc104_peekpoke
