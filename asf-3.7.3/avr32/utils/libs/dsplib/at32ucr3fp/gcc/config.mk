#
# Copyright (c) 2009-2010 Atmel Corporation. All rights reserved.
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
PRJ_PATH = ../../../../../..
APPS_PATH = avr32/applications
BRDS_PATH = avr32/boards
COMP_PATH = avr32/components
DRVR_PATH = avr32/drivers
SERV_PATH = avr32/services
UTIL_PATH = avr32/utils

# DSP library path
DSPLIB_PATH = $(SERV_PATH)/dsp/dsplib
DSPLIB_GENERIC_PATH = $(DSPLIB_PATH)/generic
DSPLIB_OPTIM_PATH = $(DSPLIB_PATH)/at32uc
DSPLIB_BASIC_PATH = $(DSPLIB_OPTIM_PATH)/basic

# include generic configuration file
include $(PRJ_PATH)/$(DSPLIB_GENERIC_PATH)/build/gcc/config.mk

# Target CPU architecture: ap, ucr1, ucr2 or ucr3
ARCH = ucr3fp

# Target part: none, ap7xxx or uc3xxxxx
PART = uc3c0512c

# Target device flash memory details (used by the avr32program programming
# tool: [cfi|internal]@address
FLASH = internal@0x80000000

# Clock source to use when programming; xtal, extclk or int
PROG_CLOCK = int

# Application target name. Given with suffix .a for library and .elf for a
# standalone application.
TARGET = $(DSPLIB_PATH)/lib/libdsp_generic_$(PART)_speed.a

# Include path
INC_PATH += \
  common/interrupt/                \
  common/utils/debug/              \
  avr32/drivers/intc/ \
  $(DSPLIB_BASIC_PATH)/filtering/ \
  $(DSPLIB_BASIC_PATH)/operators/ \
  $(DSPLIB_BASIC_PATH)/signal_generation/ \
  $(DSPLIB_BASIC_PATH)/transforms/ \
  $(DSPLIB_BASIC_PATH)/vectors/ \
  $(DSPLIB_BASIC_PATH)/windowing/

# Target specific sources for the filtering module
FILTERING_SRC = \
  $(DSPLIB_BASIC_PATH)/filtering/filt_dsp16_iir_at32uc.c \
  $(DSPLIB_BASIC_PATH)/filtering/filt_dsp16_iirpart_at32uc.c \
  $(DSPLIB_BASIC_PATH)/filtering/filt_dsp32_lms_at32uc.c \
  $(DSPLIB_BASIC_PATH)/filtering/filt_dsp32_nlms_at32uc.c

# Target specific sources for the operators module
OPERATORS_SRC = \
  $(DSPLIB_BASIC_PATH)/operators/op_dsp16_ln_at32uc.c \
  $(DSPLIB_BASIC_PATH)/operators/op_dsp16_sqrt_at32uc.c

# Target specific sources for the signal generation module
SIGNAL_GENERATION_SRC =

# Target specific sources for the transforms module
TRANSFORMS_SRC = \
  $(DSPLIB_BASIC_PATH)/transforms/trans_dsp16_complex_fft_at32uc.c \
  $(DSPLIB_BASIC_PATH)/transforms/trans_dsp16_complex_ifft_at32uc.c \
  $(DSPLIB_BASIC_PATH)/transforms/trans_dsp16_real_complex_fft_at32uc.c

# Target specific sources for the vectors module
VECTORS_SRC = \
  $(DSPLIB_BASIC_PATH)/vectors/vect_dsp16_add_at32uc.c \
  $(DSPLIB_BASIC_PATH)/vectors/vect_dsp16_convpart_at32uc.c \
  $(DSPLIB_BASIC_PATH)/vectors/vect_dsp16_dotdiv_at32uc.c \
  $(DSPLIB_BASIC_PATH)/vectors/vect_dsp16_dotmul_at32uc.c \
  $(DSPLIB_BASIC_PATH)/vectors/vect_dsp16_neg_at32uc.c \
  $(DSPLIB_BASIC_PATH)/vectors/vect_dsp16_realdiv_at32uc.c \
  $(DSPLIB_BASIC_PATH)/vectors/vect_dsp16_realmul_at32uc.c \
  $(DSPLIB_BASIC_PATH)/vectors/vect_dsp16_sub_at32uc.c \
  $(DSPLIB_BASIC_PATH)/vectors/vect_dsp32_add_at32uc.c \
  $(DSPLIB_BASIC_PATH)/vectors/vect_dsp32_convpart_at32uc.c \
  $(DSPLIB_BASIC_PATH)/vectors/vect_dsp32_max_at32uc.c \
  $(DSPLIB_BASIC_PATH)/vectors/vect_dsp32_min_at32uc.c \
  $(DSPLIB_BASIC_PATH)/vectors/vect_dsp32_neg_at32uc.c \
  $(DSPLIB_BASIC_PATH)/vectors/vect_dsp32_realmul_at32uc.c \
  $(DSPLIB_BASIC_PATH)/vectors/vect_dsp32_sub_at32uc.c \
  $(DSPLIB_BASIC_PATH)/vectors/vect_dsp16_complex_conj_at32uc.c

# Target specific sources for the windowing module
WINDOWING_SRC =

# C source files
CSRCS := \
  $(CSRCS) \
  $(FILTERING_SRC) \
  $(OPERATORS_SRC) \
  $(SIGNAL_GENERATION_SRC) \
  $(TRANSFORMS_SRC) \
  $(VECTORS_SRC) \
  $(WINDOWING_SRC)

# Assembler source files
ASSRCS = \
  $(DSPLIB_BASIC_PATH)/filtering/filt_dsp16_lms_at32uc.S \
  $(DSPLIB_BASIC_PATH)/filtering/filt_dsp16_nlms_at32uc.S \
  $(DSPLIB_BASIC_PATH)/filtering/filt_dsp32_lms_fir_at32uc.S \
  $(DSPLIB_BASIC_PATH)/filtering/filt_dsp32_nlms_fir_at32uc.S

# Additional search paths for libraries.
LIB_PATH =

# List of libraries to use during linking.
LIBS =

# Path relative to top level directory pointing to a linker script.
LINKER_SCRIPT =

# Additional options for debugging. By default the common Makefile.in will
# add -g3.
DBGFLAGS =

# Application optimization used during compilation and linking:
# -O0, -O1, -O2, -O3 or -Os
OPTIMIZATION =

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
#   FORCE_ALL_GENERICS  To use generic C coded version of the functions
#   DSP_LMS_BETA        Beta coefficient for the LMS filter
#   DSP_NLMS_BETA       Beta coefficient for the NLMS filter
CPPFLAGS =

# Extra flags to use when linking
LDFLAGS =
