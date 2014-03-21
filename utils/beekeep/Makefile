name = beekeep

bees = ../../apps/bees
sim = ../avr32_sim


include $(bees)/version.mk
version = $(maj).$(min).$(rev)
target = $(name)-$(version)

# bees sources
src += \
	$(bees)/src/app_timers.c \
	$(bees)/src/net.c \
	$(bees)/src/net_midi.c \
	$(bees)/src/net_monome.c \
	$(bees)/src/net_poll.c \
	$(bees)/src/op.c \
	$(bees)/src/op_gfx.c \
	$(bees)/src/op_math.c \
	$(bees)/src/ops/op_list16.c \
	$(bees)/src/param.c \
	$(bees)/src/pages.c \
	$(bees)/src/pickle.c \
	$(bees)/src/play.c \
	$(bees)/src/preset.c \
	$(bees)/src/render.c \
	$(bees)/src/param_scaler.c \
	$(bees)/src/scene.c \
	$(bees)/src/util.c \
	$(bees)/src/ops/op_add.c \
	$(bees)/src/ops/op_accum.c \
	$(bees)/src/ops/op_adc.c \
	$(bees)/src/ops/op_bignum.c \
	$(bees)/src/ops/op_bits.c \
	$(bees)/src/ops/op_delay.c \
	$(bees)/src/ops/op_div.c \
	$(bees)/src/ops/op_enc.c \
	$(bees)/src/ops/op_gate.c \
	$(bees)/src/ops/op_history.c \
	$(bees)/src/ops/op_is.c \
	$(bees)/src/ops/op_life.c \
	$(bees)/src/ops/op_list2.c \
	$(bees)/src/ops/op_list8.c \
	$(bees)/src/ops/op_logic.c \
	$(bees)/src/ops/op_metro.c \
	$(bees)/src/ops/op_midi_cc.c \
	$(bees)/src/ops/op_midi_note.c \
	$(bees)/src/ops/op_midi_out_note.c \
	$(bees)/src/ops/op_mul.c \
	$(bees)/src/ops/op_monome_grid_raw.c \
	$(bees)/src/ops/op_preset.c \
	$(bees)/src/ops/op_screen.c \
	$(bees)/src/ops/op_split.c \
	$(bees)/src/ops/op_split4.c \
	$(bees)/src/ops/op_sub.c \
	$(bees)/src/ops/op_sw.c \
	$(bees)/src/ops/op_timer.c \
	$(bees)/src/ops/op_thresh.c \
	$(bees)/src/ops/op_tog.c \
	$(bees)/src/ops/op_random.c \
	$(bees)/src/ops/op_route.c \
	$(bees)/src/ops/op_mod.c \
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
	$(bees)/src/scalers/scaler_integrator.c \
	$(bees)/src/scalers/scaler_note.c \
	$(bees)/src/scalers/scaler_svf_fc.c

# "fake" sources
src += 	src/app_beekeep.c \
	src/flash_beekeep.c \
	src/files.c \
	src/handler.c \
	src/main.c	

# new sources
src += src/json_read_raw.c \
	src/json_write_raw.c \
	src/json_read_max.c \
	src/json_write_max.c \
	src/json_read_native.c \
	src/json_write_native.c \


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


ifdef BUILD_DIR
	build-dir       := $(dir $(BUILD_DIR))$(if $(notdir $(BUILD_DIR)),$(notdir $(BUILD_DIR))/)
else
	build-dir        =
endif

obj = $(addprefix $(build-dir), $(addsuffix .o,$(basename $(src))))

cflags += $(foreach path,$(inc),-I$(path))
cflags += -std=gnu99
cflags += -D BEEKEEP=1
$(build-dir)%.o: %.c 
	echo $(cflags)
	$(Q)test -d $(dir $@) || mkdir -p $(dir $@)
	gcc $(cflags) -g -c $< -o $@

target: $(obj)	
	echo $(obj)
	gcc $(obj) -g $(cflags) -o $(target) -ljansson

all: beekeep

clean:
	rm $(obj)
