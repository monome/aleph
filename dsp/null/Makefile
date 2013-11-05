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
COMMON = ../../common
LIBFIXMATH = $(COMMON)/libfixmath
AUDIOLIB = ../
MODULES = ../../modules

#include paths
INC := -I$(COMMON) -I$(LIBFIXMATH) -I$(AUDIOLIB) -I./ -I../../../$(AUDIOLIB)

# common object list for all modules
APP_OBJ = main.o \
	audio.o \
	ui.o \
	$(LIBFIXMATH)/fix16.o  \
	$(LIBFIXMATH)/fix32.o \
	$(LIBFIXMATH)/fract32.o \
	$(COMMON)/fract32_emu.o \
	$(COMMON)/fix.o

$(APP_OBJ): %.o : %.c	
	$(CC) $(CFLAGS) $(INC) -c -o $@ $< $(LDFLAGS)

# mono module
MONO_OBJ =  $(MODULES)/mono/mono.o \
	$(AUDIOLIB)/env.o \
	$(AUDIOLIB)/filter_1p.o \
	$(AUDIOLIB)/table.o \
	$(AUDIOLIB)/conversion.o

$(MONO_OBJ): %.o : %.c	
	$(CC) $(CFLAGS) $(INC) -c -o $@ $< $(LDFLAGS)


MONO_INC = -I$(MODULES)/mono

mono : $(APP_OBJ) $(MONO_OBJ)
	echo $(MONO_INC)
	echo $(INC)
	$(CC) $(CFLAGS) $(INC) $(MONO_INC) -o $@ $^ $(LDFLAGS)

# FIXME: how to clean current module objects? hm
clean:
	rm $(APP_OBJ)
	rm $(AUDIOLIB)/*.o
	rm *.o 
