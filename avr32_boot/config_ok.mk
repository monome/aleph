#
# Copyright (c) 2009-2010 Atmel Corporation. All rights reserved.
#
# edited by ezra buchla for monome-aleph project, 2012
#

# Path to top level ASF directory relative to this project directory.
# moved this to the top level Makefile cause it needs to know too
# PRJ_PATH = ../../avr/asf-3.3.0/

#ALEPH_AVR32 = ../../aleph/avr32
#ALEPH_COMMON = ../../aleph/common
#ALEPH_BEES = ../../aleph/bees

ALEPH_AVR32 = ../../avr32

# Target CPU architecture: ap, ucr1, ucr2 or ucr3
ARCH = ucr2

# Target part: none, ap7xxx or uc3xxxxx
PART = uc3a0512

# Target device flash memory details (used by the avr32program programming
# tool: [cfi|internal]@address
FLASH = internal@0x80000000

# Clock source to use when programming; xtal, extclk or int
PROG_CLOCK = int

# Applicat	ion target name. Given with suffix .a for library and .elf for a
# standalone application.
TARGET = aleph.elf

# List of C source files.
CSRCS = \
	avr32/drivers/gpio/gpio.c                          \
	avr32/drivers/intc/intc.c                          \
	avr32/drivers/pm/pm.c                          \
       avr32/drivers/ebi/smc/smc.c                        \
       avr32/drivers/flashc/flashc.c                      \
	avr32/drivers/usart/usart.c                        \
	avr32/utils/debug/print_funcs.c                    \
        common/services/clock/uc3a0_a1/sysclk.c            \
	$(ALEPH_AVR32)/src/main.c

# List of assembler source files.
ASSRCS = \
       avr32/drivers/intc/exception.S                     \
	$(ALEPH_AVR32)/startup/startup.S \
	$(ALEPH_AVR32)/startup/trampoline.S

# List of include paths.
INC_PATH = \
       avr32/boards                                       \
       avr32/boards/user_board                               \
       avr32/drivers/ebi/smc                              \
       avr32/drivers/flashc                               \
       avr32/drivers/gpio                                 \
       avr32/drivers/intc                                 \
       avr32/drivers/pm                                   \
       avr32/drivers/usart                                \
       avr32/utils                                        \
       avr32/utils/debug                                  \
       avr32/utils/preprocessor                           \
       common/boards                                      \
       common/services/clock                              \
       common/utils         \
	$(ALEPH_AVR32)	\
	$(ALEPH_AVR32)/src	\
	$(ALEPH_AVR32)/conf	\
	$(ALEPH_AVR32)/startup	\

# Additional search paths for libraries.
LIB_PATH = 

# List of libraries to use during linking.
LIBS = 

# Path relative to top level directory pointing to a linker script.
# LINKER_SCRIPT = avr32/utils/linker_scripts/at32uc3a/0512/gcc/link_uc3a0512.lds
LINKER_SCRIPT = $(ALEPH_AVR32)/aleph.lds

# Additional options for debugging. By default the common Makefile.in will
# add -g3.
DBGFLAGS = 

# Application optimization used during compilation and linking:
# -O0, -O1, -O2, -O3 or -Os
 OPTIMIZATION = -O2
# OPTIMIZATION = -O3
# OPTIMIZATION = -O0 -fno-inline

# Extra flags to use when archiving.
ARFLAGS = 

# Extra flags to use when assembling.
ASFLAGS = 

# Extra flags to use when compiling.
CFLAGS += -g3
#CFLAGS += --verbose

# Extra flags to use when preprocessing.
#
# Preprocessor symbol definitions
#   To add a definition use the format "-D name[=definition]".
#   To cancel a definition use the format "-U name".
#
# The most relevant symbols to define for the preprocessor are:
#   BOARD      Target board in use, see boards/board.h for a list.
#   EXT_BOARD  Optional extension board in use, see boards/board.h for a list.
CPPFLAGS = -D BOARD=USER_BOARD -D ARCH_AVR32=1

# Extra flags to use when linking
#LDFLAGS = -nostartfiles -Wl,-e,_trampoline
LDFLAGS = -Wl,--gc-sections,-e,_trampoline -Wl,--defsym,__heap_size__=0x00080000 -nostartfiles