module_name = dacs

audio = ../../dsp
bfin = ../../bfin_lib/src

include version.mk
version = $(maj).$(min).$(rev)
ldr_name = aleph-$(module_name)-$(version).ldr

module_obj = dacs.o \
	$(audio)/buffer.o \
	$(audio)/echoTap.o \
	$(audio)/conversion.o \
	$(audio)/pitch_shift.o \
	$(audio)/filter_1p.o \
	$(audio)/filter_ramp.o \
	$(audio)/filter_svf.o \
	$(audio)/noise.o \
	$(audio)/pan.o \
	$(audio)/table.o \
	$(bfin)/libfixmath/fix32.o \
	$(bfin)/libfixmath/fix16.o \
	$(bfin)/libfixmath/fix16_sqrt.o


all: aleph-$(module_name).ldr

include ../../bfin_lib/bfin_lib.mk

CFLAGS += -D ARCH_BFIN=1
# CFLAGS += --verbose

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
	-o aleph-$(module_name) \
	-lm -lbfdsp -lbffastfp

clean: bfin_lib_clean
	rm $(module_obj)
	rm aleph-$(module_name).ldr 
	rm aleph-$(module_name)
	rm aleph-dacs.dsc
	rm dacs_desc_build

deploy: aleph-$(module_name).ldr
	make desc
	./$(module_name)_desc_build
	cp aleph-$(module_name).ldr aleph-$(module_name)-$(maj).$(min).$(rev).ldr

.PHONY: clean
	deploy
