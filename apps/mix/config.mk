# --- config.mk
#
# --- customized makefile for aleph-avr32 application.
# --- this is included via the ASF utility makefile.

# app name
APP = mix

BAUD = 115200

# boilerplate avr32 configuration
include ../../avr32_lib/avr32_lib_config.mk
# boilerplate avr32 sources
include ../../avr32_lib/avr32_lib_src.mk

# app sources
CSRCS += \
	$(APP_DIR)/src/app_mix.c \
	$(APP_DIR)/src/app_timers.c \
	$(APP_DIR)/src/ctl.c \
	$(APP_DIR)/src/handler.c \
	$(APP_DIR)/src/render.c \
	$(APP_DIR)/src/scaler.c

# List of assembler source files.
ASSRCS +=

# List of include paths.	
INC_PATH += \
	$(APP_DIR)	           \
	$(APP_DIR)/src	\
	$(APP_DIR)/../../../avr32-toolchain-linux/include
