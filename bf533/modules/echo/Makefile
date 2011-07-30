# OBJS=Initialize.o ISR.o main.o Process_data.o
# SRCS=$(SRCS:%.o=%.c)

SRC_CORE = $(wildcard ../../core/*.c)
OBJ_CORE  = $(SRC_CORE:.c=.o)

SRC_MOD = $(wildcard *.c)
OBJ_MOD  = $(SRC_MOD:.c=.o)

CROSS_COMPILE = bfin-elf-
CC = $(CROSS_COMPILE)gcc
LDR = $(CROSS_COMPILE)ldr

CPU = bf533
DEVICE = /dev/ttyUSB0
CFLAGS ?= -g
CFLAGS += -mcpu=$(CPU)
CFLAGS += -Wall
LDRFLAGS = --bits 16 --dma 8 --bmode para
INCLUDE = "../../core/"

APP_DIR = ../
APP = echo

all: $(APP)

$(APP): $(OBJ_CORE) $(OBJ_MOD)
	$(CC) $(CFLAGS) -I$(INCLUDE) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -c $< -o $@

%.ldr: %
	$(LDR) -T $(CPU) -c $(LDRFLAGS) $@ $^

load: $(APP).ldr	
	$(LDR) -l $< $(DEVICE)

clean:
	rm $(OBJ_CORE)
	rm $(OBJ_MOD)
	rm $(APP)

.PHONY: all clean load