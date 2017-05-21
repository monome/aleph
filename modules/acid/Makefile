
module_name = acid

audio = ../../dsp
bfin = ../../bfin_lib/src

include version.mk
version = $(maj).$(min).$(rev)
ldr_name = $(module_name)-$(version).ldr

module_obj = acid.o \
	noise_acid.o \
	params.o \
	$(audio)/conversion.o \
	$(audio_dir)/env_exp.o \
	$(audio_dir)/filter_1p.o \
	$(audio_dir)/filter_2p.o \
	$(audio_dir)/filter_svf.o \
	$(audio)/ricks_tricks.o \
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

sim_sourcefiles = ../../utils/bfin_sim/main.c \
	acid.c \
	noise_acid.c \
	params.c \
	$(audio)/conversion.c \
	$(audio_dir)/env_exp.c \
	$(audio_dir)/filter_1p.c \
	$(audio_dir)/filter_2p.c \
	$(audio_dir)/filter_svf.c \
	$(audio)/ricks_tricks.c \
	../../utils/bfin_sim/fract2float_conv.c \
	../../utils/bfin_sim/src/cv.c \
	../../utils/bfin_sim/fract_math.c \

sim_inc = -I ../../dsp \
	-I ../../utils/bfin_sim/src \
	-I ../../utils/bfin_sim/ \
	-I ../../utils/bfin_sim/src/libfixmath \
	-I ./ \
	-I ../../common\

sim_outfile = acid_jack

sim_flags = -ljack -llo -D ARCH_LINUX=1

sim:
	touch $(sim_outfile)
	rm ./$(sim_outfile)
	gcc -lm $(sim_sourcefiles) $(sim_flags) $(sim_inc) -o $(sim_outfile) -g
