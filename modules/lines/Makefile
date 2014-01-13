module_name = lines

audio = ../../dsp
bfin = ../../bfin_lib/src

include version.mk
version = $(maj).$(min).$(rev)
ldr_name = aleph-$(module_name)-$(version).ldr

module_obj = lines.o \
	params.o \
	$(audio)/buffer.o \
	$(audio)/conversion.o \
	$(audio)/delay.o \
	$(audio)/filter_svf.o \
	$(audio)/noise.o \
	$(audio)/table.o \
	$(bfin)/libfixmath/fix32.o \
	$(bfin)/libfixmath/fix16.o \
	$(bfin)/libfixmath/fix16_sqrt.o

all: $(ldr_name)

include ../../bfin_lib/bfin_lib.mk

CFLAGS += -D ARCH_BFIN=1

$(module_obj): %.o : %.c	
	$(CC) $(CFLAGS) -I $(audio) -c -o $@ $< $(LDFLAGS)

$(ldr_name): bfin_lib_target $(module_obj)
	echo $(ldr_name)
	$(CC) $(LDFLAGS) -T aleph-$(module_name).lds \
	$(patsubst %.o, $(bfin_lib_objdir)%.o, $(bfin_lib_obj)) \
	$(module_obj) \
	-o $(ldr_name) \
	-lm -lbfdsp -lbffastfp

clean: bfin_lib_clean
	rm $(module_obj)
	rm $(ldr_name)

.PHONY: clean