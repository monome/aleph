# avr32 core directory
ALEPH_AVR32 = ../avr32
# aleph common directory
ALEPH_COMMON = ../common

ALEPH_BEES = ../avr32/apps/bees


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
TARGET = aleph-bees.elf

# Core translation units: drivers, I/O, Aleph source etc.
CSRCS += \
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
	avr32/drivers/usart/usart.c                        \
	avr32/utils/debug/print_funcs.c                    \
	avr32/components/memory/sd_mmc/sd_mmc_spi/sd_mmc_spi.c \
	avr32/components/memory/sd_mmc/sd_mmc_spi/sd_mmc_spi_mem.c \
	avr32/drivers/usbb/usbb_host.c                     \
        common/services/clock/uc3a0_a1/sysclk.c            \
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
	$(ALEPH_AVR32)/src/region.c \
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

# List of assembler source files.
ASSRCS += \
       avr32/drivers/intc/exception.S                     \
       avr32/utils/startup/startup_uc3.S                     \
       avr32/utils/startup/trampoline_uc3.S                     
#	$(ALEPH_AVR32)/startup/startup.S \
#	$(ALEPH_AVR32)/startup/trampoline.S


# List of include paths.
INC_PATH += \
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
       avr32/drivers/usbb                                 \
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
       common/services/usb                                \
       common/services/usb/class/hid                      \
       common/services/usb/uhc                            \
       common/utils                \
	$(ALEPH_AVR32)	           \
	$(ALEPH_AVR32)/src	   \
	$(ALEPH_AVR32)/conf	   \
	$(ALEPH_AVR32)/src/fat_io_lib  \
	$(ALEPH_AVR32)/src/usb     \
	$(ALEPH_AVR32)/src/usb/ftdi   \
	$(ALEPH_AVR32)/src/usb/gamepad \
	$(ALEPH_AVR32)/src/usb/midi   \
	$(ALEPH_AVR32)/src/usb/mouse   \
	$(ALEPH_COMMON)	           \
	$(ALEPH_COMMON)/libfixmath

# List of C source files.
CSRCS += \
	$(ALEPH_BEES)/src/app_bees.c \
	$(ALEPH_BEES)/src/app_timers.c \
	$(ALEPH_BEES)/src/files.c \
	$(ALEPH_BEES)/src/flash_bees.c \
	$(ALEPH_BEES)/src/handler.c \
	$(ALEPH_BEES)/src/menu.c \
	$(ALEPH_BEES)/src/net.c \
	$(ALEPH_BEES)/src/net_monome.c \
	$(ALEPH_BEES)/src/net_poll.c \
	$(ALEPH_BEES)/src/op.c \
	$(ALEPH_BEES)/src/param.c \
	$(ALEPH_BEES)/src/play.c \
	$(ALEPH_BEES)/src/preset.c \
	$(ALEPH_BEES)/src/render.c \
	$(ALEPH_BEES)/src/scene.c \
	$(ALEPH_BEES)/src/ops/op_add.c \
	$(ALEPH_BEES)/src/ops/op_enc.c \
	$(ALEPH_BEES)/src/ops/op_gate.c \
	$(ALEPH_BEES)/src/ops/op_mul.c \
	$(ALEPH_BEES)/src/ops/op_monome_grid_raw.c \
	$(ALEPH_BEES)/src/ops/op_sw.c \
	$(ALEPH_BEES)/src/pages/page_dsp.c \
	$(ALEPH_BEES)/src/pages/page_gathered.c \
	$(ALEPH_BEES)/src/pages/page_ins.c \
	$(ALEPH_BEES)/src/pages/page_outs.c \
	$(ALEPH_BEES)/src/pages/page_play.c \
	$(ALEPH_BEES)/src/pages/page_presets.c \
	$(ALEPH_BEES)/src/pages/page_scenes.c 


# List of assembler source files.
ASSRCS +=

# List of include paths.	
INC_PATH += \
	$(ALEPH_BEES)	           \
	$(ALEPH_BEES)/src

# Additional search paths for libraries.
LIB_PATH = 

# List of libraries to use during linking.
LIBS = 

# Path relative to top level directory pointing to a linker script.
# LINKER_SCRIPT = avr32/utils/linker_scripts/at32uc3a/0512/gcc/link_uc3a0512.lds
LINKER_SCRIPT = $(ALEPH_BEES)/aleph-bees.lds

# AVR32 options for debugging. By default the common Makefile.in will
# add -g3.
DBGFLAGS =

# Application optimization used during compilation and linking:
# -O0, -O1, -O2, -O3 or -Os
# OPTIMIZATION = -Os
# OPTIMIZATION = -O0
# OPTIMIZATION = -O1
OPTIMIZATION = -O2
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