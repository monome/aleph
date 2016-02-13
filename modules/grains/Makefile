module_name = grains

audio = ../../dsp
bfin = ../../bfin_lib/src

include version.mk
version = $(maj).$(min).$(rev)
ldr_name = $(module_name)-$(version).ldr

module_obj = grains.o \
	$(audio)/buffer.o \
	$(audio)/ricks_tricks.o \
	$(audio)/echoTap.o \
	$(audio)/scrubTap.o \
	$(audio)/conversion.o \
	$(audio)/grain.o \
	$(audio)/filter_1p.o \
	$(audio)/filter_ramp.o \
	$(audio)/filter_svf.o \
	$(audio)/noise.o \
	$(audio)/pan.o \
	$(audio)/table.o \
	$(bfin)/libfixmath/fix32.o \
	$(bfin)/libfixmath/fix16.o \
	$(bfin)/libfixmath/fix16_sqrt.o


all: $(module_name).ldr

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
	rm grains.dsc
	rm grains_desc_build

deploy: $(module_name).ldr
	make desc
	./$(module_name)_desc_build
	cp $(module_name).ldr $(module_name)-$(maj).$(min).$(rev).ldr

.PHONY: clean
	deploy

sim_sourcefiles = ../../utils/bfin_sim/main.c \
	grains.c \
	$(audio)/buffer.c \
	$(audio)/ricks_tricks.c \
	$(audio)/echoTap.c \
	$(audio)/scrubTap.c \
	$(audio)/grain.c \
	$(audio)/filter_1p.c \
	$(audio)/filter_ramp.c \
	$(audio)/noise.c \
	$(audio)/pan.c \
	../../utils/bfin_sim/fract2float_conv.c \
	../../utils/bfin_sim/src/cv.c \
	../../utils/bfin_sim/fract_math.c \
	params.c

sim_inc = -I ../../dsp \
	-I ../../utils/bfin_sim/src \
	-I ../../utils/bfin_sim/ \
	-I ../../utils/bfin_sim/src/libfixmath \
	-I ./ \
	-I ../../common\

sim_outfile = grains_jack

sim_flags = -ljack -llo -D ARCH_LINUX=1

sim:
	touch $(sim_outfile)
	rm ./$(sim_outfile)
	gcc $(sim_sourcefiles) $(sim_flags) $(sim_inc) -o $(sim_outfile) -g
