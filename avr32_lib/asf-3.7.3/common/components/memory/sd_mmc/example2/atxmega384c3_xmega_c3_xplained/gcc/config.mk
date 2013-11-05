#
# Copyright (c) 2010 Atmel Corporation. All rights reserved.
#
# \asf_license_start
#
# \page License
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# 3. The name of Atmel may not be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# 4. This software may only be redistributed and used in connection with an
#    Atmel microcontroller product.
#
# THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
# EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
# \asf_license_stop
#

# Path to top level ASF directory relative to this project directory.
PRJ_PATH = ../../../../../../..

# Microcontroller: atxmega128a1, atmega128, attiny261, etc.
MCU = atxmega384c3

# Application target name. Given with suffix .a for library and .elf for a
# standalone application.
TARGET = sd_mmc_example2.elf

# C source files located from the top-level source directory
CSRCS = \
       common/components/memory/sd_mmc/example2/example.c \
       common/components/memory/sd_mmc/sd_mmc.c           \
       common/components/memory/sd_mmc/sd_mmc_mem.c       \
       common/components/memory/sd_mmc/sd_mmc_spi.c       \
       common/services/calendar/calendar.c                \
       common/services/clock/xmega/sysclk.c               \
       common/services/ioport/xmega/ioport_compat.c       \
       common/services/serial/usart_serial.c              \
       common/services/sleepmgr/xmega/sleepmgr.c          \
       common/services/spi/xmega_usart_spi/usart_spi.c    \
       common/services/storage/ctrl_access/ctrl_access.c  \
       common/utils/stdio/read.c                          \
       common/utils/stdio/write.c                         \
       thirdparty/fatfs/fatfs-port-r0.09/diskio.c         \
       thirdparty/fatfs/fatfs-port-r0.09/xmega/fattime.c  \
       thirdparty/fatfs/fatfs-r0.09/src/ff.c              \
       thirdparty/fatfs/fatfs-r0.09/src/option/ccsbcs.c   \
       xmega/boards/xmega_c3_xplained/init.c              \
       xmega/drivers/nvm/nvm.c                            \
       xmega/drivers/rtc/rtc.c                            \
       xmega/drivers/usart/usart.c

# Assembler source files located from the top-level source directory
ASSRCS = \
       xmega/drivers/cpu/ccp.s                            \
       xmega/drivers/nvm/nvm_asm.s

# Include path located from the top-level source directory
INC_PATH = \
       common/boards                                      \
       common/components/memory/sd_mmc                    \
       common/components/memory/sd_mmc/example2           \
       common/components/memory/sd_mmc/example2/atxmega384c3_xmega_c3_xplained \
       common/services/calendar                           \
       common/services/clock                              \
       common/services/delay                              \
       common/services/gpio                               \
       common/services/ioport                             \
       common/services/serial                             \
       common/services/serial/xmega_usart                 \
       common/services/sleepmgr                           \
       common/services/spi                                \
       common/services/storage/ctrl_access                \
       common/utils                                       \
       common/utils/stdio/stdio_serial                    \
       thirdparty/fatfs/fatfs-port-r0.09/xmega            \
       thirdparty/fatfs/fatfs-r0.09/src                   \
       xmega/boards                                       \
       xmega/boards/xmega_c3_xplained                     \
       xmega/drivers/cpu                                  \
       xmega/drivers/nvm                                  \
       xmega/drivers/pmic                                 \
       xmega/drivers/rtc                                  \
       xmega/drivers/sleep                                \
       xmega/drivers/usart                                \
       xmega/utils                                        \
       xmega/utils/preprocessor \
       common/components/memory/sd_mmc/example2/atxmega384c3_xmega_c3_xplained/gcc

# Library paths from the top-level source directory
LIB_PATH = 

# Libraries to link with the project
LIBS = 

# Additional options for debugging. By default the common Makefile.in will
# add -gdwarf-2.
DBGFLAGS = 

# Optimization settings
OPTIMIZATION = -Os

# Extra flags used when creating an EEPROM Intel HEX file. By default the
# common Makefile.in will add -j .eeprom
# --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0.
EEPROMFLAGS = 

# Extra flags used when creating an Intel HEX file. By default the common
# Makefile.in will add -R .eeprom -R .usb_descriptor_table.
FLASHFLAGS = 

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
       -D BOARD=XMEGA_C3_XPLAINED                         \
       -D IOPORT_XMEGA_COMPAT                             \
       -D SD_MMC_ENABLE

# Extra flags to use when linking
LDFLAGS =  \
       -Wl,--section-start=.BOOT=0x60000                 
