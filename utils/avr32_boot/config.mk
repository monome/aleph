#
# Copyright (c) 2009-2010 Atmel Corporation. All rights reserved.
#
# edited by ezra buchla for monome-aleph project, 2012
#

# Path to top level ASF directory relative to this project directory.
# moved this to the top level Makefile cause it needs to know too
# PRJ_PATH = ../../avr/asf-3.3.0/


# Application target name. Given with suffix .a for library and .elf for a
# standalone application.
TARGET = aleph-boot.elf

# these paths are relative to top-level ASF directory
ALEPH_AVR32 = ../../utils/avr32_boot
ALEPH_COMMON = ../../common

# Target CPU architecture: ap, ucr1, ucr2 or ucr3
ARCH = ucr2

# Target part: none, ap7xxx or uc3xxxxx
PART = uc3a0512

# Target device flash memory details (used by the avr32program programming
# tool: [cfi|internal]@address
FLASH = internal@0x80000000

# debug level
BUILD_DEBUG_LEVEL = 0

# Clock source to use when programming; xtal, extclk or int
PROG_CLOCK = int

# List of C source files.
CSRCS = \
	avr32/drivers/ebi/smc/smc.c \
	avr32/drivers/flashc/flashc.c                      \
	avr32/drivers/gpio/gpio.c                          \
	avr32/drivers/intc/intc.c                          \
	avr32/drivers/pdca/pdca.c                          \
	avr32/drivers/pm/pm.c                              \
	avr32/drivers/pm/pm_conf_clocks.c                  \
	avr32/drivers/pm/power_clocks_lib.c                \
	avr32/drivers/spi/spi.c                            \
        avr32/drivers/tc/tc.c                          \
	avr32/drivers/wdt/wdt.c                        \
	avr32/components/memory/sd_mmc/sd_mmc_spi/sd_mmc_spi.c \
	avr32/components/memory/sd_mmc/sd_mmc_spi/sd_mmc_spi_mem.c \
        common/services/clock/uc3a0_a1/sysclk.c            \
	common/services/storage/ctrl_access/ctrl_access.c \
	$(ALEPH_AVR32)/src/app_timers.c \
	$(ALEPH_AVR32)/src/bfin.c \
	$(ALEPH_AVR32)/src/events.c \
	$(ALEPH_AVR32)/src/encoders.c \
	$(ALEPH_AVR32)/src/files.c \
	$(ALEPH_AVR32)/src/filesystem.c \
	$(ALEPH_AVR32)/src/flash.c \
	$(ALEPH_AVR32)/src/font.c \
	$(ALEPH_AVR32)/src/init.c	\
	$(ALEPH_AVR32)/src/interrupts.c \
	$(ALEPH_AVR32)/src/key_handler.c \
	$(ALEPH_AVR32)/src/main.c \
	$(ALEPH_AVR32)/src/memory.c \
	$(ALEPH_AVR32)/src/menu.c \
	$(ALEPH_AVR32)/src/parse_hex.c \
	$(ALEPH_AVR32)/src/redraw.c \
	$(ALEPH_AVR32)/src/screen.c \
	$(ALEPH_AVR32)/src/simple_string.c \
	$(ALEPH_AVR32)/src/switches.c \
	$(ALEPH_AVR32)/src/timers.c \
	$(ALEPH_AVR32)/src/util.c \
	$(ALEPH_AVR32)/src/watchdog.c \
	$(ALEPH_AVR32)/src/fat_io_lib/fat_access.c \
	$(ALEPH_AVR32)/src/fat_io_lib/fat_cache.c \
	$(ALEPH_AVR32)/src/fat_io_lib/fat_filelib.c \
	$(ALEPH_AVR32)/src/fat_io_lib/fat_format.c \
	$(ALEPH_AVR32)/src/fat_io_lib/fat_misc.c \
	$(ALEPH_AVR32)/src/fat_io_lib/fat_string.c \
	$(ALEPH_AVR32)/src/fat_io_lib/fat_table.c \
	$(ALEPH_AVR32)/src/fat_io_lib/fat_write.c \

# List of assembler source files.
ASSRCS = \
       avr32/drivers/intc/exception.S                     \
       avr32/utils/startup/startup_uc3.S                     \
       avr32/utils/startup/trampoline_uc3.S                     
#	$(ALEPH_AVR32)/startup/startup.S \
#	$(ALEPH_AVR32)/startup/trampoline.S

# List of include paths.
INC_PATH = \
       avr32/boards                                       \
       avr32/boards/user_board                               \
	avr32/components/memory/sdram			 \
	avr32/drivers/cpu/cycle_counter/                 \
	avr32/drivers/ebi/smc/                            \
	avr32/drivers/eic/                            \
       avr32/drivers/flashc                               \
       avr32/drivers/gpio                                 \
       avr32/drivers/intc                                 \
       avr32/drivers/pdca                                   \
       avr32/drivers/pm                                   \
       avr32/drivers/spi                                  \
       avr32/drivers/tc                                 \
       avr32/drivers/twi                                 \
       avr32/drivers/usart                                \
       avr32/drivers/wdt                                \
       avr32/services/delay                               \
       avr32/services/fs/fat                              \
       avr32/utils                                        \
       avr32/utils/debug                                  \
       avr32/utils/preprocessor                           \
       common/boards                                      \
	avr32/components/memory/sd_mmc/sd_mmc_spi         \
       common/services/storage/ctrl_access                \
       common/services/clock                              \
	common/services/delay                             \
       common/utils                \
	$(ALEPH_AVR32)	           \
	$(ALEPH_AVR32)/src	   \
	$(ALEPH_AVR32)/src/fat_io_lib \
	$(ALEPH_AVR32)/src/libfixmath \
	$(ALEPH_AVR32)/conf	   \
	$(ALEPH_COMMON)	           

# Additional search paths for libraries.
LIB_PATH = 

# List of libraries to use during linking.
LIBS = 

# Path relative to top level directory pointing to a linker script.
# LINKER_SCRIPT = avr32/utils/linker_scripts/at32uc3a/0512/gcc/link_uc3a0512.lds
LINKER_SCRIPT = $(ALEPH_AVR32)/aleph-boot.lds

# Additional options for debugging. By default the common Makefile.in will
# add -g3.
DBGFLAGS = -g3

# Application optimization used during compilation and linking:
# -O0, -O1, -O2, -O3 or -Os
# bootloader just needs to be small
OPTIMIZATION = -Os

# Extra flags to use when archiving.
ARFLAGS = 

# Extra flags to use when assembling.
ASFLAGS = 

# Extra flags to use when compiling.
#CFLAGS += -g3
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
CPPFLAGS = \
       -D BOARD=USER_BOARD -D ARCH_AVR32=1 -D UHD_ENABLE

# Extra flags to use when linking
# LDFLAGS = -Wl,--gc-sections,-e,_trampoline -Wl,--defsym,__heap_size__=0x00080000 -nostartfile	
# programmer doesn't work without trampoline
# (can't unlock?)
#	LDFLAGS = -nostartfiles -Wl,-e
LDFLAGS = -nostartfiles -Wl,-e,_trampoline