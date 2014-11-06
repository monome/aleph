
module_name = dsyn

audio = ../../dsp
bfin = ../../bfin_lib/src

include version.mk
version = $(maj).$(min).$(rev)
ldr_name = $(module_name)-$(version).ldr

module_obj = dsyn.o \
	noise_dsyn.o \
	params.o \
	$(audio)/conversion.o \
	$(audio_dir)/env_exp.o \
	$(audio_dir)/filter_1p.o \
	$(audio_dir)/filter_2p.o \
	$(audio_dir)/filter_svf.o \
	$(bfin_lib_srcdir)/libfixmath/fix32.o \
	$(bfin_lib_srcdir)/libfixmath/fix16.o \
	$(bfin_lib_srcdir)/libfixmath/fix16_sqrt.o

all: $(module_name).ldr

include ../../bfin_lib/bfin_lib.mk

CFLAGS += -D ARCH_BFIN=1
# CFLAGS += --verbose

desc_src = \
	$(bfin_lib_srcdir)desc.c \
	$(bfin_lib_srcdir)pickle.c \
	param_desc.c

desc:
	gcc $(desc_src) \
	$(INC) \
	-D NAME=\"$(module_name)\" \
	-o $(module_name)_desc_build

$(module_obj): %.o : %.c	
	$(CC) $(CFLAGS) -I $(audio_dir) -c -o $@ $< $(LDFLAGS)

$(module_name): bfin_lib_target $(module_obj)
	$(CC) $(LDFLAGS) -T $(module_name).lds \
	$(patsubst %.o, $(bfin_lib_objdir)%.o, $(bfin_lib_obj)) \
	$(module_obj) \
	-o $(module_name) \
	-lm -lbfdsp -lbffastfp

clean: bfin_lib_clean
	rm $(module_obj)
	rm $(module_name).ldr 
	rm $(module_name)

deploy: $(module_name).ldr
	make desc
	./$(module_name)_desc_build
	cp $(module_name).ldr $(module_name)-$(maj).$(min).$(rev).ldr


.PHONY: clean
