# Makefile for Aleph.  List of available make goals:
# ==========================================================
# all                     Default target, builds the project
# clean                   Clean up the project
# rebuild                 Rebuild the project
# ==========================================================

# Path to top level ASF directory relative to this project directory.
# our version-controlled dir
ASF_PATH = ../asf-3.7.3

MAKEFILE_PATH = ./ # current directory

# Core and common Aleph translation units, app
# avr32 core directory
ALEPH_AVR32 = ../avr32_test/
# aleph common directory
ALEPH_COMMON = ../common
# app
# ALEPH_APP = ../avr32_test/

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
TARGET = aleph-test.elf

# Core translation units: drivers, I/O, Aleph source etc.
CSRCS += \
	avr32/drivers/ebi/smc/smc.c \
	avr32/drivers/flashc/flashc.c \
	avr32/drivers/gpio/gpio.c \
	avr32/drivers/intc/intc.c \
	avr32/drivers/pdca/pdca.c \
	avr32/drivers/pm/pm.c \
	avr32/drivers/pm/pm_conf_clocks.c \
	avr32/drivers/pm/power_clocks_lib.c \
	avr32/drivers/spi/spi.c \
    avr32/drivers/tc/tc.c \
	avr32/drivers/usart/usart.c \
	avr32/utils/debug/print_funcs.c \
	avr32/components/memory/sd_mmc/sd_mmc_spi/sd_mmc_spi.c \
	avr32/components/memory/sd_mmc/sd_mmc_spi/sd_mmc_spi_mem.c \
	avr32/drivers/usbb/usbb_host.c \
    common/services/clock/uc3a0_a1/sysclk.c \
	common/services/storage/ctrl_access/ctrl_access.c \
    common/services/usb/uhc/uhc.c \
	$(ALEPH_AVR32)/src/adc.c \
	$(ALEPH_AVR32)/src/app.c \
	$(ALEPH_AVR32)/src/bfin.c \
	$(ALEPH_AVR32)/src/control.c \
	$(ALEPH_AVR32)/src/events.c \
	$(ALEPH_AVR32)/src/encoders.c \
	$(ALEPH_AVR32)/src/filesystem.c \
	$(ALEPH_AVR32)/src/flash.c \
	$(ALEPH_AVR32)/src/font.c \
	$(ALEPH_AVR32)/src/init.c	\
	$(ALEPH_AVR32)/src/interrupts.c \
	$(ALEPH_AVR32)/src/memory.c \
	$(ALEPH_AVR32)/src/monome.c \
	$(ALEPH_AVR32)/src/screen.c \
	$(ALEPH_AVR32)/src/simple_string.c \
	$(ALEPH_AVR32)/src/switches.c \
	$(ALEPH_AVR32)/src/sys_timers.c \
	$(ALEPH_AVR32)/src/timers.c \
	$(ALEPH_AVR32)/src/main.c \
	$(ALEPH_AVR32)/src/usb.c \
	$(ALEPH_AVR32)/src/usb/gamepad/hid_gamepad.c \
	$(ALEPH_AVR32)/src/usb/gamepad/uhi_hid_gamepad.c \
	$(ALEPH_AVR32)/src/usb/ftdi/uhi_ftdi.c \
	$(ALEPH_AVR32)/src/usb/ftdi/ftdi.c \
	$(ALEPH_AVR32)/src/usb/midi/uhi_midi.c \
	$(ALEPH_AVR32)/src/usb/midi/midi.c \
	$(ALEPH_AVR32)/src/usb/mouse/hid_mouse.c \
	$(ALEPH_AVR32)/src/usb/mouse/uhi_hid_mouse.c \
	$(ALEPH_AVR32)/src/fat_io_lib/fat_access.c \
	$(ALEPH_AVR32)/src/fat_io_lib/fat_cache.c \
	$(ALEPH_AVR32)/src/fat_io_lib/fat_filelib.c \
	$(ALEPH_AVR32)/src/fat_io_lib/fat_format.c \
	$(ALEPH_AVR32)/src/fat_io_lib/fat_misc.c \
	$(ALEPH_AVR32)/src/fat_io_lib/fat_string.c \
	$(ALEPH_AVR32)/src/fat_io_lib/fat_table.c \
	$(ALEPH_AVR32)/src/fat_io_lib/fat_write.c \
	$(ALEPH_COMMON)/fix.c \
	$(ALEPH_COMMON)/libfixmath/fix16.c

