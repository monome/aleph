# compiler flags
CC = gcc
CFLAGS += -std=gnu99
CFLAGS += -g3
CFLAGS += -D ARCH_LINUX=1
#linker flags
LDFLAGS = -lm
LDFLAGS += -lportaudio
LDFLAGS += -lncurses
LDFLAGS += -lm
# source directories
COMMON = ../common
LIBFIXMATH = $(COMMON)/libfixmath
AUDIOLIB = ../audio
# FIXME: use this... stupid to maintain 2 copies of each src
MODULES = ../bfin/modules
#include paths
INC := -I$(COMMON) -I$(LIBFIXMATH) -I$(AUDIOLIB)

APP_OBJ  = main.o \
	audio.o \
	ui.o \
	$(LIBFIXMATH)/fix16.o  \
	$(LIBFIXMATH)/fix32.o \
	$(LIBFIXMATH)/fract32.o \
	$(COMMON)/fract32_emu.o \
	$(COMMON)/simple_string.o \
	$(COMMON)/fix.o

MONO_FM_OBJ :=  mono_fm.o \
	$(AUDIOLIB)/env.o \
	$(AUDIOLIB)/filters.o \
	$(AUDIOLIB)/table.o

mono_fm : $(APP_OBJ) $(MONO_FM_OBJ)
	echo $(INC)
	$(CC) $(CFLAGS) $(INC) -o $@ $^ $(LDFLAGS)

TAPE_OBJ :=  tape.o \
	$(AUDIOLIB)/filters.o \
	$(AUDIOLIB)/buffer.o \

tape : $(APP_OBJ) $(TAPE_OBJ)
	$(CC) $(CFLAGS) $(INC) -o $@ $^ $(LDFLAGS)

%.o : %.c
	$(CC) $(CFLAGS) -c $(INC) -o $@ $<

# FIXME: how to clean current module objects as well
clean:
	rm $(APP_OBJ)
	rm $(AUDIOLIB)/*.o
	rm *.o 