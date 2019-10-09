GITCOMMIT:= $(shell git describe --abbrev=12 --dirty --always)

silabs_SOURCES = silabs.c
silabs_CFLAGS = -Wall

tshwctl_SOURCES = tshwctl.c fpga.c eval_cmdline.c
tshwctl_CFLAGS = -Wall -fno-tree-cselim
tshwctl_CPPFLAGS = -DGITCOMMIT="\"${GITCOMMIT}\""

bin_PROGRAMS = silabs tshwctl
