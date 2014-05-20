# makefile for aleph-blackfin core objects.
#
# this should be included from module makefiles.

# paths are relative to module directory
bfin_lib_dir = ../../bfin_lib/
bfin_lib_srcdir = $(bfin_lib_dir)src/
bfin_lib_objdir = $(bfin_lib_dir)obj/
common_dir = $(bfin_lib_dir)/../common
audio_dir = $(bfin_lib_dir)/../dsp
module_dir = ./

bfin_lib_src = control.c \
	cv.c \
	init.c \
	isr.c \
	main.c \
	spi.c \
	util.c

bfin_lib_obj = $(patsubst %.c, %.o, $(bfin_lib_src))

INC += -I$(bfin_lib_srcdir) \
	-I$(bfin_lib_srcdir)/libfixmath \
	-I$(common_dir) \
	-I$(audio_dir) \
	-I$(module_dir)

CROSS_COMPILE = bfin-elf-
CC = $(CROSS_COMPILE)gcc
LDR = $(CROSS_COMPILE)ldr
CPU = bf533
CFLAGS += -Wall -mcpu=$(CPU) $(INC)
# CFLAGS += -00 -g # debug
# CFLAGS += -02
CFLAGS += -03

LDFLAGS += -mcpu=$(CPU)
# LDRFLAGS += --initcode $(bfin_lib_objdir)init.o
LDRFLAGS += --bits 16 --dma 8
LDRFLAGS += --bmode spi_slave --port F --gpio 2
LDRFLAGS += --verbose

bfin_lib_target: $(patsubst %.o, $(bfin_lib_objdir)%.o, $(bfin_lib_obj))
	@echo bfin_lib objects are complete in $(bfin_lib_objdir)

$(bfin_lib_objdir)%.o : # $(bfin_lib_srcdir)%.c
	$(CC) $(CFLAGS) $(INC) -c \
	-D MAJ=$(maj) -D MIN=$(min) -D REV=$(rev) \
	$(patsubst $(bfin_lib_objdir)%.o, $(bfin_lib_srcdir)%.c, $@) \
	-o $@

%.ldr: %
	$(LDR) -T $(CPU) -c $(LDRFLAGS) $@ $<

bfin_lib_clean:
	rm $(bfin_lib_objdir)*.o

.PHONY: bfin_lib_target bfin_lib_clean
