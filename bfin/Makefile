CORE_OBJ=main.o init.o isr.o spi.o
CORE_SRC=$(CORE_OBJ:%.o=%.c)

CROSS_COMPILE = bfin-elf-
CC = $(CROSS_COMPILE)gcc
LDR = $(CROSS_COMPILE)ldr

CPU = bf533

CFLAGS ?= -g
CFLAGS += -mcpu=$(CPU)
CFLAGS += -Wall
LDFLAGS += -mcpu=$(CPU) -T aleph-bfin.lds
LDRFLAGS = --bits 16 --dma 8 --bmode para

all: echo.ldr

%.ldr: %
	$(LDR) -T $(CPU) -c $(LDRFLAGS) $@ $^

echo: $(CORE_OBJ) modules/echo/echo.o modules/echo/module_data.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

clean:
	rm -f *.o
	rm -f modules/echo/*.o

.PHONY: clean