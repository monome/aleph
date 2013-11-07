# a starting place

CSRCS += \
	avr32/drivers/flashc/flashc.c                      \
	avr32/drivers/gpio/gpio.c                          \
	avr32/drivers/intc/intc.c                          \
	avr32/drivers/pm/pm.c                              \
	avr32/drivers/pm/pm_conf_clocks.c                  \
	avr32/drivers/pm/power_clocks_lib.c                \
        avr32/drivers/tc/tc.c                          \
	avr32/drivers/usart/usart.c                        \
	avr32/drivers/twi/twi.c                        \
	avr32/utils/debug/print_funcs.c                    \
        common/services/clock/uc3a0_a1/sysclk.c            \
	$(ALEPH_AVR32)/src/i2c.c	\
	$(ALEPH_AVR32)/src/init.c	\
	$(ALEPH_AVR32)/src/interrupts.c \
	$(ALEPH_AVR32)/src/main.c \


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
	avr32/drivers/cpu/cycle_counter/                 \
	avr32/drivers/eic/                            \
       avr32/drivers/flashc                               \
       avr32/drivers/gpio                                 \
       avr32/drivers/intc                                 \
       avr32/drivers/pm                                   \
       avr32/drivers/spi                                  \
       avr32/drivers/tc                                 \
       avr32/drivers/twi                                 \
       avr32/drivers/usart                                \
       avr32/services/delay                               \
       avr32/utils                                        \
       avr32/utils/debug                                  \
       avr32/utils/preprocessor                           \
       common/boards                                      \
	avr32/components/memory/sd_mmc/sd_mmc_spi         \
       common/services/clock                              \
	common/services/delay                             \
       common/utils                \
	$(ALEPH_AVR32)	           \
	$(ALEPH_AVR32)/src	   \
	$(ALEPH_AVR32)/conf	   