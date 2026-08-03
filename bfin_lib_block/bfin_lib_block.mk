# makefile for aleph-blackfin core objects. (block-processing version)
#
# this should be included from module makefiles.

# paths are relative to module directory
bfin_lib_dir = ../../bfin_lib_block/
bfin_lib_srcdir = $(bfin_lib_dir)src/
# core objects bake in the including module's module_custom.h (blocksize,
# xrun/meter opt-ins), so each module needs its own object dir. sharing one
# dir silently links a module against whichever module was built last.
bfin_lib_objdir = $(bfin_lib_dir)obj/$(module_name)/
common_dir = $(bfin_lib_dir)/../common
audio_dir = $(bfin_lib_dir)/../dsp
module_dir = ./

dsp_block_dir = $(bfin_lib_dir)/../dsp_block

bfin_lib_src = audio.c \
	clock_ebiu.c \
	control.c \
	cv.c \
	gpio.c \
	dma.c \
	isr.c \
	main.c \
	meters.c \
	serial.c \
	spi.c

bfin_lib_obj = $(patsubst %.c, %.o, $(bfin_lib_src))

# coarse but cheap: the core is a dozen files and rebuilds in about a second,
# so rebuild all of it whenever any core header or the module config changes.
bfin_lib_hdr = $(wildcard $(bfin_lib_srcdir)*.h) $(module_dir)module_custom.h

INC += -I$(bfin_lib_srcdir) \
	-I$(bfin_lib_srcdir)/libfixmath \
	-I$(common_dir) \
	-I$(audio_dir) \
	-I$(dsp_block_dir) \
	-I$(module_dir)

CROSS_COMPILE = bfin-elf-
CC = $(CROSS_COMPILE)gcc
LDR = $(CROSS_COMPILE)ldr
CPU = bf533
CFLAGS += -Wall -mcpu=$(CPU) $(INC)
# CFLAGS += -00 -g # debug
# CFLAGS += -02
CFLAGS += -03
#CFLAGS += -funsafe-loop-optimizations
#CFLAGS += -funsafe-math-optimizations
#CFLAGS += -mfast-fp

LDFLAGS += -mcpu=$(CPU)
LDRFLAGS += --bits 16 --dma 8
LDRFLAGS += --bmode spi_slave --port F --gpio 2
LDRFLAGS += --verbose

bfin_lib_target: $(patsubst %.o, $(bfin_lib_objdir)%.o, $(bfin_lib_obj))
	@echo bfin_lib objects are complete in $(bfin_lib_objdir)

$(bfin_lib_objdir):
	mkdir -p $@

$(bfin_lib_objdir)%.o : $(bfin_lib_srcdir)%.c $(bfin_lib_hdr) | $(bfin_lib_objdir)
	$(CC) $(CFLAGS) $(INC) -c \
	-D MAJ=$(maj) -D MIN=$(min) -D REV=$(rev) \
	$< -o $@

%.ldr: %
	$(LDR) -T $(CPU) -c $(LDRFLAGS) $@ $<

bfin_lib_clean:
	rm -rf $(bfin_lib_objdir)

.PHONY: bfin_lib_target bfin_lib_clean
