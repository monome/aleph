# --- config.mk
#
# --- customized makefile for aleph-avr32 application.
# --- this is included via the ASF utility makefile.

# app name
APP = bees

# boilerplate avr32 configuration
include ../../core_config.mk
# boilerplate avr32 sources
include ../../core_src.mk


# add C source files from application
CSRCS += \
	$(APP_DIR)/src/app_bees.c \
	$(APP_DIR)/src/app_timers.c \
	$(APP_DIR)/src/files.c \
	$(APP_DIR)/src/flash_bees.c \
	$(APP_DIR)/src/handler.c \
	$(APP_DIR)/src/net.c \
	$(APP_DIR)/src/net_monome.c \
	$(APP_DIR)/src/net_poll.c \
	$(APP_DIR)/src/op.c \
	$(APP_DIR)/src/param.c \
	$(APP_DIR)/src/pages.c \
	$(APP_DIR)/src/pickle.c \
	$(APP_DIR)/src/play.c \
	$(APP_DIR)/src/preset.c \
	$(APP_DIR)/src/render.c \
	$(APP_DIR)/src/scene.c \
	$(APP_DIR)/src/ops/op_add.c \
	$(APP_DIR)/src/ops/op_enc.c \
	$(APP_DIR)/src/ops/op_gate.c \
	$(APP_DIR)/src/ops/op_mul.c \
	$(APP_DIR)/src/ops/op_monome_grid_raw.c \
	$(APP_DIR)/src/ops/op_sw.c \
	$(APP_DIR)/src/pages/page_dsp.c \
	$(APP_DIR)/src/pages/page_gathered.c \
	$(APP_DIR)/src/pages/page_ins.c \
	$(APP_DIR)/src/pages/page_ops.c \
	$(APP_DIR)/src/pages/page_outs.c \
	$(APP_DIR)/src/pages/page_play.c \
	$(APP_DIR)/src/pages/page_presets.c \
	$(APP_DIR)/src/pages/page_scenes.c 


# List of assembler source files.
ASSRCS +=

# List of include paths.	
INC_PATH += \
	$(APP_DIR)	           \
	$(APP_DIR)/src \
	$(APP_DIR)/../../common # ughh blergg


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
