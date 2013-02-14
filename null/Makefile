CC = gcc
CFLAGS += -std=gnu99
CFLAGS += -g3
LDFLAGS = -lm
LDFLAGS += -lportaudio
LDFLAGS += -lncurses
LDFLAGS += -lm
# LIBS += -llo -ldl 
# LIBS += -ltinfo
COMMON = ../common
LIBFIXMATH = $(COMMON)/libfixmath
AUDIOLIB = ../audio
INC := -I$(COMMON) -I$(LIBFIXMATH) -I$(AUDIOLIB)

# # APP := null
OBJ  = main.o \
	audio.o \
	ui.o \
	$(LIBFIXMATH)/fix16.o  \
	$(LIBFIXMATH)/fract32.o \
	$(COMMON)/fract32_emu.o \
	$(COMMON)/simple_string.o \
	$(COMMON)/fix.o

MONO_FM_OBJ :=  mono_fm.o \
	$(AUDIOLIB)/env.o \
	$(AUDIOLIB)/filters.o \
	$(AUDIOLIB)/table.o

mono_fm : $(OBJ) $(MONO_FM_OBJ)
	echo $(INC)
	$(CC) $(CFLAGS) $(INC) -o $@ $^ $(LDFLAGS)

TAPE_OBJ :=  tape.o \
	$(AUDIOLIB)/filters.o \

tape : $(OBJ) $(TAPE_OBJ)
	$(CC) $(CFLAGS) $(INC) -o $@ $^ $(LDFLAGS)

%.o : %.c
	$(CC) $(CFLAGS) -c $(INC) -o $@ $<

clean :
	rm $(OBJ)
	rm *.o