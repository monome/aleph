name = beekeep

bees = ../../apps/bees
sim = ../avr32_sim

include $(bees)/version.mk
version = $(maj).$(min).$(rev)
target = $(name)-$(version)

#copy the approriate python version file here for your distro
#include beekeep_py.mk

# bees sources
# fixme: move some of these to sim sources
# instead of switching on BEEKEEP in the production code
src += \
	$(bees)/src/app_timers.c \
	$(bees)/src/net.c \
	$(bees)/src/net_midi.c \
	$(bees)/src/net_monome.c \
	$(bees)/src/net_poll.c \
	$(bees)/src/op.c \
	$(bees)/src/op_gfx.c \
	$(bees)/src/op_math.c \
	$(bees)/src/param.c \
	$(bees)/src/pages.c \
	$(bees)/src/pickle.c \
	$(bees)/src/play.c \
	$(bees)/src/preset.c \
	$(bees)/src/render.c \
	$(bees)/src/scene.c \
	$(bees)/src/util.c \
	$(bees)/src/ops/op_add.c \
	$(bees)/src/ops/op_accum.c \
	$(bees)/src/ops/op_adc.c \
	$(bees)/src/ops/op_bars.c \
	$(bees)/src/ops/op_bars8.c \
	$(bees)/src/ops/op_bignum.c \
	$(bees)/src/ops/op_bits.c \
	$(bees)/src/ops/op_cascades.c \
	$(bees)/src/ops/op_change.c \
	$(bees)/src/ops/op_delay.c \
	$(bees)/src/ops/op_div.c \
	$(bees)/src/ops/op_divr.c \
	$(bees)/src/ops/op_enc.c \
	$(bees)/src/ops/op_fade.c \
	$(bees)/src/ops/op_gate.c \
	$(bees)/src/ops/op_hid_word.c \
	$(bees)/src/ops/op_history.c \
	$(bees)/src/ops/op_is.c \
	$(bees)/src/ops/op_life.c \
	$(bees)/src/ops/op_list2.c \
	$(bees)/src/ops/op_list8.c \
	$(bees)/src/ops/op_list16.c \
	$(bees)/src/ops/op_logic.c \
	$(bees)/src/ops/op_metro.c \
	$(bees)/src/ops/op_midi_cc.c \
	$(bees)/src/ops/op_midi_note.c \
	$(bees)/src/ops/op_midi_out_cc.c \
	$(bees)/src/ops/op_midi_out_note.c \
	$(bees)/src/ops/op_mul.c \
	$(bees)/src/ops/op_monome_grid_raw.c \
	$(bees)/src/ops/op_monome_arc.c \
	$(bees)/src/ops/op_preset.c \
	$(bees)/src/ops/op_screen.c \
	$(bees)/src/ops/op_serial.c \
	$(bees)/src/ops/op_shl.c \
	$(bees)/src/ops/op_shr.c \
	$(bees)/src/ops/op_split.c \
	$(bees)/src/ops/op_split4.c \
	$(bees)/src/ops/op_step.c \
	$(bees)/src/ops/op_sub.c \
	$(bees)/src/ops/op_sw.c \
	$(bees)/src/ops/op_timer.c \
	$(bees)/src/ops/op_thresh.c \
	$(bees)/src/ops/op_tog.c \
	$(bees)/src/ops/op_random.c \
	$(bees)/src/ops/op_route.c \
	$(bees)/src/ops/op_route8.c \
	$(bees)/src/ops/op_route16.c \
	$(bees)/src/ops/op_mod.c \
	$(bees)/src/ops/op_ww.c \
	$(bees)/src/pages/page_dsp.c \
	$(bees)/src/pages/page_gathered.c \
	$(bees)/src/pages/page_ins.c \
	$(bees)/src/pages/page_ops.c \
	$(bees)/src/pages/page_outs.c \
	$(bees)/src/pages/page_play.c \
	$(bees)/src/pages/page_presets.c \
	$(bees)/src/pages/page_scenes.c \
	$(bees)/src/scalers/scaler_amp.c \
	$(bees)/src/scalers/scaler_bool.c \
	$(bees)/src/scalers/scaler_fix.c \
	$(bees)/src/scalers/scaler_fract.c \
	$(bees)/src/scalers/scaler_integrator.c \
	$(bees)/src/scalers/scaler_integrator_short.c \
	$(bees)/src/scalers/scaler_note.c \
	$(bees)/src/scalers/scaler_short.c \
	$(bees)/src/scalers/scaler_svf_fc.c

# "fake" sources
src += 	src/app_beekeep.c \
	src/flash_beekeep.c \
	src/files.c \
	src/handler.c \
	src/main.c \
	src/param_scaler.c

# new sources
src += 	src/dot.c \
	src/json_read_native.c \
	src/json_write_native.c \
	src/ui.c \
	src/ui_files.c \
	src/ui_handlers.c \
	src/ui_lists.c \
	src/ui_op_menu.c

# srcs from simulator
src += \
	$(sim)/src/adc.c \
	$(sim)/src/app.c \
	$(sim)/src/bfin.c \
	$(sim)/src/control.c \
	$(sim)/src/delay.c \
	$(sim)/src/events.c \
	$(sim)/src/encoders.c \
	$(sim)/src/filesystem.c \
	$(sim)/src/flash.c \
	$(sim)/src/font.c \
	$(sim)/src/i2c.c \
	$(sim)/src/init.c	\
	$(sim)/src/interrupts.c \
	$(sim)/src/memory.c \
	$(sim)/src/monome.c\
	$(sim)/src/print_funcs.c \
	$(sim)/src/region.c \
	$(sim)/src/screen.c \
	$(sim)/src/serial.c \
	$(sim)/src/simple_string.c \
	$(sim)/src/switches.c \
	$(sim)/src/timers.c \
	$(sim)/src/usb.c \
	$(sim)/src/usb/midi/midi.c \
	$(sim)/src/usb/ftdi/ftdi.c \
	$(sim)/src/fonts/ume_tgo5_18.c \
	$(sim)/src/fix.c \
	$(sim)/src/libfixmath/fix16.c

#includes
inc = 	$(bees) \
	$(bees)/src \
	$(bees)/../../common \
	$(sim) \
	$(sim)/src \
	$(sim)/src/usb \
	$(sim)/src/usb/midi \
	$(sim)/src/usb/ftdi \
	$(sim)/src/usb/hid \
	$(sim)/src/usb/mouse \
	$(sim)/src/usb/hub \
    $(pyaleph)


ifdef BUILD_DIR
	build-dir       := $(dir $(BUILD_DIR))$(if $(notdir $(BUILD_DIR)),$(notdir $(BUILD_DIR))/)
else
	build-dir        =
endif

obj = $(addprefix $(build-dir), $(addsuffix .o,$(basename $(src))))

cflags += $(foreach path,$(inc),-I$(path))
cflags += -std=gnu99
cflags += -D BEEKEEP=1
cflags += -g -O0

lflags += -Bstatic -ljansson

cflags += $(shell pkg-config --cflags gtk+-3.0)
lflags += -Bdynamic $(shell pkg-config --libs gtk+-3.0)


$(build-dir)%.o: %.c 
	$(Q)test -d $(dir $@) || mkdir -p $(dir $@)
	gcc $(cflags) -g -c $< -o $@

target: $(obj)	
	gcc $(obj) -g -o $(target) $(cflags) $(lflags)

all: beekeep

clean:
#	rm $(obj) $(pyaleph)/pyaleph.pyc
	rm $(obj)
