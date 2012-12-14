OBJS=Initialization.o ISRs.o main.o
SRCS=$(SRCS:%.o=%.c)

CROSS_COMPILE = bfin-elf-
CC = $(CROSS_COMPILE)gcc
LDR = $(CROSS_COMPILE)ldr

CPU = bf533
DEVICE = /dev/ttyUSB0
CFLAGS ?= -g
CFLAGS += -mcpu=$(CPU)
CFLAGS += -Wall
LIBS = -lm
# LDRFLAGS = --bits 16 --dma 8 --bmode para
# use SPI boot mode, port F, pin 13
LDRFLAGS += --bits 16 --dma 8
#LDRFLAGS += --bmode spi_slave --port F --gpio 13
LDRFLAGS += --bmode spi_slave --port F --gpio 2
LDRFLAGS += --verbose

all: blink_533.ldr

%.ldr: %
	$(LDR) -T $(CPU) -c $(LDRFLAGS) $@ $^

load: blink_533.ldr	
	$(LDR) -l $< /dev/ttyUSB0

blink_533: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f blink_533 *.o *.i *.s
	rm -f blink_533.ldr

.PHONY: all clean load
