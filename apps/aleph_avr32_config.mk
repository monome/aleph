# ------------------------
# -- aleph_avr32_config.mk

# configure paths for libavr32, common code, app code, and 
# avr32 core directory, relative to top-level asf
LIB_AVR32 = ../
# aleph common directory
ALEPH_COMMON = ../../common
# aleph-specific avr32 code
ALEPH_AVR32 = ../../avr32
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
LINKER_SCRIPT = $(APP_DIR)/aleph-$(APP).lds

# AVR32 options for debugging. By default the common Makefile.in will
# add -g3.
DBGFLAGS =

# optimization level
OPTIMIZATION = -O3

# release/debug build
ifdef R
  ifeq ("$(origin R)", "command line")
    RELEASE = $(R)
  endif
endif
ifndef RELEASE
  RELEASE = 0
endif

$(info RELEASE: $(RELEASE))

# version define
include version.mk

# preprocessor definitions
CPPFLAGS = \
       -D BOARD=USER_BOARD -D ARCH_AVR32=1 -D UHD_ENABLE -DDEV_USART_BAUDRATE=$(BAUD) -D VERSIONSTRING='"$(version)"' -D MAJ=$(maj) -D MIN=$(min) -D REV=$(rev) -D RELEASEBUILD=$(RELEASE)

# Extra flags to use when linking
#####
## NOTE:
# use this line instead if you want to use without the bootloader!
# linking with the trampoline on a bootloaded app is bad news.
# LDFLAGS = -nostartfiles -Wl,-e,_trampoline
LDFLAGS = -nostartfiles
