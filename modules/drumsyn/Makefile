include ../../bfin_lib/bfin_lib.mk

module_name = drumsyn
module_obj = drumsyn.o \
	params.o \
	$(audio)/conversion.o \
	$(audio_dir)/env_exp.o \
	$(audio_dir)/filter_1p.o \
	$(audio_dir)/filter_2p.o \
	$(audio_dir)/filter_svf.o \
	$(audio_dir)/noise.o \
	$(bfin_lib_srcdir)/libfixmath/fix32.o \
	$(bfin_lib_srcdir)/libfixmath/fix16.o \
	$(bfin_lib_srcdir)/libfixmath/fix16_sqrt.o

all: aleph-$(module_name).ldr

CFLAGS += -D ARCH_BFIN=1
CFLAGS += -g
# CFLAGS += -O0
# CFLAGS += -O3
# CFLAGS += --verbose

$(module_obj): %.o : %.c	
	$(CC) $(CFLAGS) -I $(audio_dir) -c -o $@ $< $(LDFLAGS)

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
