# compiler flags
CC = gcc
CFLAGS += -std=gnu99
CFLAGS += -g3
CFLAGS += -D ARCH_LINUX=1
# CFLAGS += --verbose

#linker flags
LDFLAGS = -lm
LDFLAGS += -lportaudio
LDFLAGS += -lncurses
LDFLAGS += -lm

# source directories
COMMON = ../common
LIBFIXMATH = $(COMMON)/libfixmath
AUDIOLIB = ../audio
MODULES = ../bfin/modules

#include paths
INC := -I$(COMMON) -I$(LIBFIXMATH) -I$(AUDIOLIB) -I./ -I../../../$(AUDIOLIB)

# common object list for all modules
APP_OBJ  = main.o \
	audio.o \
	ui.o \
	$(LIBFIXMATH)/fix16.o  \
	$(LIBFIXMATH)/fix32.o \
	$(LIBFIXMATH)/fract32.o \
	$(COMMON)/fract32_emu.o \
	$(COMMON)/simple_string.o \
	$(COMMON)/fix.o

# mono_fm module
MONO_FM_OBJ :=  $(MODULES)/mono/mono_fm.o \
	$(AUDIOLIB)/env.o \
	$(AUDIOLIB)/filter_1p.o \
	$(AUDIOLIB)/table.o \
	$(AUDIOLIB)/conversion.o

mono_fm : $(APP_OBJ) $(MONO_FM_OBJ)
	echo $(INC)
	$(CC) $(CFLAGS) $(INC) -o $@ $^ $(LDFLAGS)

# iotest module
IOTEST_OBJ :=  $(MODULES)/iotest/iotest.o \
	$(AUDIOLIB)/env.o \
	$(AUDIOLIB)/filter_1p.o \
	$(AUDIOLIB)/table.o \
	$(AUDIOLIB)/conversion.o

iotest : $(APP_OBJ) $(IOTEST_OBJ)
	echo $(INC)
	$(CC) $(CFLAGS) $(INC) -o $@ $^ $(LDFLAGS)

# tape module
TAPE_OBJ :=  $(MODULES)/tape/tape.o \
	$(AUDIOLIB)/filter_1pole.o \
	$(AUDIOLIB)/buffer.o \

tape : $(APP_OBJ) $(TAPE_OBJ)
	$(CC) $(CFLAGS) $(INC) -o $@ $^ $(LDFLAGS)

# svfnoise module
SVF_OBJ :=  $(MODULES)/svfnoise/svfnoise.o \
	$(AUDIOLIB)/filter_svf.o \
	$(AUDIOLIB)/sin_fr32.o \
	$(LIBFIXMATH)/fix16_sqrt.o \
	$(AUDIOLIB)/noise.o \

svf : $(APP_OBJ) $(SVF_OBJ)
	$(CC) $(CFLAGS) $(INC) -o $@ $^ $(LDFLAGS)

%.o : %.c
	$(CC) $(CFLAGS) -c $(INC) -o $@ $<

# FIXME: how to clean current module objects? hm
clean:
	rm $(APP_OBJ)
	rm $(AUDIOLIB)/*.o
	rm *.o 