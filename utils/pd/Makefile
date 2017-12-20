sim = ../../utils/avr32_sim
bees = ../../apps/bees
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
	$(sim)/src/usb/hub

beekeep_src += \
	$(bees)/src/app_timers.c \
	$(bees)/src/net.c \
	$(bees)/src/net_hid.c \
	$(bees)/src/net_midi.c \
	$(bees)/src/net_monome.c \
	$(bees)/src/net_poll.c \
	$(bees)/src/op.c \
	$(bees)/src/op_gfx.c \
	$(bees)/src/op_math.c \
	$(bees)/src/op_pool.c \
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
	$(bees)/src/ops/op_ckdiv.c \
	$(bees)/src/ops/op_delay.c \
	$(bees)/src/ops/op_div.c \
	$(bees)/src/ops/op_divr.c \
	$(bees)/src/ops/op_enc.c \
	$(bees)/src/ops/op_fade.c \
	$(bees)/src/ops/op_gate.c \
	$(bees)/src/ops/op_harry.c \
	$(bees)/src/ops/op_hid_word.c \
	$(bees)/src/ops/op_history.c \
	$(bees)/src/ops/op_is.c \
	$(bees)/src/ops/op_iter.c \
	$(bees)/src/ops/op_kria.c \
	$(bees)/src/ops/op_life_classic.c \
	$(bees)/src/ops/op_linlin.c \
	$(bees)/src/ops/op_list2.c \
	$(bees)/src/ops/op_list4.c \
	$(bees)/src/ops/op_list8.c \
	$(bees)/src/ops/op_list16.c \
	$(bees)/src/ops/op_logic.c \
	$(bees)/src/ops/op_maginc.c \
	$(bees)/src/ops/op_metro.c \
	$(bees)/src/ops/op_midi_cc.c \
	$(bees)/src/ops/op_midi_prog.c \
	$(bees)/src/ops/op_midi_clock.c \
	$(bees)/src/ops/op_midi_note.c \
	$(bees)/src/ops/op_midi_out_cc.c \
	$(bees)/src/ops/op_midi_out_note.c \
	$(bees)/src/ops/op_midi_out_clock.c \
	$(bees)/src/ops/op_mul.c \
	$(bees)/src/ops/op_monome_grid_raw.c \
	$(bees)/src/ops/op_monome_grid_classic.c \
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
	$(bees)/src/ops/op_mem0d.c \
	$(bees)/src/ops/op_mem1d.c \
	$(bees)/src/ops/op_mem2d.c \
	$(bees)/src/ops/op_mod.c \
	$(bees)/src/ops/op_ww.c \
	$(bees)/src/ops/op_param.c \
	$(bees)/src/ops/op_poly.c \
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

beekeep_src += \
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
	$(sim)/src/usb/hid/hid.c \
	$(sim)/src/fonts/ume_tgo5_18.c \
	$(sim)/src/fix.c \
	$(sim)/src/libfixmath/fix16.c \

beekeep_root = ../beekeep
# "fake" sources
beekeep_src += 	app_pd.c \
	flash_beekeep.c \
	handler.c \
	param_scaler.c \
	files.c 


# new sources
beekeep_src += 	dot.c \
	ui_files.c
#	ui_op_menu.c \
#	ui.c \
#	ui_handlers.c \
#	ui_lists.c \
#	json_read_native.c \
#	json_write_native.c \

lib.name = bees_op
cflags = -D BEEKEEP -D PD -fno-common -g
ldflags = -llo -fno-common -g
common.sources =  $(beekeep_src)
class.sources = bees_op.c

#datafiles = myclass1-help.pd myclass2-help.pd README.txt LICENSE.txt
PDINCLUDEDIR=/usr/local/include/pd
cflags+= -I./dummy_include -I../../common/ -I../../apps/bees/src/ops/ -I../../apps/bees/src -I../../utils/avr32_sim/src/
cflags += $(foreach path,$(inc),-I$(path))
include Makefile.pdlibbuilder

tags:
	find . ../avr32_sim/src ../../common ../../apps/bees ~/git_checkouts/pure-data -name '*.[ch]' | etags -

test:
	cp bees_op.pd_linux ~/pd-externals; pd bees_ops_grid.pd
