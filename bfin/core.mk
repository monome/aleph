# makefile for aleph-blackfin core objects.
#
# this should be included from module makefiles.

core_src = main.c init.c isr.c spi.c
core_obj = $(patsubst %.c, %.o, $(core_src))

core_dir = ../../
core_srcdir = $(core_dir)src/
core_objdir = $(core_dir)obj/

CROSS_COMPILE = bfin-elf-
CC = $(CROSS_COMPILE)gcc
LDR = $(CROSS_COMPILE)ldr
CPU = bf533
CFLAGS += -Wall -g -mcpu=$(CPU) -I$(core_srcdir) -I$(core_dir)../common/
INC += -I$(core_srcdir) -I$(core_dir)../common/
LDFLAGS += -mcpu=$(CPU)
# LDRFLAGS += --initcode $(core_objdir)init.o
LDRFLAGS += --bits 16 --dma 8
LDRFLAGS += --bmode spi_slave --port F --gpio 11
LDRFLAGS += --verbose

core_target: $(patsubst %.o, $(core_objdir)%.o, $(core_obj))
	@echo core objects are complete in $(core_objdir)

$(core_objdir)%.o : # $(core_srcdir)%.c
	$(CC) $(CFLAGS) $(INC) -c \
	$(patsubst $(core_objdir)%.o, $(core_srcdir)%.c, $@) \
	-o $@

%.ldr: %
	$(LDR) -T $(CPU) -c $(LDRFLAGS) $@ $<

core_clean:
	rm $(core_objdir)*.o

.PHONY: core_target core_clean
