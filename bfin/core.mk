# makefile for aleph-blackfin core objects.
#
# this should be included from module makefiles.

core_src = main.c init.c isr.c spi.c
core_obj = $(patsubst %.c, %.o, $(core_src))

core_srcdir = ../../src/
core_objdir = ../../obj/

CROSS_COMPILE = bfin-elf-
CC = $(CROSS_COMPILE)gcc
LDR = $(CROSS_COMPILE)ldr
CPU = bf533
CFLAGS += -Wall -mcpu=$(CPU)
LDFLAGS += -mcpu=$(CPU)
LDRFLAGS += --initcode $(core_objdir)init.o

core_target: $(patsubst %.o, $(core_objdir)%.o, $(core_obj))
	@echo core objects are complete in $(core_objdir)

$(core_objdir)%.o : # $(core_srcdir)%.c
	$(CC) $(CFLAGS) -c \
	$(patsubst $(core_objdir)%.o, $(core_srcdir)%.c, $@) \
	-o $@

%.ldr: %
	$(LDR) -T $(CPU) -c $(LDRFLAGS) $@ $<

core_clean:
	rm $(core_objdir)*.o

.PHONY: core_target core_clean