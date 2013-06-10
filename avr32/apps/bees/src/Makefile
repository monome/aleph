COMMON = ../common
LIBFIX = $(COMMON)/libfixmath

INC = -I$(COMMON) -I$(LIBFIX) -I../ -I./

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
	memory.o \
	ops/op_add.o \
	ops/op_enc.o \
	ops/op_gate.o \
	ops/op_mul.o \
	ops/op_sw.o

OBJ += $(LIBFIX)/fix16.o
OBJ += $(COMMON)/fix.o
OBJ += $(COMMON)/font.o
OBJ += $(COMMON)/simple_string.o

CC = gcc
CFLAGS = -g3 -c
CFLAGS += -Wall -Wextra -D ARCH_LINUX=1
CFLAGS += $(INC)
LDFLAGS = -lm -lncurses #-lportaudio #-lasound

# test:
# CFLAGS += --verbose
# LDFLAGS += --verbose

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
