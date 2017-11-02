# --- config.mk
#
# --- customized makefile for aleph-avr32 application.
# --- this is included via the ASF utility makefile.

# app name
APP = picolisp

BAUD = 115200

# boilerplate avr32 configuration
include ../../avr32_lib/avr32_lib_config.mk
# boilerplate avr32 sources
include ../../avr32_lib/avr32_lib_src.mk

# app sources
CSRCS += \
	$(APP_DIR)/src/app_picolisp.c \
	$(APP_DIR)/src/app_timers.c \
	$(APP_DIR)/src/render.c \
	$(APP_DIR)/src/ctl.c \
	$(APP_DIR)/src/util.c \
	$(APP_DIR)/src/files.c \
	$(APP_DIR)/src/handler.c \
	$(APP_DIR)/src/pil_main.c \
	$(APP_DIR)/src/gc.c \
	$(APP_DIR)/src/apply.c \
	$(APP_DIR)/src/flow.c \
	$(APP_DIR)/src/sym.c \
	$(APP_DIR)/src/subr.c \
	$(APP_DIR)/src/io.c \
	$(APP_DIR)/src/math.c \
	$(APP_DIR)/src/ser.c \
	$(APP_DIR)/src/tab.c

# List of assembler source files.
ASSRCS +=

# List of include paths.	
INC_PATH += \
	$(APP_DIR)	           \
	$(APP_DIR)/src	\
	$(APP_DIR)/../../../avr32-toolchain-linux/include
