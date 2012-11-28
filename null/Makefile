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
INC += -I$(COMMON) -I$(LIBFIXMATH)

APP = null
OBJ  = main.o \
	audio.o \
	ui.o \
	$(LIBFIXMATH)/fix16.o  \
	$(LIBFIXMATH)/fract32.o \
	$(COMMON)/fract32_emu.o \
	$(COMMON)/simple_string.o \
	$(COMMON)/fix.o
#OBJ += wire.o
#OBJ += fsine.o
#OBJ += sine.o
OBJ += mono.o
OBJ += env.o
OBJ += table.o
OBJ += filters.o

$(APP) : $(OBJ)
	$(CC) $(CFLAGS) $(INC) -o $@ $^ $(LDFLAGS)
#	chmod +x $(APP)

$(OBJ) : %.o : %.c
	$(CC) $(CFLAGS) -c $(INC) -o $@ $<

clean :
	rm $(OBJ)
	rm $(APP)