# test-specific
# CSRCS += \
#	$(ALEPH_APP)/src/app_lppr.c \
#	$(ALEPH_APP)/src/app_timers.c \
#	$(ALEPH_APP)/src/files.c \
#	$(ALEPH_APP)/src/handler.c \
#	$(ALEPH_APP)/src/ctl.c \
#	$(ALEPH_APP)/src/render.c

# List of assembler source files.
ASSRCS += \
    avr32/drivers/intc/exception.S \
    avr32/utils/startup/startup_uc3.S \
    avr32/utils/startup/trampoline_uc3.S                     
#	$(ALEPH_AVR32)/startup/startup.S \
#	$(ALEPH_AVR32)/startup/trampoline.S

# List of include paths.
INC_PATH += \
    avr32/boards \
    avr32/boards/user_board \
	avr32/components/memory/sdram \
	avr32/drivers/cpu/cycle_counter \
	avr32/drivers/ebi/smc \
	avr32/drivers/ei \
    avr32/drivers/flashc \
    avr32/drivers/gpio \
    avr32/drivers/intc \
    avr32/drivers/pdca \
    avr32/drivers/pm \
    avr32/drivers/spi \
    avr32/drivers/tc \
    avr32/drivers/twi \
    avr32/drivers/usart \
    avr32/drivers/usbb \
    avr32/services/delay \
    avr32/services/fs/fat \
    avr32/utils \
    avr32/utils/debug \
    avr32/utils/preprocessor \
    common/boards \
	avr32/components/memory/sd_mmc/sd_mmc_spi \
    common/services/storage/ctrl_access \
    common/services/clock \
	common/services/delay \
    common/services/usb \
    common/services/usb/class/hid \
    common/services/usb/uhc \
    common/utils \
	$(ALEPH_AVR32) \
	$(ALEPH_AVR32)/src \
	$(ALEPH_AVR32)/conf \
	$(ALEPH_AVR32)/src/fat_io_lib \
	$(ALEPH_AVR32)/src/usb \
	$(ALEPH_AVR32)/src/usb/ftdi \
	$(ALEPH_AVR32)/src/usb/gamepad \
	$(ALEPH_AVR32)/src/usb/midi \
	$(ALEPH_AVR32)/src/usb/mouse \
	$(ALEPH_COMMON) \
	$(ALEPH_COMMON)/libfixmath
#	$(ALEPH_APP) \
#	$(ALEPH_APP)/src

# Additional search paths for libraries.
LIB_PATH = 

# List of libraries to use during linking.
LIBS = 

# Path relative to top level directory pointing to a linker script.
LINKER_SCRIPT = $(ALEPH_AVR32)/aleph-test.lds

# AVR32 options for debugging. By default -g3 added.
DBGFLAGS =

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

# beginning of organizing target files
# Look for source files relative to the top-level source directory
VPATH           := $(ASF_PATH)

# Output project name (target name minus suffix) and target type
target          := $(TARGET)
project         := $(basename $(target))
target_type     := elf

# Aleph always Linux
os_type         := Linux
os              := Linux

CROSS           ?= avr32-
AR              := $(CROSS)ar
AS              := $(CROSS)as
CC              := $(CROSS)gcc
CPP             := $(CROSS)gcc -E
CXX             := $(CROSS)g++
LD              := $(CROSS)gcc
NM              := $(CROSS)nm
OBJCOPY         := $(CROSS)objcopy
OBJDUMP         := $(CROSS)objdump
SIZE            := $(CROSS)size

