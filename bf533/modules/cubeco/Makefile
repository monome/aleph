SRC = $(wildcard *.c)
SRC += $(wildcard ../common/*.c)
SRC += $(wildcard ../../core/*.c)
SRC += ../paramTables/gainTable.c
SRC += ../paramTables/ratioTable.c
OBJ  = $(SRC:.c=.o)

CROSS_COMPILE = bfin-elf-
CC = $(CROSS_COMPILE)gcc
LDR = $(CROSS_COMPILE)ldr

CPU = bf533
DEVICE = /dev/ttyUSB0
CFLAGS ?= -g
CFLAGS += -mcpu=$(CPU)
CFLAGS += -Wall 
# CFLAGS += --verbose
LDFLAGS = -lm 
# LDFLAGS += --verbose
LDRFLAGS = --bits 16 --dma 8 --bmode para
INCLUDE = -I"../../core/" 
INCLUDE += -I"../paramTables/"

APP_DIR = ../
APP = cubeco

all: $(APP)

$(APP): $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

%.ldr: %
	$(LDR) -T $(CPU) -c $(LDRFLAGS) $@ $^

load: $(APP).ldr	
	$(LDR) -l $< $(DEVICE)

clean:
	rm $(OBJ)
	rm $(APP)

.PHONY: all clean load