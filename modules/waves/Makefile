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

all: $(ldr_name)

include ../../bfin_lib/bfin_lib.mk
CFLAGS += -D ARCH_BFIN=1
#CFLAGS += -g
#CFLAGS += --verbose

$(module_obj): %.o : %.c	
	$(CC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)

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