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
PRJ_PATH = ../../../../../../../../..

# Microcontroller: atxmega128a1, atmega128, attiny261, etc.
MCU = atxmega256a3bu

# Application target name. Given with suffix .a for library and .elf for a
# standalone application.
TARGET = device_unit_tests.elf

# C source files located from the top-level source directory
CSRCS = \
       common/components/memory/data_flash/at45dbx/at45dbx.c \
       common/components/memory/data_flash/at45dbx/at45dbx_mem.c \
       common/services/clock/xmega/sysclk.c               \
       common/services/ioport/xmega/ioport_compat.c       \
       common/services/serial/usart_serial.c              \
       common/services/sleepmgr/xmega/sleepmgr.c          \
       common/services/spi/xmega_usart_spi/usart_spi.c    \
       common/services/storage/ctrl_access/ctrl_access.c  \
       common/services/usb/class/msc/device/udi_msc.c     \
       common/services/usb/class/msc/device/udi_msc_desc.c \
       common/services/usb/class/msc/device/unit_tests/memories_initialization_xmega.c \
       common/services/usb/class/msc/device/unit_tests/unit_tests.c \
       common/services/usb/udc/udc.c                      \
       common/utils/stdio/read.c                          \
       common/utils/stdio/write.c                         \
       common/utils/unit_test/suite.c                     \
       xmega/boards/xmega_a3bu_xplained/init.c            \
       xmega/drivers/nvm/nvm.c                            \
       xmega/drivers/usart/usart.c                        \
       xmega/drivers/usb/usb_device.c

# Assembler source files located from the top-level source directory
ASSRCS = \
       xmega/drivers/cpu/ccp.s                            \
       xmega/drivers/nvm/nvm_asm.s

# Include path located from the top-level source directory
INC_PATH = \
       common/boards                                      \
       common/components/memory/data_flash/at45dbx        \
       common/services/clock                              \
       common/services/gpio                               \
       common/services/ioport                             \
       common/services/serial                             \
       common/services/serial/xmega_usart                 \
       common/services/sleepmgr                           \
       common/services/spi                                \
       common/services/storage/ctrl_access                \
       common/services/usb                                \
       common/services/usb/class/msc                      \
       common/services/usb/class/msc/device               \
       common/services/usb/class/msc/device/unit_tests    \
       common/services/usb/class/msc/device/unit_tests/atxmega256a3bu_xmega_a3bu_xplained \
       common/services/usb/udc                            \
       common/utils                                       \
       common/utils/stdio/stdio_serial                    \
       xmega/boards                                       \
       xmega/boards/xmega_a3bu_xplained                   \
       xmega/drivers/cpu                                  \
       xmega/drivers/nvm                                  \
       xmega/drivers/pmic                                 \
       xmega/drivers/sleep                                \
       xmega/drivers/usart                                \
       xmega/drivers/usb                                  \
       xmega/utils                                        \
       xmega/utils/preprocessor \
       common/services/usb/class/msc/device/unit_tests/atxmega256a3bu_xmega_a3bu_xplained/gcc

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
       -D ACCESS_USB_ENABLED                              \
       -D AT45DBX_ENABLE                                  \
       -D BOARD=XMEGA_A3BU_XPLAINED                       \
       -D CONFIG_NVM_IGNORE_XMEGA_A3_D3_REVB_ERRATA       \
       -D IOPORT_XMEGA_COMPAT                             \
       -D TEST_SUITE_DEFINE_ASSERT_MACRO                  \
       -D _ASSERT_ENABLE_

# Extra flags to use when linking
LDFLAGS =  \
       -Wl,--section-start=.BOOT=0x40000                 
