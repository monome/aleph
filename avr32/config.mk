#
# Copyright (c) 2009-2010 Atmel Corporation. All rights reserved.
#
# edited by ezra buchla for monome-aleph project, 2012
#

# Path to top level ASF directory relative to this project directory.
PRJ_PATH = asf-3.1.3/

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
	avr32/boards/evk1100/led.c                         \
	avr32/drivers/flashc/flashc.c                      \
	avr32/drivers/gpio/gpio.c                          \
	avr32/drivers/intc/intc.c                          \
	avr32/drivers/pdca/pdca.c                          \
	avr32/drivers/pm/pm.c                              \
	avr32/drivers/pm/pm_conf_clocks.c                  \
	avr32/drivers/pm/power_clocks_lib.c                \
	avr32/drivers/spi/spi.c                            \
        avr32/drivers/tc/tc.c                          \
	avr32/drivers/usart/usart.c                        \
        avr32/services/delay/delay.c			   \
	avr32/services/fs/fat/fat.c                        \
	avr32/services/fs/fat/fat_unusual.c                \
	avr32/services/fs/fat/file.c                       \
	avr32/services/fs/fat/navigation.c                 \
	avr32/utils/debug/print_funcs.c                    \
	avr32/components/memory/sd_mmc/sd_mmc_spi/sd_mmc_spi.c \
	avr32/components/memory/sd_mmc/sd_mmc_spi/sd_mmc_spi_mem.c \
        common/services/clock/uc3a0_a1/sysclk.c            \
	common/services/storage/ctrl_access/ctrl_access.c \
	./src/bfin.c \
	./src/events.c \
	./src/encoders.c \
	./src/files.c \
	./src/font.c \
	./src/init.c	\
	./src/interrupts.c \
	./src/screen.c \
	./src/timers.c \
	./src/main.c

# List of assembler source files.
ASSRCS = \
       avr32/drivers/intc/exception.S                     \
       avr32/utils/startup/startup_uc3.S                  \
       avr32/utils/startup/trampoline_uc3.S

# List of include paths.
INC_PATH = \
       avr32/boards                                       \
       avr32/boards/evk1100                               \
	avr32/drivers/cpu/cycle_counter/                 \
       avr32/drivers/flashc                               \
       avr32/drivers/gpio                                 \
       avr32/drivers/intc                                 \
       avr32/drivers/pdca                                   \
       avr32/drivers/pm                                   \
       avr32/drivers/spi                                  \
       avr32/drivers/tc                                 \
       avr32/drivers/usart                                \
       avr32/services/delay                               \
       avr32/services/fs/fat                              \
       avr32/utils                                        \
       avr32/utils/debug                                  \
       avr32/utils/preprocessor                           \
       common/boards                                      \
	avr32/components/memory/sd_mmc/sd_mmc_spi \
       common/services/storage/ctrl_access                \
       common/services/clock                              \
       common/utils \
	. \
	../ \
	../conf

# Additional search paths for libraries.
LIB_PATH = 

# List of libraries to use during linking.
LIBS = 

# Path relative to top level directory pointing to a linker script.
LINKER_SCRIPT = avr32/utils/linker_scripts/at32uc3a/0512/gcc/link_uc3a0512.lds

# Additional options for debugging. By default the common Makefile.in will
# add -g3.
DBGFLAGS = 

# Application optimization used during compilation and linking:
# -O0, -O1, -O2, -O3 or -Os
OPTIMIZATION = -O0

# Extra flags to use when archiving.
ARFLAGS = 

# Extra flags to use when assembling.
ASFLAGS = 

# Extra flags to use when compiling.
CFLAGS = 

# Extra flags to use when preprocessing.
#
# Preprocessor symbol definitions
#   To add a definition use the format "-D name[=definition]".
#   To cancel a definition use the format "-U name".
#
# The most relevant symbols to define for the preprocessor are:
#   BOARD      Target board in use, see boards/board.h for a list.
#   EXT_BOARD  Optional extension board in use, see boards/board.h for a list.
CPPFLAGS = \
       -D BOARD=EVK1100

# Extra flags to use when linking
LDFLAGS = \
       -nostartfiles -Wl,-e,_trampoline
