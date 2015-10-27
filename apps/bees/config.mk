# --- config.mk
#
# --- customized makefile for aleph-avr32 application.
# --- this is included via the ASF utility makefile.

# app name
APP = bees

# baudrate! can override in make invocation
BAUD=115200

# boilerplate avr32 configuration
include ../../avr32_lib/avr32_lib_config.mk
# boilerplate avr32 sources
include ../../avr32_lib/avr32_lib_src.mk


# add C source files from application
# please keep this alphabetized... 
CSRCS += \
	$(APP_DIR)/src/app_bees.c \
	$(APP_DIR)/src/app_timers.c \
	$(APP_DIR)/src/files.c \
	$(APP_DIR)/src/flash_bees.c \
	$(APP_DIR)/src/handler.c \
	$(APP_DIR)/src/net.c \
	$(APP_DIR)/src/net_hid.c \
	$(APP_DIR)/src/net_midi.c \
	$(APP_DIR)/src/net_monome.c \
	$(APP_DIR)/src/net_poll.c \
	$(APP_DIR)/src/op.c \
	$(APP_DIR)/src/op_gfx.c \
	$(APP_DIR)/src/op_math.c \
	$(APP_DIR)/src/param.c \
	$(APP_DIR)/src/pages.c \
	$(APP_DIR)/src/pickle.c \
	$(APP_DIR)/src/play.c \
	$(APP_DIR)/src/preset.c \
	$(APP_DIR)/src/render.c \
	$(APP_DIR)/src/param_scaler.c \
	$(APP_DIR)/src/scene.c \
	$(APP_DIR)/src/ser.c \
	$(APP_DIR)/src/util.c \
	$(APP_DIR)/src/ops/op_add.c \
	$(APP_DIR)/src/ops/op_accum.c \
	$(APP_DIR)/src/ops/op_adc.c \
	$(APP_DIR)/src/ops/op_bars.c \
	$(APP_DIR)/src/ops/op_bars8.c \
	$(APP_DIR)/src/ops/op_bignum.c \
	$(APP_DIR)/src/ops/op_bits.c \
	$(APP_DIR)/src/ops/op_cascades.c \
	$(APP_DIR)/src/ops/op_change.c \
	$(APP_DIR)/src/ops/op_delay.c \
	$(APP_DIR)/src/ops/op_div.c \
	$(APP_DIR)/src/ops/op_divr.c \
	$(APP_DIR)/src/ops/op_enc.c \
	$(APP_DIR)/src/ops/op_fade.c \
	$(APP_DIR)/src/ops/op_gate.c \
	$(APP_DIR)/src/ops/op_hid_word.c \
	$(APP_DIR)/src/ops/op_history.c \
	$(APP_DIR)/src/ops/op_is.c \
	$(APP_DIR)/src/ops/op_life.c \
	$(APP_DIR)/src/ops/op_list2.c \
	$(APP_DIR)/src/ops/op_list8.c \
	$(APP_DIR)/src/ops/op_list16.c \
	$(APP_DIR)/src/ops/op_logic.c \
	$(APP_DIR)/src/ops/op_metro.c \
	$(APP_DIR)/src/ops/op_midi_cc.c \
	$(APP_DIR)/src/ops/op_midi_out_note.c \
	$(APP_DIR)/src/ops/op_midi_out_cc.c \
	$(APP_DIR)/src/ops/op_midi_note.c \
	$(APP_DIR)/src/ops/op_mod.c \
	$(APP_DIR)/src/ops/op_mul.c \
	$(APP_DIR)/src/ops/op_monome_arc.c \
	$(APP_DIR)/src/ops/op_monome_grid_raw.c \
	$(APP_DIR)/src/ops/op_preset.c \
	$(APP_DIR)/src/ops/op_random.c \
	$(APP_DIR)/src/ops/op_route.c \
	$(APP_DIR)/src/ops/op_route8.c \
	$(APP_DIR)/src/ops/op_route16.c \
	$(APP_DIR)/src/ops/op_screen.c \
	$(APP_DIR)/src/ops/op_serial.c \
	$(APP_DIR)/src/ops/op_shl.c \
	$(APP_DIR)/src/ops/op_shr.c \
	$(APP_DIR)/src/ops/op_split.c \
	$(APP_DIR)/src/ops/op_split4.c \
	$(APP_DIR)/src/ops/op_sub.c \
	$(APP_DIR)/src/ops/op_step.c \
	$(APP_DIR)/src/ops/op_sw.c \
	$(APP_DIR)/src/ops/op_thresh.c \
	$(APP_DIR)/src/ops/op_timer.c \
	$(APP_DIR)/src/ops/op_tog.c \
	$(APP_DIR)/src/ops/op_ww.c \
	$(APP_DIR)/src/pages/page_dsp.c \
	$(APP_DIR)/src/pages/page_gathered.c \
	$(APP_DIR)/src/pages/page_ins.c \
	$(APP_DIR)/src/pages/page_ops.c \
	$(APP_DIR)/src/pages/page_outs.c \
	$(APP_DIR)/src/pages/page_play.c \
	$(APP_DIR)/src/pages/page_presets.c \
	$(APP_DIR)/src/pages/page_scenes.c \
	$(APP_DIR)/src/scalers/scaler_amp.c \
	$(APP_DIR)/src/scalers/scaler_bool.c \
	$(APP_DIR)/src/scalers/scaler_fix.c \
	$(APP_DIR)/src/scalers/scaler_fract.c \
	$(APP_DIR)/src/scalers/scaler_integrator.c \
	$(APP_DIR)/src/scalers/scaler_integrator_short.c \
	$(APP_DIR)/src/scalers/scaler_note.c \
	$(APP_DIR)/src/scalers/scaler_short.c \
	$(APP_DIR)/src/scalers/scaler_svf_fc.c

# List of assembler source files.
ASSRCS +=

# List of include paths.	
INC_PATH += \
	$(APP_DIR)	           \
	$(APP_DIR)/src \
	$(APP_DIR)/../../common \
	$(APP_DIR)/../../../avr32-toolchain-linux/include

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
