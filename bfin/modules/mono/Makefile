audiolib = ../../../audio

module_name = mono
module_obj = mono_fm.o \
	$(audiolib)/env.o \
	$(audiolib)/filters.o \
	$(audiolib)/table.o

all: aleph-$(module_name).ldr

include ../../core.mk

CFLAGS += -D ARCH_BFIN=1
#CFLAGS += --verbose

$(module_obj): %.o : %.c	
	$(CC) $(CFLAGS) -I $(audiolib) -c -o $@ $< $(LDFLAGS)

aleph-$(module_name): core_target $(module_obj)
	$(CC) $(LDFLAGS) -T aleph-$(module_name).lds \
	$(patsubst %.o, $(core_objdir)%.o, $(core_obj)) \
	$(module_obj) \
	-o aleph-$(module_name) \
	-lm -lbfdsp -lbffastfp

clean: core_clean
	rm $(module_obj)
	rm aleph-$(module_name).ldr 
	rm aleph-$(module_name)

.PHONY: clean