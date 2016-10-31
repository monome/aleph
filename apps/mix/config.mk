# --- config.mk
#
# --- customized makefile for aleph-avr32 application.
# --- this is included via the ASF utility makefile.

# app name
APP = mix

# baudrate! can override in make invocation
BAUD = 115200

# boilerplate avr32 configuration
# include ../../avr32_lib/avr32_lib_config.mk
# boilerplate avr32 sources
# include ../../avr32_lib/avr32_lib_src.mk
# avr32 configuration
include ../aleph_avr32_config.mk
# avr32 sources
include ../aleph_avr32_src.mk

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
