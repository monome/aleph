OBJ = op.o net.o main.o screen.o menu.o preset.o param.o key_handler.o redraw.o scene.o

CC = gcc
CFLAGS = -g -c
CFLAGS += -Wall -Wextra -D ARCH_LINUX=1
CFLAGS += -I../common/
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
