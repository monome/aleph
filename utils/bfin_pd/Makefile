# Makefile for bfin~

lib.name = bfin~

class.sources = bfin~.c

inc =   ../../dsp \
	../../utils/bfin_sim/src \
	../../utils/bfin_sim/ \
	../../utils/bfin_sim/src/libfixmath \
	./ \
	../../common

bfin_emu_src += \
	../../utils/bfin_sim/fract2float_conv.c \
	../../utils/bfin_sim/src/cv.c \
	../../utils/bfin_sim/fract_math.c \
	../../utils/bfin_sim/src/fix16.c

bees_scaler_src = op_math.c \
	param_scaler.c \
	fix.c \
	scalers/scaler_amp.c \
	scalers/scaler_integrator.c \
	scalers/scaler_short.c \
	scalers/scaler_bool.c \
	scalers/scaler_integrator_short.c \
	scalers/scaler_svf_fc.c \
	scalers/scaler_fix.c \
	scalers/scaler_label.c \
	scalers/scaler_fract.c \
	scalers/scaler_note.c \


module_src = ../../modules/fmsynth/fmsynth.c \
	../../modules/fmsynth/params.c \
	../../dsp/filter_1p.c \
	../../dsp/filter_svf.c \
	../../dsp/ricks_tricks.c \
	../../dsp/biquad.c \
	../../dsp/env_adsr.c \
	../../dsp/osc_polyblep.c \
	../../dsp/fm_voice.c \

module_inc = ../../modules/fmsynth

inc += $(module_inc)


cflags += $(foreach path,$(inc),-I$(path))
cflags += -D ARCH_LINUX=1

common.sources = $(bfin_emu_src) $(module_src) $(bees_scaler_src)

#datafiles = myclass1-help.pd myclass2-help.pd README.txt LICENSE.txt
PDINCLUDEDIR=/usr/local/include/pd

include Makefile.pdlibbuilder

dbg:
	echo $(inc)
