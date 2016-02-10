BEES = ../../apps/bees
SIM = ../avr32_sim
JUCE_LIB = juce_lib
JUCE_SRC = juce_src

include $(BEES)/version.mk
VERSION = $(maj).$(min).$(rev)
TARGET = $(name)-$(version)

# bees sources
# fixme: move some of these to sim sources
# instead of switching on BEEKEEP in the production code
SRC += \
	$(BEES)/src/app_timers.c \
	$(BEES)/src/net.c \
	$(BEES)/src/net_midi.c \
	$(BEES)/src/net_monome.c \
	$(BEES)/src/net_poll.c \
	$(BEES)/src/op.c \
	$(BEES)/src/op_gfx.c \
	$(BEES)/src/op_math.c \
	$(BEES)/src/param.c \
	$(BEES)/src/pages.c \
	$(BEES)/src/pickle.c \
	$(BEES)/src/play.c \
	$(BEES)/src/preset.c \
	$(BEES)/src/render.c \
	$(BEES)/src/scene.c \
	$(BEES)/src/util.c \
	$(BEES)/src/ops/op_add.c \
	$(BEES)/src/ops/op_accum.c \
	$(BEES)/src/ops/op_adc.c \
	$(BEES)/src/ops/op_bars.c \
	$(BEES)/src/ops/op_bars8.c \
	$(BEES)/src/ops/op_bignum.c \
	$(BEES)/src/ops/op_bits.c \
	$(BEES)/src/ops/op_cascades.c \
	$(BEES)/src/ops/op_change.c \
	$(BEES)/src/ops/op_delay.c \
	$(BEES)/src/ops/op_div.c \
	$(BEES)/src/ops/op_divr.c \
	$(BEES)/src/ops/op_enc.c \
	$(BEES)/src/ops/op_fade.c \
	$(BEES)/src/ops/op_gate.c \
	$(BEES)/src/ops/op_hid_word.c \
	$(BEES)/src/ops/op_history.c \
	$(BEES)/src/ops/op_is.c \
	$(BEES)/src/ops/op_life.c \
	$(BEES)/src/ops/op_list2.c \
	$(BEES)/src/ops/op_list8.c \
	$(BEES)/src/ops/op_list16.c \
	$(BEES)/src/ops/op_logic.c \
	$(BEES)/src/ops/op_metro.c \
	$(BEES)/src/ops/op_midi_cc.c \
	$(BEES)/src/ops/op_midi_note.c \
	$(BEES)/src/ops/op_midi_out_cc.c \
	$(BEES)/src/ops/op_midi_out_note.c \
	$(BEES)/src/ops/op_mul.c \
	$(BEES)/src/ops/op_monome_grid_raw.c \
	$(BEES)/src/ops/op_monome_arc.c \
	$(BEES)/src/ops/op_preset.c \
	$(BEES)/src/ops/op_screen.c \
	$(BEES)/src/ops/op_serial.c \
	$(BEES)/src/ops/op_shl.c \
	$(BEES)/src/ops/op_shr.c \
	$(BEES)/src/ops/op_split.c \
	$(BEES)/src/ops/op_split4.c \
	$(BEES)/src/ops/op_step.c \
	$(BEES)/src/ops/op_sub.c \
	$(BEES)/src/ops/op_sw.c \
	$(BEES)/src/ops/op_timer.c \
	$(BEES)/src/ops/op_thresh.c \
	$(BEES)/src/ops/op_tog.c \
	$(BEES)/src/ops/op_random.c \
	$(BEES)/src/ops/op_route.c \
	$(BEES)/src/ops/op_route8.c \
	$(BEES)/src/ops/op_route16.c \
	$(BEES)/src/ops/op_mod.c \
	$(BEES)/src/ops/op_ww.c \
	$(BEES)/src/pages/page_dsp.c \
	$(BEES)/src/pages/page_gathered.c \
	$(BEES)/src/pages/page_ins.c \
	$(BEES)/src/pages/page_ops.c \
	$(BEES)/src/pages/page_outs.c \
	$(BEES)/src/pages/page_play.c \
	$(BEES)/src/pages/page_presets.c \
	$(BEES)/src/pages/page_scenes.c \
	$(BEES)/src/scalers/scaler_amp.c \
	$(BEES)/src/scalers/scaler_bool.c \
	$(BEES)/src/scalers/scaler_fix.c \
	$(BEES)/src/scalers/scaler_fract.c \
	$(BEES)/src/scalers/scaler_integrator.c \
	$(BEES)/src/scalers/scaler_integrator_short.c \
	$(BEES)/src/scalers/scaler_note.c \
	$(BEES)/src/scalers/scaler_short.c \
	$(BEES)/src/scalers/scaler_svf_fc.c

