# ------------------------
# -- aleph_avr32_src.mk

# sources from libavr32
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
avr32/drivers/tc/tc.c                              \
avr32/drivers/twi/twi.c                            \
avr32/drivers/usart/usart.c                        \
avr32/components/memory/sd_mmc/sd_mmc_spi/sd_mmc_spi.c \
avr32/components/memory/sd_mmc/sd_mmc_spi/sd_mmc_spi_mem.c \
avr32/drivers/usbb/usbb_host.c                     \
common/services/clock/uc3a0_a1/sysclk.c            \
common/services/storage/ctrl_access/ctrl_access.c \
common/services/usb/uhc/uhc.c \
$(LIB_AVR32)src/adc.c \
$(LIB_AVR32)src/events.c \
$(LIB_AVR32)src/font.c \
$(LIB_AVR32)src/i2c.c \
$(LIB_AVR32)src/monome.c \
$(LIB_AVR32)src/region.c \
$(LIB_AVR32)src/screen.c \
$(LIB_AVR32)src/timers.c \
$(LIB_AVR32)src/usb.c \
$(LIB_AVR32)src/usb/ftdi/uhi_ftdi.c \
$(LIB_AVR32)src/usb/ftdi/ftdi.c \
$(LIB_AVR32)src/usb/hid/hid.c \
$(LIB_AVR32)src/usb/hid/uhi_hid.c \
$(LIB_AVR32)src/usb/midi/uhi_midi.c \
$(LIB_AVR32)src/usb/midi/midi.c \
$(LIB_AVR32)src/fix.c \
$(LIB_AVR32)src/libfixmath/fix16.c

# include paths from libavr32
INC_PATH += \
avr32/boards                                       \
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
$(LIB_AVR32)	           \
$(LIB_AVR32)src	   \
$(LIB_AVR32)/conf/aleph	   \
$(LIB_AVR32)src/fat_io_lib  \
$(LIB_AVR32)src/usb     \
$(LIB_AVR32)src/usb/ftdi   \
$(LIB_AVR32)src/usb/hid \
$(LIB_AVR32)src/usb/midi   \
$(LIB_AVR32)src/libfixmath

# aleph_specific avr32 sources
CSRCS += \
$(ALEPH_AVR32)/src/adc_poll.c \
$(ALEPH_AVR32)/src/app.c \
$(ALEPH_AVR32)/src/bfin.c \
$(ALEPH_AVR32)/src/control.c \
$(ALEPH_AVR32)/src/encoders.c \
$(ALEPH_AVR32)/src/filesystem.c \
$(ALEPH_AVR32)/src/flash.c \
$(ALEPH_AVR32)/src/font_aa.c \
$(ALEPH_AVR32)/src/init.c	\
$(ALEPH_AVR32)/src/interrupts.c \
$(ALEPH_AVR32)/src/main.c \
$(ALEPH_AVR32)/src/memory.c \
$(ALEPH_AVR32)/src/print_funcs.c \
$(ALEPH_AVR32)/src/region_extra.c \
$(ALEPH_AVR32)/src/serial.c \
$(ALEPH_AVR32)/src/simple_string.c \
$(ALEPH_AVR32)/src/switches.c \
$(ALEPH_AVR32)/src/fat_io_lib/fat_access.c \
$(ALEPH_AVR32)/src/fat_io_lib/fat_cache.c \
$(ALEPH_AVR32)/src/fat_io_lib/fat_filelib.c \
$(ALEPH_AVR32)/src/fat_io_lib/fat_format.c \
$(ALEPH_AVR32)/src/fat_io_lib/fat_misc.c \
$(ALEPH_AVR32)/src/fat_io_lib/fat_string.c \
$(ALEPH_AVR32)/src/fat_io_lib/fat_table.c \
$(ALEPH_AVR32)/src/fat_io_lib/fat_write.c \
$(ALEPH_AVR32)/src/fonts/ume_tgo5_18.c

# List of assembler source files.
ASSRCS += \
avr32/drivers/intc/exception.S \
avr32/utils/startup/trampoline_uc3.S \
$(ALEPH_AVR32)/startup/startup.S \

# aleph-specific avr32 include paths
INC_PATH += \
$(ALEPH_AVR32)/src \
$(ALEPH_AVR32)/src/fat_io_lib \
$(ALEPH_COMMON)
