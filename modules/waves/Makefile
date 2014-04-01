
module_name = waves

include version.mk
version = $(maj).$(min).$(rev)
ldr_name = aleph-$(module_name)-$(version).ldr

module_obj = waves.o  \
	osc_waves.o \
	svf_waves.o \
	$(audio_dir)/conversion.o \
	$(audio_dir)/filter_1p.o \
	$(audio_dir)/interpolate.o \
	$(audio_dir)/table.o \
	$(bfin_lib_srcdir)/libfixmath/fix32.o \
	$(bfin_lib_srcdir)/libfixmath/fix16.o


all: aleph-$(module_name).ldr

include ../../bfin_lib/bfin_lib.mk
CFLAGS += -D ARCH_BFIN=1
#CFLAGS += -g
#CFLAGS += --verbose

include version.mk

desc_src = \
	$(bfin_lib_srcdir)desc.c \
	$(bfin_lib_srcdir)pickle.c \
	params.c

desc:
	gcc $(desc_src) \
	$(INC) \
	-D NAME=\"$(module_name)\" \
	-o $(module_name)_desc_build

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
	rm aleph-$(module_name).ldr 
	rm aleph-$(module_name)

deploy: aleph-$(module_name).ldr
	make desc
	./$(module_name)_desc_build
	cp aleph-$(module_name).ldr aleph-$(module_name)-$(maj).$(min).$(rev).ldr
	cp aleph-$(module_name).ldr ../../release/
	cp aleph-$(module_name)-$(maj).$(min).$(rev).ldr ../../release/
	cp aleph-$(module_name).dsc ../../release/

.PHONY: clean
	deploy
