# Universal AVR32 Makefile for firmware without operating system.
# Wolfgang Wieser 07/2008.
# stolen and mutilated by emb, 2011

PART = uc3a0512
BOARD = EVK1100
CFLAGS = -g -O2 -Wall -ffunction-sections 
ASMFLAGS = -Wa,-g -Wa,-g3

LDFLAGS += -nostartfiles -Lsrc/SOFTWARE_FRAMEWORK/UTILS/LIBS/NEWLIB_ADDONS -Lsrc/SOFTWARE_FRAMEWORK/BOARDS 
LDFLAGS += -Wl,--gc-sections -Wl,-e,_trampoline
LDFLAGS += -Tsrc/SOFTWARE_FRAMEWORK/UTILS/LINKER_SCRIPTS/AT32UC3A/0512/GCC/link_uc3a0512.lds
LDFLAGS += -mpart=$(PART)
LDFLAGS += -Wl,--gc-sections --rodata-writable -Wl,--direct-data
LDFLAGS += -g
#LDFLAGS += -v

SRC = src/main.c src/init.c
OBJ = src/main.o src/init.o
INC += -I../common

# Atmel Software Framework
include asf.mk
SRC += $(ASF_SRC)
OBJ += $(ASF_OBJ)
INC += $(ASF_INC)

ELF = spi_boot_avr.elf

all: $(ELF)

wtf:
	@echo $(SRC)
	@echo "\n"
	@echo $(OBJ)
	@echo "\n"
	@echo $(INC)

run: $(ELF) program

clean:
	rm -f $(OBJ)

$(ELF): $(OBJ)
	avr32-gcc $(LDFLAGS) -mpart=$(PART) -o$(ELF) $(OBJ)

program: $(ELF)
	avr32program -c USB -pjtagicemkii program -finternal@0x80000000 -v -e --run -R -cint $(ELF)
reset:
	avr32program -c USB -pjtagicemkii reset -r
cont:
	avr32program -c USB -pjtagicemkii run
erase:
	avr32program -c USB -pjtagicemkii erase -finternal@0x80000000
chiperase:
	avr32program -c USB -pjtagicemkii chiperase

%.o : %.c
	avr32-gcc -c $(CFLAGS) $(INC) -DBOARD=$(BOARD) -mpart=$(PART) -o$@ $<

%.o : %.x	
	avr32-gcc -x assembler-with-cpp -c $(ASMFLAGS) $(INC) -DBOARD=$(BOARD) -mpart=$(PART) -o$@ $<

.PHONY: all run clean program reset cont erase