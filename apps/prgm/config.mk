# --- config.mk
#
# --- customized makefile for aleph-avr32 application.
# --- this is included via the ASF utility makefile.

# app name
APP = prgm

# boilerplate avr32 configuration
include ../../avr32_lib/avr32_lib_config.mk
# boilerplate avr32 sources
include ../../avr32_lib/avr32_lib_src.mk

# app sources
CSRCS += \
	$(APP_DIR)/src/app_prgm.c \
	$(APP_DIR)/src/app_timers.c \
	$(APP_DIR)/src/files.c \
	$(APP_DIR)/src/handler.c \
	$(APP_DIR)/src/ctl.c \
	$(APP_DIR)/src/render.c \
	$(APP_DIR)/src/util.c \
	$(APP_DIR)/src/tracker.c \
	$(APP_DIR)/src/pages.c \
	$(APP_DIR)/src/page_param.c \
	$(APP_DIR)/src/page_env.c \
    $(APP_DIR)/src/scale.c

# List of assembler source files.
ASSRCS +=

# List of include paths.	
INC_PATH += \
	$(APP_DIR)	           \
	$(APP_DIR)/src         \
	$(APP_DIR)/../../common # ughh blergg
    
# Additional search paths for libraries.
LIB_PATH += 

# List of libraries to use during linking.
LIBS += 

# Extra flags to use when archiving.
ARFLAGS += 

# Extra flags to use when assembling.
ASFLAGS += 

# extra flags for compiling
# CFLAGS += --verbose

# debug:
# OPTIMIZATION = -Og
