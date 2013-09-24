audiolib = ../../../audio
common = ../../../common

module_name = drumsyn
module_obj = drumsyn.o \
	params.o \
	$(audiolib)/conversion.o \
	$(audiolib)/env_exp.o \
	$(audiolib)/filter_1p.o \
	$(audiolib)/filter_2p.o \
	$(audiolib)/filter_svf.o \
	$(audiolib)/noise.o \
	$(common)/libfixmath/fix32.o \
	$(common)/libfixmath/fix16.o \
	$(common)/libfixmath/fix16_sqrt.o

all: aleph-$(module_name).ldr

include ../../core.mk

CFLAGS += -D ARCH_BFIN=1
CFLAGS += -g
# CFLAGS += -O0
# CFLAGS += -O3
# CFLAGS += --verbose

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
