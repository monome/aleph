srcdir = src/
objdir = obj/

src = main.c init.c isr.c util.c
obj = $(patsubst %.c, $(objdir)%.o, $(src))

INC += -I$(srcdir)

CROSS_COMPILE = bfin-elf-
CC = $(CROSS_COMPILE)gcc
LDR = $(CROSS_COMPILE)ldr
CPU = bf533
CFLAGS += -Wall -g -mcpu=$(CPU) $(INC)
CFLAGS += -00 # debug
# CFLAGS += -02

LDFLAGS += -mcpu=$(CPU)
# LDRFLAGS += --initcode $(core_objdir)init.o
LDRFLAGS += --bits 16 --dma 8
LDRFLAGS += --bmode spi_slave --port F --gpio 2
LDRFLAGS += --verbose

TARGET = bfin-dac-test

all: $(TARGET).ldr


%.ldr: %
	$(LDR) -T $(CPU) -c $(LDRFLAGS) $@ $<

$(TARGET): $(obj)
	$(CC) $(LDFLAGS) $(obj) -o $(TARGET) $(LFLAGS)

$(obj): $(patsubst $(objdir)%.o, $(srcdir)%.c, $@) \
#	echo $(obj)
#	echo $(src)
	$(CC) $(CFLAGS) $(INC) -c \
	$(patsubst $(objdir)%.o, $(srcdir)%.c, $@) \
	-o $@
clean:
	rm $(obj)
	rm $(TARGET)
	rm $(TARGET).ldr

.PHONY: clean