PROGRAM         := avr32program
GDBPROXY        := avr32gdbproxy
BATCHISP        := batchisp
ispflags-y      := -device at32$(PART) -hardware usb -operation

# Don't use make's built-in rules and variables, entering dir etc.
MAKEFLAGS       += -rR --no-print-directory

# Function for reversing the order of a list
reverse = $(if $(1),$(call reverse,$(wordlist 2,$(words $(1)),$(1)))) $(firstword $(1))

arflags-gnu-y           := $(ARFLAGS)
asflags-gnu-y           := $(ASFLAGS)
cflags-gnu-y            := $(CFLAGS)
cppflags-gnu-y          := $(CPPFLAGS)
cpuflags-gnu-y          :=
dbgflags-gnu-y          := $(DBGFLAGS)
libflags-gnu-y          := $(foreach LIB,$(LIBS),-l$(LIB))
ldflags-gnu-y           := $(LDFLAGS)
clean-files             :=
clean-dirs              :=
clean-files             += $(wildcard $(target) $(project).map)
clean-files             += $(wildcard $(project).hex $(project).bin)
clean-files             += $(wildcard $(project).lss $(project).sym)
clean-files             += $(wildcard $(build))

# Use pipes instead of temporary files for communication between processes
cflags-gnu-y    += -pipe
asflags-gnu-y   += -pipe
ldflags-gnu-y   += -pipe
arflags-gnu-y   += rcs # Archiver flags.

# Always enable warnings. And be very careful about implicit declarations.
cflags-gnu-y    += -Wall -Wstrict-prototypes -Wmissing-prototypes
cflags-gnu-y    += -Werror-implicit-function-declaration

# IAR doesn't allow arithmetic on void pointers, so warn about that.
cflags-gnu-y    += -Wpointer-arith

# Preprocessor flags.
cppflags-gnu-y  += $(foreach INC,$(addprefix $(ASF_PATH)/,$(INC_PATH)),-I$(INC))
asflags-gnu-y   += $(foreach INC,$(addprefix $(ASF_PATH)/,$(INC_PATH)),'-Wa,-I$(INC)')
cpuflags-gnu-y  += -march=$(ARCH) -mpart=$(PART) # CPU specific flags.

depflags        = -MD -MP -MQ $@ # Dependency file flags.

# Debug specific flags.
ifdef BUILD_DEBUG_LEVEL
dbgflags-gnu-y  += -g$(BUILD_DEBUG_LEVEL)
else
dbgflags-gnu-y  += -g3
endif

# Optimization specific flags.
ifdef BUILD_OPTIMIZATION
optflags-gnu-y  = -O$(BUILD_OPTIMIZATION)
else
optflags-gnu-y  = $(OPTIMIZATION)
endif

# Application optimization used during compilation and linking:
# -O0, -O1, -O2, -O3 or -Os
# OPTIMIZATION = -Os
# OPTIMIZATION = -O0
# OPTIMIZATION = -O1
# OPTIMIZATION = -O2
OPTIMIZATION = -O3

# debug:
# OPTIMIZATION = -O0 -fno-inline

# Relax compilation and linking.
cflags-gnu-y    += -mrelax
asflags-gnu-y   += -mrelax
ldflags-gnu-y   += -Wl,--relax,--direct-data

# Patch for compiler bug
cflags-gnu-y    += -mno-cond-exec-before-reload

# Always preprocess assembler files.
asflags-gnu-y   += -x assembler-with-cpp

# Compile C files using the GNU99 standard.
cflags-gnu-y    += -std=gnu99

# Use signed character type when compiling.
cflags-gnu-y    += -funsigned-char

# Don't use strict aliasing (very common in embedded applications).
cflags-gnu-y    += -fno-strict-aliasing

