# paths are relative to toplevel ASF
# avr32 core directory
ALEPH_AVR32 = ../../utils/avr32_blank
# aleph common directory
ALEPH_COMMON = ../../common

APP = blank
APP_DIR = ../../utils/avr32_blank


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

#include 
include $(ALEPH_AVR32)/core.mk

# List of C source files.
CSRCS += \
	$(APP_DIR)/src/jansson/dump.c \
	$(APP_DIR)/src/jansson/error.c \
	$(APP_DIR)/src/jansson/hashtable.c \
	$(APP_DIR)/src/jansson/load.c \
	$(APP_DIR)/src/jansson/memory.c \
	$(APP_DIR)/src/jansson/pack_unpack.c \
	$(APP_DIR)/src/jansson/strbuffer.c \
	$(APP_DIR)/src/jansson/strconv.c \
	$(APP_DIR)/src/jansson/utf.c \
	$(APP_DIR)/src/jansson/value.c

# CSRCS += \
#	$(APP_DIR)/src/app_lppr.c \
#	$(APP_DIR)/src/app_timers.c \
#	$(APP_DIR)/src/files.c \
#	$(APP_DIR)/src/handler.c \
#	$(APP_DIR)/src/ctl.c \
#	$(APP_DIR)/src/inputs.c	\
#	$(APP_DIR)/src/render.c

# List of assembler source files.
ASSRCS +=

# List of include paths.	
INC_PATH += \
	$(APP_DIR)	           \
	$(APP_DIR)/src \
	$(APP_DIR)/conf \
	$(APP_DIR)/src/jansson \

# Additional search paths for libraries.
LIB_PATH = 

# List of libraries to use during linking.
LIBS = 

# Path relative to top level directory pointing to a linker script.
# LINKER_SCRIPT = avr32/utils/linker_scripts/at32uc3a/0512/gcc/link_uc3a0512.lds
LINKER_SCRIPT = $(APP_DIR)/aleph-$(APP).lds

# AVR32 options for debugging. By default the common Makefile.in will
# add -g3.
DBGFLAGS =

# Application optimization used during compilation and linking:
# -O0, -O1, -O2, -O3 or -Os
# OPTIMIZATION = -Os
OPTIMIZATION = -O0
# OPTIMIZATION = -O1
# OPTIMIZATION = -O2
# OPTIMIZATION = -O3

# debug:
# OPTIMIZATION = -O0 -fno-inline

# Extra flags to use when archiving.
ARFLAGS = 

# Extra flags to use when assembling.
ASFLAGS = 

# Extra flags to use when compiling.
#debug:
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
LDFLAGS = -nostartfiles -Wl,-e,_trampoline

# for external heap:
# LDFLAGS += --gc-sections,--defsym,__heap_size__=0x00080000	

# for a bootloader:
# LDFLAGS = -nostartfiles
