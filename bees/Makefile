OBJ = op.o net.o main.o ui.o

CC = gcc
CFLAGS = -g -c
# CFLAGS += -Wall
LDFLAGS = -lm -lncurses

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
