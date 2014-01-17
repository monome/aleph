
module_name = waves

include version.mk
version = $(maj).$(min).$(rev)
ldr_name = aleph-$(module_name)-$(version).ldr

module_obj = waves.o  \
	params.o \
	$(audio_dir)/conversion.o \
	$(audio_dir)/filter_1p.o \
	$(audio_dir)/filter_svf.o \
	$(audio_dir)/interpolate.o \
	$(audio_dir)/osc.o \
	$(audio_dir)/table.o \
	$(bfin_lib_srcdir)/libfixmath/fix32.o \
	$(bfin_lib_srcdir)/libfixmath/fix16.o


all: aleph-$(module_name).ldr
	cp aleph-$(module_name).ldr aleph-$(module_name)-$(maj).$(min).$(rev).ldr

include ../../bfin_lib/bfin_lib.mk
CFLAGS += -D ARCH_BFIN=1
#CFLAGS += -g
#CFLAGS += --verbose

include version.mk

$(module_obj): %.o : %.c	
	$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)

aleph-$(module_name): bfin_lib_target $(module_obj)
	$(CC) $(LDFLAGS) -T aleph-$(module_name).lds \
	$(patsubst %.o, $(bfin_lib_objdir)%.o, $(bfin_lib_obj)) \
	$(module_obj) \
	-D MAJ=$(maj) -D MIN=$(min) -D REV=$(rev) \
	-o aleph-$(module_name) \
	-lm -lbfdsp -lbffastfp

clean: bfin_lib_clean
	rm $(module_obj)
	rm aleph-$(module_name)-$(maj).$(min).$(rev).ldr
	rm aleph-$(module_name).ldr 
	rm aleph-$(module_name)

.PHONY: clean