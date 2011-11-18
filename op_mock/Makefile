OBJ = ctl_op.o ctl_net.o main.o

CC = gcc
CFLAGS = -g -c
LDFLAGS = -lm

WHAT = bees

all : $(WHAT)

$(WHAT) : $(OBJ)
	$(CC) $(LDFLAGS) -o$@ $^

%.o : %.c
	$(CC) $(CFLAGS) $(INC) -o$@ $<

clean :
	rm *.o
	rm $(WHAT)

.PHONY : clean all