# "fake" sources
SRC += 	src/app_beekeep.c \
	src/flash_beekeep.c \
	src/files.c \
	src/handler.c \
	src/param_scaler.c

# new sources
# SRC += 	src/dot.c \
	src/json_read_native.c \
	src/json_write_native.c

# srcs from simulator
SRC += \
	$(SIM)/src/adc.c \
	$(SIM)/src/app.c \
	$(SIM)/src/bfin.c \
	$(SIM)/src/control.c \
	$(SIM)/src/delay.c \
	$(SIM)/src/events.c \
	$(SIM)/src/encoders.c \
	$(SIM)/src/filesystem.c \
	$(SIM)/src/flash.c \
	$(SIM)/src/font.c \
	$(SIM)/src/i2c.c \
	$(SIM)/src/init.c	\
	$(SIM)/src/interrupts.c \
	$(SIM)/src/memory.c \
	$(SIM)/src/monome.c\
	$(SIM)/src/print_funcs.c \
	$(SIM)/src/region.c \
	$(SIM)/src/screen.c \
	$(SIM)/src/serial.c \
	$(SIM)/src/simple_string.c \
	$(SIM)/src/switches.c \
	$(SIM)/src/timers.c \
	$(SIM)/src/usb.c \
	$(SIM)/src/usb/midi/midi.c \
	$(SIM)/src/usb/ftdi/ftdi.c \
	$(SIM)/src/fonts/ume_tgo5_18.c \
	$(SIM)/src/fix.c \
	$(SIM)/src/libfixmath/fix16.c

# juce library sources
SRC_CPP += \
	$(JUCE_LIB)/modules/juce_core/juce_core.cpp \
	$(JUCE_LIB)/modules/juce_data_structures/juce_data_structures.cpp \
	$(JUCE_LIB)/modules/juce_events/juce_events.cpp \
	$(JUCE_LIB)/modules/juce_graphics/juce_graphics.cpp \
	$(JUCE_LIB)/modules/juce_gui_basics/juce_gui_basics.cpp \
	$(JUCE_LIB)/modules/juce_gui_extra/juce_gui_extra.cpp

# juce classes
SRC_CPP += \
	$(JUCE_SRC)/Main.cpp \
	$(JUCE_SRC)/MainComponent.cpp \

#includes
INC = 	-I$(BEES) \
	-I$(BEES)/src \
	-I$(BEES)/../../common \
	-I$(SIM) \
	-I$(SIM)/src \
	-I$(SIM)/src/usb \
	-I$(SIM)/src/usb/midi \
	-I$(SIM)/src/usb/ftdi \
	-I$(SIM)/src/usb/hid \
	-I$(SIM)/src/usb/mouse \
	-I$(SIM)/src/usb/hub \
	-I$(JUCE_LIB) \
	-I$(JUCE_SRC)


OBJ = $(SRC:.c=.o)
OBJ_CPP = $(SRC_CPP:.cpp=.o)

CXX = g++
CC = gcc

CFLAGS += -D BEEKEEP=1
CFLAGS += -g -O0

CXXFLAGS = $(CFLAGS)
CXXFLAGS += -DDEBUG -std=c++0x

LDFLAGS = 

UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		CCFLAGS += -D LINUX
	endif
	ifeq ($(UNAME_S),Darwin)
		CCFLAGS += -D OSX
		# link to OSX core frameworks...
		LDFLAGS += -framework Foundation
		LDFLAGS += -framework CoreFoundation
		LDFLAGS += -framework CoreServices
	endif

all: beekeep

beekeep: $(OBJ) $(OBJ_CPP)
	echo $(CXX) $(OBJ) $(OBJ_CPP) -o $@
	$(CXX) $(OBJ) $(OBJ_CPP) $(LDFLAGS) -o $@

.c.o:
	echo $(CC) $(CFLAGS) $(INC) $< -o $@
	$(CC) $(CFLAGS) $(INC) $< -c -o $@

.cpp.o:
	echo $(CXX) $(CXXFLAGS) $(INC) $< -o $@
	$(CXX) $(CXXFLAGS) $(INC) $< -c -o $@

clean:
	rm src/*.o
	rm $(BEES)/src/*.o
	rm $(SIM)/src/*.o
	rm $(JUCE_SRC)*.o
	rm $(JUCE_LIB)/*.o
	rm ./src/*.o