# ------------------------
# -- avr32_lib_config.mk

# boilerplate configuration makefile for aleph-avr32 applications.

# app developers: feel free to include this or copy/edit as you like,
# but beware of breaking things / bricking your unit..

# avr32 core directory, relative to top-level asf
ALEPH_AVR32 = ../
# aleph common directory
ALEPH_COMMON = ../../common
# application directory
APP_DIR = ../../apps/$(APP)

# Target CPU architecture: ap, ucr1, ucr2 or ucr3
ARCH = ucr2

# Target part: none, ap7xxx or uc3xxxxx
PART = uc3a0512

# Target device flash memory details (used by the avr32program programming
# tool: [cfi|internal]@address
FLASH = internal@0x80000000

# Clock source to use when programming; xtal, extclk or int
PROG_CLOCK = int

# Application target name. Given with suffix .a for library and .elf for a
# standalone application.
TARGET = aleph-$(APP).elf

# Path relative to top level directory pointing to a linker script.
# LINKER_SCRIPT = avr32/utils/linker_scripts/at32uc3a/0512/gcc/link_uc3a0512.lds
LINKER_SCRIPT = $(APP_DIR)/aleph-$(APP).lds

# AVR32 options for debugging. By default the common Makefile.in will
# add -g3.
DBGFLAGS =

# optimization level
OPTIMIZATION = -O3

# preprocessor definitions
CPPFLAGS = \
       -D BOARD=USER_BOARD -D ARCH_AVR32=1 -D UHD_ENABLE -DDEV_USART_BAUDRATE=$(BAUD)

# Extra flags to use when linking
#####
## NOTE:
# use this line instead if you want to use without the bootloader!
# linking with the trampoline on a bootloaded app is bad news.
# LDFLAGS = -nostartfiles -Wl,-e,_trampoline
LDFLAGS = -nostartfiles

# for external heap:
# LDFLAGS += --gc-sections,--defsym,__heap_size__=0x00080000
