# --- config.mk
#
# --- customized makefile for aleph-avr32 application.
# --- this is included via the ASF utility makefile.

APP = flry

# boilerplate avr32 configuration
include ../../avr32_lib/avr32_lib_config.mk
# boilerplate avr32 sources
include ../../avr32_lib/avr32_lib_src.mk

# List of C source files.
CSRCS += \
	$(APP_DIR)/src/app_flry.c \
	$(APP_DIR)/src/app_timers.c \
	$(APP_DIR)/src/files.c \
	$(APP_DIR)/src/handler.c \
	$(APP_DIR)/src/grid.c \
	$(APP_DIR)/src/ctl.c \
	$(APP_DIR)/src/inputs.c	\
	$(APP_DIR)/src/life.c	\
	$(APP_DIR)/src/render.c 	

# List of assembler source files.
ASSRCS +=

# List of include paths.	
INC_PATH += \
	$(APP_DIR)	           \
	$(APP_DIR)/src

# Additional search paths for libraries.
LIB_PATH += 

# List of libraries to use during linking.
LIBS += 

# Extra flags to use when archiving.
ARFLAGS += 

# Extra flags to use when assembling.
ASFLAGS += 


# debug:
# OPTIMIZATION = -Og
