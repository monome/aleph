COMMON = ../common
LIBFIX = $(COMMON)/libfixmath

OBJ = op.o net.o main.o screen.o menu.o preset.o param.o key_handler.o redraw.o scene.o
OBJ += $(LIBFIX)/fix16.o
OBJ += $(COMMON)/fix.c

CC = gcc
CFLAGS = -g -c
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
	rm *.o
	rm $(WHAT)

.PHONY : clean all
