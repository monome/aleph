TARGET = dsp-block-test

OBJ_DIR=obj
INC += -I../common

_OBJ =	audio.o \
	dma.o \
	isr.o \
	main.o \
	module.o \
	osc.o \
	serial.o

OBJ = $(patsubst %,$(OBJ_DIR)/%,$(_OBJ))

all: $(TARGET).ldr

CROSS_COMPILE = bfin-elf-
CC = $(CROSS_COMPILE)gcc
LDR = $(CROSS_COMPILE)ldr
CPU = bf533
CFLAGS += -Wall -mcpu=$(CPU) $(INC)
CFLAGS += -O3
# CFLAGS += --verbose 

LDFLAGS += -mcpu=$(CPU)
LDRFLAGS += --bits 16 --dma 8
LDRFLAGS += --bmode spi_slave --port F --gpio 2
# LDRFLAGS += --verbose

LIBS += -lm -lbfdsp -lbffastfp


$(OBJ_DIR)/%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

%.ldr: %
	$(LDR) -T $(CPU) -c $(LDRFLAGS) $@ $<

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) -T $(TARGET).lds \
	-o $(TARGET) $^ $(LIBS)

clean:
	rm -f $(OBJ_DIR)/*.o
	rm $(TARGET).ldr
	rm $(TARGET)

.PHONY: clean
