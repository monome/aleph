# Makefile for aleph-blackfin modules.
#
# CUSTOMIZING:
# 
# if you use a single source file, you can call it [yourname].c, 
# set the module_name variable to [yourname],
# and use this file without further modification.
#
# if you need to allocate external memory, 
# you should also create [yourname]_data.c,
# which will be  linked to SDRAM by the linker script.
# you must edit the appropriate line in the .lds file
# to use [yourname]_data.o
#
# if you want to use multiple source files, you must edit these targets.

module_name = echo

all: aleph-$(module_name).ldr

include ../../core.mk

aleph-$(module_name): core_target $(module_name).c $(module_name)_data.c
	$(CC) $(CFLAGS) -I../../src/ -c $(module_name).c -o $(module_name).o
	$(CC) $(CFLAGS) -I../../src/ -c $(module_name)_data.c -o $(module_name)_data.o
	$(CC) $(LDFLAGS) -T aleph-$(module_name).lds \
	$(patsubst %.o, $(core_objdir)%.o, $(core_obj)) \
	$(module_name).o $(module_name)_data.o \
	-o aleph-$(module_name)

clean: core_clean
	rm aleph-$(module_name).ldr 
	rm aleph-$(module_name)
	rm $(module_name).o
	rm $(module_name)_data.o

.PHONY: clean