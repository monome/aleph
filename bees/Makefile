COMMON = ../common
LIBFIX = $(COMMON)/libfixmath

OBJ = op.o net.o \
	main.o \
	screen.o \
	menu.o \
	preset.o \
	param.o \
	key_handler.o \
	redraw.o \
	scene.o \
	files.o \
	memory.o 
OBJ += $(LIBFIX)/fix16.o
OBJ += $(COMMON)/fix.o
OBJ += $(COMMON)/font.o

CC = gcc
CFLAGS = -g3 -c
CFLAGS += -Wall -Wextra -D ARCH_LINUX=1
CFLAGS += -I$(COMMON) -I$(LIBFIX)
LDFLAGS = -lm -lncurses #-lportaudio #-lasound

WHAT = bees

all : $(WHAT)

$(WHAT) : $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o : %.c
	$(CC) $(CFLAGS) $(INC) -o$@ $<

clean :
	rm $(OBJ)
	rm $(WHAT)

.PHONY : clean all