# Separate each function and data into its own separate section to allow
# garbage collection of unused sections.
cflags-gnu-y    += -ffunction-sections -fdata-sections

# Garbage collect unreferred sections when linking.
ldflags-gnu-y   += -Wl,--gc-sections

# Use the linker script if provided by the project.
ifneq ($(strip $(LINKER_SCRIPT)),)
ldflags-gnu-y   += -Wl,-T $(ASF_PATH)/$(LINKER_SCRIPT)
endif

# Output a link map file and a cross reference table
ldflags-gnu-y   += -Wl,-Map=$(project).map,--cref

# Add library search paths relative to the top level directory.
ldflags-gnu-y   += $(foreach _LIB_PATH,$(addprefix $(ASF_PATH)/,$(LIB_PATH)),-L$(_LIB_PATH))

a_flags  = $(cpuflags-gnu-y) $(depflags) $(cppflags-gnu-y) $(asflags-gnu-y) -D__ASSEMBLY__
c_flags  = $(cpuflags-gnu-y) $(dbgflags-gnu-y) $(depflags) $(optflags-gnu-y) $(cppflags-gnu-y) $(cflags-gnu-y)
l_flags  = $(cpuflags-gnu-y) $(optflags-gnu-y) $(ldflags-gnu-y)
ar_flags = $(arflags-gnu-y)

# Source files list and part information must already be included before
# running this makefile

# If a custom build directory is specified, use it -- force trailing / in directory name.
ifdef BUILD_DIR
	build-dir       := $(dir $(BUILD_DIR))$(if $(notdir $(BUILD_DIR)),$(notdir $(BUILD_DIR))/)
else
	build-dir        =
endif

# Create object files list from source files list.
obj-y                   := $(addprefix $(build-dir), $(addsuffix .o,$(basename $(CSRCS) $(ASSRCS))))
# Create dependency files list from source files list.
dep-files               := $(wildcard $(foreach f,$(obj-y),$(basename $(f)).d))
clean-files             += $(wildcard $(obj-y))
clean-files             += $(dep-files)
clean-dirs              += $(call reverse,$(sort $(wildcard $(dir $(obj-y)))))

# Default target.
.PHONY: all
all: $(target) $(project).lss $(project).sym $(project).hex $(project).bin

# Clean up the project.
.PHONY: clean
clean:
	@$(if $(strip $(clean-files)),echo "RM      *.o *.d")
	$(if $(strip $(clean-files)),rm $(clean-files),)
	@$(if $(strip $(clean-dirs)),echo "RMDIR   $(strip $(clean-dirs))")
# Remove created directories, and make sure we only remove existing
# directories, since recursive rmdir might help us a bit on the way.
	$(if $(strip $(clean-dirs)),                        \
		for directory in $(strip $(clean-dirs)); do     \
			if [ -d "$$directory" ]; then           \
				rmdir -p --ignore-fail-on-non-empty $$directory;           \
			fi                                      \
		done                                            \
	)

# Rebuild the project.
.PHONY: rebuild
rebuild: clean all

.PHONY: objfiles
objfiles: $(obj-y)

# Create object files from C source files.
$(build-dir)%.o: %.c $(MAKEFILE_PATH)
#	test -d $(dir $@) || @echo "MKDIR   $(dir $@)"
	@test -d $(dir $@) || mkdir -p $(dir $@)
	@echo $(CC) $@
	@$(CC) $(c_flags) -c $< -o $@

# Preprocess and assemble: create object files from assembler source files.
$(build-dir)%.o: %.s $(MAKEFILE_PATH)
#	test -d $(dir $@) || @echo "MKDIR   $(dir $@)"
	@test -d $(dir $@) || mkdir -p $(dir $@)
	@echo $(CC) $@
	@$(CC) $(a_flags) -c $< -o $@

