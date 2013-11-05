# compiler flags
CC = gcc
CFLAGS += -std=gnu99
CFLAGS += -g3
# CFLAGS += --verbose

#linker flags
LDFLAGS = -lm

# source directories
COMMON = ../../common
LIBFIXMATH = $(COMMON)/libfixmath

#include paths
INC := -I$(COMMON) -I$(LIBFIXMATH)

# common object list for all modules
APP_OBJ = tables.o \
	$(LIBFIXMATH)/fix16.o  \
	$(COMMON)/fix.o

$(APP_OBJ): %.o : %.c	
	$(CC) $(CFLAGS) $(INC) -c -o $@ $< $(LDFLAGS)

# mono module

tabletool : $(APP_OBJ)
	$(CC) $(CFLAGS) $(INC) -o $@ $^ $(LDFLAGS)

clean:
	rm $(APP_OBJ)