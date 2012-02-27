OBJ = op.o ctlnet.o main.o ui.o

CC = gcc
CFLAGS = -g -c
# CFLAGS += -Wall
LDFLAGS = -lm

WHAT = bees

all : $(WHAT)

$(WHAT) : $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

%.o : %.c
	$(CC) $(CFLAGS) $(INC) -o$@ $<

clean :
	rm *.o
	rm $(WHAT)

.PHONY : clean all