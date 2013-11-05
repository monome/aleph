audiolib = ../../dsp
common = ../../common

module_name = svfnoise
module_obj = svfnoise.o  \
	$(audiolib)/filter_svf.o \
	$(audiolib)/table.o \
	$(audiolib)/noise.o \
	$(common)/libfixmath/fix32.o \
	$(common)/libfixmath/fix16.o \
	$(common)/libfixmath/fix16_sqrt.o

all: aleph-$(module_name).ldr

include ../../bfin_lib/bfin_lib.mk

CFLAGS += -D ARCH_BFIN=1
#CFLAGS += --verbose

$(module_obj): %.o : %.c	
	$(CC) $(CFLAGS) -I $(audiolib) -c -o $@ $< $(LDFLAGS)

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

.PHONY: clean
