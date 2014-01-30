# --- config.mk
#
# --- customized makefile for aleph-avr32 application.
# --- this is included via the ASF utility makefile.

# app name
APP = dsyn

# boilerplate avr32 configuration
include ../../avr32_lib/avr32_lib_config.mk
# boilerplate avr32 sources
include ../../avr32_lib/avr32_lib_src.mk

# app sources
CSRCS += \
	$(APP_DIR)/src/app_dsyn.c \
	$(APP_DIR)/src/app_timers.c \
	$(APP_DIR)/src/files.c \
	$(APP_DIR)/src/grid.c \
	$(APP_DIR)/src/handler.c \
	$(APP_DIR)/src/ctl.c \
	$(APP_DIR)/src/inputs.c	\
	$(APP_DIR)/src/render.c \
	$(APP_DIR)/src/sequence.c \
	$(APP_DIR)/src/util.c


# List of assembler source files.
ASSRCS +=

# List of include paths.	
INC_PATH += \
	$(APP_DIR)	           \
	$(APP_DIR)/src