# Preprocess and assemble: create object files from assembler source files.
$(build-dir)%.o: %.S $(MAKEFILE_PATH)
#	test -d $(dir $@) || echo "MKDIR   $(dir $@)"
	@test -d $(dir $@) || mkdir -p $(dir $@)
	@echo $(CC) $@
	@$(CC) $(a_flags) -c $< -o $@

# Include all dependency files to add depedency to all header files in use.
include $(dep-files)

# Link the object files into an ELF file. Also make sure the target is rebuilt
# if the common Makefile.in or project config.mk is changed.
$(target): $(ASF_PATH)/$(LINKER_SCRIPT) $(obj-y)
	@echo "LN      $@"
	@$(LD) $(l_flags) $(obj-y) -Wl,--start-group $(libflags-gnu-y) -Wl,--end-group -o $@
	$(SIZE) -Ax $@
	$(SIZE) -Bx $@
	@echo "\r\n"

# Create extended function listing from target output file.
%.lss: $(target)
	$(OBJDUMP) -h -S $< > $@

# Create symbol table from target output file.
%.sym: $(target)
	$(NM) -n $< > $@

# Create Intel HEX image from ELF output file.
%.hex: $(target)
	$(OBJCOPY) -O ihex $< $@

# Create binary image from ELF output file.
%.bin: $(target)
	$(OBJCOPY) -O binary $< $@

# Perform a JTAG chip erase command
.PHONY: chiperase
chiperase:
	@echo $(MSG_ERASING_CHIP)
	$(Q)$(PROGRAM) chiperase

# Get CPU information
.PHONY: cpuinfo
cpuinfo:
	@echo $(MSG_GETTING_CPU_INFO)
	$(Q)$(PROGRAM) cpuinfo

# Open a debug connection with the MCU
.PHONY: debug
debug: $(filter program,$(MAKECMDGOALS))
	@echo $(MSG_DEBUGGING)
	$(Q)$(GDBPROXY) $(FLASH:%=-f%)

# Perform a flash erase
.PHONY: erase
erase:
# The erase target should only be executed when program target is NOT given.
ifeq ($(filter program,$(MAKECMDGOALS)),)
	@echo $(MSG_ERASING)
	$(Q)$(PROGRAM) erase $(FLASH:%=-f%)
else
	@:
endif

# Stop CPU execution
.PHONY: halt
halt:
	@echo $(MSG_HALTING)
	$(Q)$(PROGRAM) halt

# Program MCU memory from ELF output file
.PHONY: program
program: $(target)
	@echo $(MSG_PROGRAMMING)
	$(Q)$(PROGRAM) program $(FLASH:%=-f%) $(PROG_CLOCK:%=-c%) -e -v $(if $(findstring reset,$(MAKECMDGOALS)),-R) $(if $(findstring run,$(MAKECMDGOALS)),-r) $(target)

# Reset MCU.
.PHONY: reset
reset:
# The reset target should only be executed when run and program targets are NOT
# given.
ifeq ($(filter run program,$(MAKECMDGOALS)),)
	@echo $(MSG_RESETTING)
	$(Q)$(PROGRAM) reset
else
	@:
endif

# Read CPU registers
.PHONY: readregs
readregs:
	@echo $(MSG_READING_CPU_REGS)
	$(Q)$(PROGRAM) readregs

# Start CPU execution
.PHONY: run
run:
# The run target should only be executed when the program target is NOT given.
ifeq ($(filter program,$(MAKECMDGOALS)),)
	@echo $(MSG_RUNNING)
	$(Q)$(PROGRAM) run $(if $(findstring reset,$(MAKECMDGOALS)),-R)
else
	@:
endif

# Protect chip by setting security bit
.PHONY: secureflash
# TODO: the secureflash target is not thread safe and should depend upon erase
# and program if these goals are given to make. By adding these dependencies to
# secureflash will cause make to print unnecessary "Nothing to be done for..."
# messages.
secureflash:
	@echo $(MSG_SECURING_FLASH)
	$(Q)$(PROGRAM) secureflash
