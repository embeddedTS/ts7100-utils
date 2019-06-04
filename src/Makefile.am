GITCOMMIT:= $(shell git describe --abbrev=12 --dirty --always)

tsmicroctl_SOURCES = tsmicroctl.c
tsmicroctl_CFLAGS = -Wall -fno-tree-cselim
tsmicroctl_CPPFLAGS = -DCTL -DGITCOMMIT="\"${GITCOMMIT}\""

tshwctl_SOURCES = tshwctl.c fpga.c eval_cmdline.c
tshwctl_CFLAGS = -Wall -fno-tree-cselim
tshwctl_CPPFLAGS = -DGITCOMMIT="\"${GITCOMMIT}\""

bin_PROGRAMS = tsmicroctl tshwctl
