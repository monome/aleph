# Makefile for aleph-blackfin modules.
#
# CUSTOMIZING:
# 
# if you use a single source file, you can call it [yourname].c, 
# set the module_name variable to [yourname],
# and use this file without further modification.
#
# the last line of the module rule includes
# linker flags for external libraries.
#
# if you want to use multiple source files, you must edit these targets.

module_name = osc

all: aleph-$(module_name).ldr

include ../../core.mk

aleph-$(module_name): core_target $(module_name).c
	$(CC) $(CFLAGS) -c $(module_name).c -o $(module_name).o
	$(CC) $(LDFLAGS) -T aleph-$(module_name).lds \
	$(patsubst %.o, $(core_objdir)%.o, $(core_obj)) \
	$(module_name).o \
	-o aleph-$(module_name) \
	-lm -lbfdsp -lbffastfp

clean: core_clean
	rm aleph-$(module_name).ldr 
	rm aleph-$(module_name)
	rm $(module_name).o

.PHONY: clean