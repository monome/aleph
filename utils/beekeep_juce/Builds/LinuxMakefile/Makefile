# Automatically generated makefile, created by the Introjucer
# Don't edit this file! Your changes will be overwritten when you re-save the Introjucer project!

# (this disables dependency generation if multiple architectures are set)
DEPFLAGS := $(if $(word 2, $(TARGET_ARCH)), , -MMD)

ifndef CONFIG
  CONFIG=Debug
endif

ifeq ($(CONFIG),Debug)
  BINDIR := build
  LIBDIR := build
  OBJDIR := build/intermediate/Debug
  OUTDIR := build

  ifeq ($(TARGET_ARCH),)
    TARGET_ARCH := -march=native
  endif

  CPPFLAGS := $(DEPFLAGS) -D "LINUX=1" -D "DEBUG=1" -D "_DEBUG=1" -D "-D" -D "BEEKEEP=1" -D "JUCER_LINUX_MAKE_6D53C8B4=1" -D "JUCE_APP_VERSION=1.0.0" -D "JUCE_APP_VERSION_HEX=0x10000" -I /usr/include -I /usr/include/freetype2 -I ../../JuceLibraryCode -I ../../JuceLibraryCode/modules
  CFLAGS += $(CPPFLAGS) $(TARGET_ARCH) -g -ggdb -O0
  CXXFLAGS += $(CFLAGS) -std=c++11
  LDFLAGS += $(TARGET_ARCH) -L$(BINDIR) -L$(LIBDIR) -L/usr/X11R6/lib/ -lX11 -lXext -lXinerama -ldl -lfreetype -lpthread -lrt 

  TARGET := beekeep
  BLDCMD = $(CXX) -o $(OUTDIR)/$(TARGET) $(OBJECTS) $(LDFLAGS) $(RESOURCES) $(TARGET_ARCH)
  CLEANCMD = rm -rf $(OUTDIR)/$(TARGET) $(OBJDIR)
endif

ifeq ($(CONFIG),Release)
  BINDIR := build
  LIBDIR := build
  OBJDIR := build/intermediate/Release
  OUTDIR := build

  ifeq ($(TARGET_ARCH),)
    TARGET_ARCH := -march=native
  endif

  CPPFLAGS := $(DEPFLAGS) -D "LINUX=1" -D "NDEBUG=1" -D "-D" -D "BEEKEEP=1" -D "JUCER_LINUX_MAKE_6D53C8B4=1" -D "JUCE_APP_VERSION=1.0.0" -D "JUCE_APP_VERSION_HEX=0x10000" -I /usr/include -I /usr/include/freetype2 -I ../../JuceLibraryCode -I ../../JuceLibraryCode/modules
  CFLAGS += $(CPPFLAGS) $(TARGET_ARCH) -O3
  CXXFLAGS += $(CFLAGS) -std=c++11
  LDFLAGS += $(TARGET_ARCH) -L$(BINDIR) -L$(LIBDIR) -fvisibility=hidden -L/usr/X11R6/lib/ -lX11 -lXext -lXinerama -ldl -lfreetype -lpthread -lrt 

  TARGET := beekeep
  BLDCMD = $(CXX) -o $(OUTDIR)/$(TARGET) $(OBJECTS) $(LDFLAGS) $(RESOURCES) $(TARGET_ARCH)
  CLEANCMD = rm -rf $(OUTDIR)/$(TARGET) $(OBJDIR)
endif

OBJECTS := \
  $(OBJDIR)/GfxUtil_f10ed9fc.o \
  $(OBJDIR)/OpComponent_79d4687d.o \
  $(OBJDIR)/OpGraph_a51ed9ce.o \
  $(OBJDIR)/flash_beekeep_c1a02d79.o \
  $(OBJDIR)/app_beekeep_3c46f16a.o \
  $(OBJDIR)/files_7f070278.o \
  $(OBJDIR)/handler_2a9df38b.o \
  $(OBJDIR)/fixtest_f823ce78.o \
  $(OBJDIR)/fract32_emu_a0430204.o \
  $(OBJDIR)/adc_7d562540.o \
  $(OBJDIR)/app_7d5bca81.o \
  $(OBJDIR)/bfin_2f432ed3.o \
  $(OBJDIR)/control_4cd6893d.o \
  $(OBJDIR)/delay_215fbaa3.o \
  $(OBJDIR)/encoders_aafef41.o \
  $(OBJDIR)/events_ea86b243.o \
  $(OBJDIR)/filesystem_439e4cd5.o \
  $(OBJDIR)/fix_7d9f2f75.o \
  $(OBJDIR)/flash_968b09d0.o \
  $(OBJDIR)/font_3697c139.o \
  $(OBJDIR)/dejavu_numerals_24_8b8f24f9.o \
  $(OBJDIR)/ume_tgo5_18_45751130.o \
  $(OBJDIR)/i2c_7db026ba.o \
  $(OBJDIR)/init_3ba5edba.o \
  $(OBJDIR)/interrupts_e81b35a.o \
  $(OBJDIR)/fix16_46f2e30d.o \
  $(OBJDIR)/fix16_exp_517270eb.o \
  $(OBJDIR)/fix16_sqrt_f45eca1c.o \
  $(OBJDIR)/fix16_trig_f61d7218.o \
  $(OBJDIR)/fix32_46f3bcc7.o \
  $(OBJDIR)/fract32_94bf28f8.o \
  $(OBJDIR)/uint32_a0397c10.o \
  $(OBJDIR)/memory_b3fa342b.o \
  $(OBJDIR)/monome_c6aacc65.o \
  $(OBJDIR)/print_funcs_f8543c7d.o \
  $(OBJDIR)/region_b0cced5e.o \
  $(OBJDIR)/screen_c16a08d6.o \
  $(OBJDIR)/serial_2b6d045e.o \
  $(OBJDIR)/simple_string_a5eca29e.o \
  $(OBJDIR)/switches_1351c08c.o \
  $(OBJDIR)/timers_5e2e59b8.o \
  $(OBJDIR)/ftdi_4c1b4ba4.o \
  $(OBJDIR)/hid_b246180c.o \
  $(OBJDIR)/midi_ea8b3c64.o \
  $(OBJDIR)/usb_7e76c924.o \
  $(OBJDIR)/app_timers_b963c46d.o \
  $(OBJDIR)/net_7be9e06.o \
  $(OBJDIR)/net_hid_ffa94f4a.o \
  $(OBJDIR)/net_midi_fe09ac84.o \
  $(OBJDIR)/net_monome_f137b31e.o \
  $(OBJDIR)/net_poll_38071a2.o \
  $(OBJDIR)/op_d6f63762.o \
  $(OBJDIR)/op_gfx_cc6db11c.o \
  $(OBJDIR)/op_math_cb3e12cf.o \
  $(OBJDIR)/op_accum_22058a79.o \
  $(OBJDIR)/op_adc_87e54140.o \
  $(OBJDIR)/op_add_87e54501.o \
  $(OBJDIR)/op_bars_7655476a.o \
  $(OBJDIR)/op_bars8_5453c558.o \
  $(OBJDIR)/op_bignum_ca679c70.o \
  $(OBJDIR)/op_bits_76c6ec30.o \
  $(OBJDIR)/op_cascades_973b4aa9.o \
  $(OBJDIR)/op_change_f31eff1a.o \
  $(OBJDIR)/op_delay_c497d6a3.o \
  $(OBJDIR)/op_div_881214f1.o \
  $(OBJDIR)/op_divr_7a31826b.o \
  $(OBJDIR)/op_enc_88222afa.o \
  $(OBJDIR)/op_fade_7d221826.o \
  $(OBJDIR)/op_gate_7ede36b5.o \
  $(OBJDIR)/op_hid_word_3b2ebaf0.o \
  $(OBJDIR)/op_history_80dd32b4.o \
  $(OBJDIR)/op_is_3e348494.o \
  $(OBJDIR)/op_life_87d0d0a6.o \
  $(OBJDIR)/op_list2_73075034.o \
  $(OBJDIR)/op_list8_730766ba.o \
  $(OBJDIR)/op_list16_ede259ed.o \
  $(OBJDIR)/op_logic_7c9701be.o \
  $(OBJDIR)/op_metro_a1283cb9.o \
  $(OBJDIR)/op_midi_cc_3907f9de.o \
  $(OBJDIR)/op_midi_note_2a7f42b0.o \
  $(OBJDIR)/op_midi_out_cc_2cc377af.o \
  $(OBJDIR)/op_midi_out_note_1d529041.o \
  $(OBJDIR)/op_mod_88935f22.o \
  $(OBJDIR)/op_monome_arc_5d240938.o \
  $(OBJDIR)/op_monome_grid_raw_45ba1f13.o \
  $(OBJDIR)/op_mul_88963764.o \
  $(OBJDIR)/op_param_38e39b2d.o \
  $(OBJDIR)/op_preset_51a5be89.o \
  $(OBJDIR)/op_random_acaad76d.o \
  $(OBJDIR)/op_route_bac6b5e9.o \
  $(OBJDIR)/op_route8_9e1026b9.o \
  $(OBJDIR)/op_route16_23f199ce.o \
  $(OBJDIR)/op_screen_85356cd6.o \
  $(OBJDIR)/op_serial_ef38685e.o \
  $(OBJDIR)/op_shl_88e4db97.o \
  $(OBJDIR)/op_shr_88e4f21d.o \
  $(OBJDIR)/op_split_f0de2e7a.o \
  $(OBJDIR)/op_split4_2ae7b144.o \
  $(OBJDIR)/op_step_945d7476.o \
  $(OBJDIR)/op_sub_88ea9ee0.o \
  $(OBJDIR)/op_sw_3e391f4e.o \
  $(OBJDIR)/op_thresh_f59c08a6.o \
  $(OBJDIR)/op_timer_19debbe5.o \
  $(OBJDIR)/op_tog_88f60eec.o \
  $(OBJDIR)/op_ww_3e3af0ca.o \
  $(OBJDIR)/page_dsp_7abe62bd.o \
  $(OBJDIR)/page_gathered_1f0d0068.o \
  $(OBJDIR)/page_ins_7b029daa.o \
  $(OBJDIR)/page_ops_7b58136e.o \
  $(OBJDIR)/page_outs_eff24133.o \
  $(OBJDIR)/page_play_f11fb9c2.o \
  $(OBJDIR)/page_presets_16a53a30.o \
  $(OBJDIR)/page_scenes_89f564f5.o \
  $(OBJDIR)/pages_74ea596d.o \
  $(OBJDIR)/param_758373f6.o \
  $(OBJDIR)/param_scaler_da82dc9b.o \
  $(OBJDIR)/pickle_c911729b.o \
  $(OBJDIR)/play_f3d9e8f5.o \
  $(OBJDIR)/preset_a900fee0.o \
  $(OBJDIR)/render_d79a8bd7.o \
  $(OBJDIR)/scaler_amp_517ec272.o \
  $(OBJDIR)/scaler_bool_e02b01c6.o \
  $(OBJDIR)/scaler_fix_51c38483.o \
  $(OBJDIR)/scaler_fract_fd244e34.o \
  $(OBJDIR)/scaler_integrator_e37c6cf3.o \
  $(OBJDIR)/scaler_integrator_short_964cb2b0.o \
  $(OBJDIR)/scaler_note_f4a754ce.o \
  $(OBJDIR)/scaler_short_9c90f4ea.o \
  $(OBJDIR)/scaler_svf_fc_c9b7ed75.o \
  $(OBJDIR)/scene_16ee7035.o \
  $(OBJDIR)/util_fcd65243.o \
  $(OBJDIR)/Main_90ebc5c2.o \
  $(OBJDIR)/GraphEditorComponent_693cb26b.o \
  $(OBJDIR)/BinaryData_ce4232d4.o \
  $(OBJDIR)/juce_core_aff681cc.o \
  $(OBJDIR)/juce_data_structures_bdd6d488.o \
  $(OBJDIR)/juce_events_79b2840.o \
  $(OBJDIR)/juce_graphics_c8f1e7a4.o \
  $(OBJDIR)/juce_gui_basics_a630dd20.o \
  $(OBJDIR)/juce_gui_extra_7767d6a8.o \

.PHONY: clean

$(OUTDIR)/$(TARGET): $(OBJECTS) $(RESOURCES)
	@echo Linking beekeep
	-@mkdir -p $(BINDIR)
	-@mkdir -p $(LIBDIR)
	-@mkdir -p $(OUTDIR)
	@$(BLDCMD)

clean:
	@echo Cleaning beekeep
	@$(CLEANCMD)

strip:
	@echo Stripping beekeep
	-@strip --strip-unneeded $(OUTDIR)/$(TARGET)

$(OBJDIR)/GfxUtil_f10ed9fc.o: ../../Source/GfxUtil.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling GfxUtil.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/OpComponent_79d4687d.o: ../../Source/OpComponent.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling OpComponent.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/OpGraph_a51ed9ce.o: ../../Source/OpGraph.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling OpGraph.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/flash_beekeep_c1a02d79.o: ../../Source/flash_beekeep.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling flash_beekeep.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/app_beekeep_3c46f16a.o: ../../Source/app_beekeep.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling app_beekeep.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/files_7f070278.o: ../../Source/files.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling files.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/handler_2a9df38b.o: ../../Source/handler.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling handler.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/fixtest_f823ce78.o: ../../../../common/fixtest.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling fixtest.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/fract32_emu_a0430204.o: ../../../../common/fract32_emu.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling fract32_emu.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/adc_7d562540.o: ../../../avr32_sim/src/adc.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling adc.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/app_7d5bca81.o: ../../../avr32_sim/src/app.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling app.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/bfin_2f432ed3.o: ../../../avr32_sim/src/bfin.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling bfin.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/control_4cd6893d.o: ../../../avr32_sim/src/control.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling control.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/delay_215fbaa3.o: ../../../avr32_sim/src/delay.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling delay.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/encoders_aafef41.o: ../../../avr32_sim/src/encoders.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling encoders.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/events_ea86b243.o: ../../../avr32_sim/src/events.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling events.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/filesystem_439e4cd5.o: ../../../avr32_sim/src/filesystem.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling filesystem.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/fix_7d9f2f75.o: ../../../avr32_sim/src/fix.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling fix.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/flash_968b09d0.o: ../../../avr32_sim/src/flash.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling flash.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/font_3697c139.o: ../../../avr32_sim/src/font.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling font.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/dejavu_numerals_24_8b8f24f9.o: ../../../avr32_sim/src/fonts/dejavu_numerals_24.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling dejavu_numerals_24.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/ume_tgo5_18_45751130.o: ../../../avr32_sim/src/fonts/ume_tgo5_18.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling ume_tgo5_18.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/i2c_7db026ba.o: ../../../avr32_sim/src/i2c.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling i2c.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/init_3ba5edba.o: ../../../avr32_sim/src/init.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling init.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/interrupts_e81b35a.o: ../../../avr32_sim/src/interrupts.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling interrupts.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/fix16_46f2e30d.o: ../../../avr32_sim/src/libfixmath/fix16.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling fix16.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/fix16_exp_517270eb.o: ../../../avr32_sim/src/libfixmath/fix16_exp.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling fix16_exp.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/fix16_sqrt_f45eca1c.o: ../../../avr32_sim/src/libfixmath/fix16_sqrt.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling fix16_sqrt.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/fix16_trig_f61d7218.o: ../../../avr32_sim/src/libfixmath/fix16_trig.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling fix16_trig.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/fix32_46f3bcc7.o: ../../../avr32_sim/src/libfixmath/fix32.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling fix32.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/fract32_94bf28f8.o: ../../../avr32_sim/src/libfixmath/fract32.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling fract32.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/uint32_a0397c10.o: ../../../avr32_sim/src/libfixmath/uint32.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling uint32.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/memory_b3fa342b.o: ../../../avr32_sim/src/memory.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling memory.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/monome_c6aacc65.o: ../../../avr32_sim/src/monome.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling monome.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/print_funcs_f8543c7d.o: ../../../avr32_sim/src/print_funcs.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling print_funcs.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/region_b0cced5e.o: ../../../avr32_sim/src/region.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling region.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/screen_c16a08d6.o: ../../../avr32_sim/src/screen.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling screen.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/serial_2b6d045e.o: ../../../avr32_sim/src/serial.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling serial.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/simple_string_a5eca29e.o: ../../../avr32_sim/src/simple_string.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling simple_string.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/switches_1351c08c.o: ../../../avr32_sim/src/switches.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling switches.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/timers_5e2e59b8.o: ../../../avr32_sim/src/timers.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling timers.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/ftdi_4c1b4ba4.o: ../../../avr32_sim/src/usb/ftdi/ftdi.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling ftdi.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/hid_b246180c.o: ../../../avr32_sim/src/usb/hid/hid.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling hid.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/midi_ea8b3c64.o: ../../../avr32_sim/src/usb/midi/midi.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling midi.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/usb_7e76c924.o: ../../../avr32_sim/src/usb.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling usb.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/app_timers_b963c46d.o: ../../../../apps/bees/src/app_timers.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling app_timers.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/net_7be9e06.o: ../../../../apps/bees/src/net.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling net.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/net_hid_ffa94f4a.o: ../../../../apps/bees/src/net_hid.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling net_hid.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/net_midi_fe09ac84.o: ../../../../apps/bees/src/net_midi.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling net_midi.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/net_monome_f137b31e.o: ../../../../apps/bees/src/net_monome.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling net_monome.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/net_poll_38071a2.o: ../../../../apps/bees/src/net_poll.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling net_poll.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_d6f63762.o: ../../../../apps/bees/src/op.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_gfx_cc6db11c.o: ../../../../apps/bees/src/op_gfx.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_gfx.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_math_cb3e12cf.o: ../../../../apps/bees/src/op_math.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_math.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_accum_22058a79.o: ../../../../apps/bees/src/ops/op_accum.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_accum.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_adc_87e54140.o: ../../../../apps/bees/src/ops/op_adc.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_adc.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_add_87e54501.o: ../../../../apps/bees/src/ops/op_add.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_add.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_bars_7655476a.o: ../../../../apps/bees/src/ops/op_bars.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_bars.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_bars8_5453c558.o: ../../../../apps/bees/src/ops/op_bars8.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_bars8.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_bignum_ca679c70.o: ../../../../apps/bees/src/ops/op_bignum.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_bignum.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_bits_76c6ec30.o: ../../../../apps/bees/src/ops/op_bits.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_bits.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_cascades_973b4aa9.o: ../../../../apps/bees/src/ops/op_cascades.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_cascades.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_change_f31eff1a.o: ../../../../apps/bees/src/ops/op_change.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_change.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_delay_c497d6a3.o: ../../../../apps/bees/src/ops/op_delay.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_delay.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_div_881214f1.o: ../../../../apps/bees/src/ops/op_div.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_div.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_divr_7a31826b.o: ../../../../apps/bees/src/ops/op_divr.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_divr.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_enc_88222afa.o: ../../../../apps/bees/src/ops/op_enc.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_enc.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_fade_7d221826.o: ../../../../apps/bees/src/ops/op_fade.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_fade.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_gate_7ede36b5.o: ../../../../apps/bees/src/ops/op_gate.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_gate.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_hid_word_3b2ebaf0.o: ../../../../apps/bees/src/ops/op_hid_word.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_hid_word.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_history_80dd32b4.o: ../../../../apps/bees/src/ops/op_history.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_history.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_is_3e348494.o: ../../../../apps/bees/src/ops/op_is.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_is.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_life_87d0d0a6.o: ../../../../apps/bees/src/ops/op_life.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_life.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_list2_73075034.o: ../../../../apps/bees/src/ops/op_list2.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_list2.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_list8_730766ba.o: ../../../../apps/bees/src/ops/op_list8.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_list8.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_list16_ede259ed.o: ../../../../apps/bees/src/ops/op_list16.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_list16.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_logic_7c9701be.o: ../../../../apps/bees/src/ops/op_logic.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_logic.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_metro_a1283cb9.o: ../../../../apps/bees/src/ops/op_metro.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_metro.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_midi_cc_3907f9de.o: ../../../../apps/bees/src/ops/op_midi_cc.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_midi_cc.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_midi_note_2a7f42b0.o: ../../../../apps/bees/src/ops/op_midi_note.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_midi_note.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_midi_out_cc_2cc377af.o: ../../../../apps/bees/src/ops/op_midi_out_cc.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_midi_out_cc.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_midi_out_note_1d529041.o: ../../../../apps/bees/src/ops/op_midi_out_note.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_midi_out_note.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_mod_88935f22.o: ../../../../apps/bees/src/ops/op_mod.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_mod.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_monome_arc_5d240938.o: ../../../../apps/bees/src/ops/op_monome_arc.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_monome_arc.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_monome_grid_raw_45ba1f13.o: ../../../../apps/bees/src/ops/op_monome_grid_raw.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_monome_grid_raw.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_mul_88963764.o: ../../../../apps/bees/src/ops/op_mul.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_mul.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_param_38e39b2d.o: ../../../../apps/bees/src/ops/op_param.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_param.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_preset_51a5be89.o: ../../../../apps/bees/src/ops/op_preset.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_preset.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_random_acaad76d.o: ../../../../apps/bees/src/ops/op_random.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_random.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_route_bac6b5e9.o: ../../../../apps/bees/src/ops/op_route.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_route.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_route8_9e1026b9.o: ../../../../apps/bees/src/ops/op_route8.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_route8.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_route16_23f199ce.o: ../../../../apps/bees/src/ops/op_route16.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_route16.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_screen_85356cd6.o: ../../../../apps/bees/src/ops/op_screen.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_screen.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_serial_ef38685e.o: ../../../../apps/bees/src/ops/op_serial.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_serial.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_shl_88e4db97.o: ../../../../apps/bees/src/ops/op_shl.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_shl.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_shr_88e4f21d.o: ../../../../apps/bees/src/ops/op_shr.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_shr.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_split_f0de2e7a.o: ../../../../apps/bees/src/ops/op_split.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_split.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_split4_2ae7b144.o: ../../../../apps/bees/src/ops/op_split4.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_split4.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_step_945d7476.o: ../../../../apps/bees/src/ops/op_step.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_step.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_sub_88ea9ee0.o: ../../../../apps/bees/src/ops/op_sub.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_sub.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_sw_3e391f4e.o: ../../../../apps/bees/src/ops/op_sw.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_sw.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_thresh_f59c08a6.o: ../../../../apps/bees/src/ops/op_thresh.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_thresh.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_timer_19debbe5.o: ../../../../apps/bees/src/ops/op_timer.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_timer.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_tog_88f60eec.o: ../../../../apps/bees/src/ops/op_tog.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_tog.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/op_ww_3e3af0ca.o: ../../../../apps/bees/src/ops/op_ww.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling op_ww.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/page_dsp_7abe62bd.o: ../../../../apps/bees/src/pages/page_dsp.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling page_dsp.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/page_gathered_1f0d0068.o: ../../../../apps/bees/src/pages/page_gathered.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling page_gathered.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/page_ins_7b029daa.o: ../../../../apps/bees/src/pages/page_ins.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling page_ins.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/page_ops_7b58136e.o: ../../../../apps/bees/src/pages/page_ops.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling page_ops.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/page_outs_eff24133.o: ../../../../apps/bees/src/pages/page_outs.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling page_outs.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/page_play_f11fb9c2.o: ../../../../apps/bees/src/pages/page_play.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling page_play.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/page_presets_16a53a30.o: ../../../../apps/bees/src/pages/page_presets.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling page_presets.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/page_scenes_89f564f5.o: ../../../../apps/bees/src/pages/page_scenes.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling page_scenes.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/pages_74ea596d.o: ../../../../apps/bees/src/pages.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling pages.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/param_758373f6.o: ../../../../apps/bees/src/param.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling param.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/param_scaler_da82dc9b.o: ../../../../apps/bees/src/param_scaler.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling param_scaler.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/pickle_c911729b.o: ../../../../apps/bees/src/pickle.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling pickle.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/play_f3d9e8f5.o: ../../../../apps/bees/src/play.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling play.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/preset_a900fee0.o: ../../../../apps/bees/src/preset.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling preset.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/render_d79a8bd7.o: ../../../../apps/bees/src/render.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling render.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/scaler_amp_517ec272.o: ../../../../apps/bees/src/scalers/scaler_amp.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling scaler_amp.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/scaler_bool_e02b01c6.o: ../../../../apps/bees/src/scalers/scaler_bool.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling scaler_bool.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/scaler_fix_51c38483.o: ../../../../apps/bees/src/scalers/scaler_fix.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling scaler_fix.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/scaler_fract_fd244e34.o: ../../../../apps/bees/src/scalers/scaler_fract.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling scaler_fract.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/scaler_integrator_e37c6cf3.o: ../../../../apps/bees/src/scalers/scaler_integrator.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling scaler_integrator.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/scaler_integrator_short_964cb2b0.o: ../../../../apps/bees/src/scalers/scaler_integrator_short.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling scaler_integrator_short.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/scaler_note_f4a754ce.o: ../../../../apps/bees/src/scalers/scaler_note.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling scaler_note.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/scaler_short_9c90f4ea.o: ../../../../apps/bees/src/scalers/scaler_short.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling scaler_short.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/scaler_svf_fc_c9b7ed75.o: ../../../../apps/bees/src/scalers/scaler_svf_fc.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling scaler_svf_fc.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/scene_16ee7035.o: ../../../../apps/bees/src/scene.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling scene.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/util_fcd65243.o: ../../../../apps/bees/src/util.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling util.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Main_90ebc5c2.o: ../../Source/Main.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling Main.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/GraphEditorComponent_693cb26b.o: ../../Source/GraphEditorComponent.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling GraphEditorComponent.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/BinaryData_ce4232d4.o: ../../JuceLibraryCode/BinaryData.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling BinaryData.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_core_aff681cc.o: ../../JuceLibraryCode/modules/juce_core/juce_core.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_core.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_data_structures_bdd6d488.o: ../../JuceLibraryCode/modules/juce_data_structures/juce_data_structures.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_data_structures.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_events_79b2840.o: ../../JuceLibraryCode/modules/juce_events/juce_events.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_events.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_graphics_c8f1e7a4.o: ../../JuceLibraryCode/modules/juce_graphics/juce_graphics.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_graphics.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_gui_basics_a630dd20.o: ../../JuceLibraryCode/modules/juce_gui_basics/juce_gui_basics.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_gui_basics.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_gui_extra_7767d6a8.o: ../../JuceLibraryCode/modules/juce_gui_extra/juce_gui_extra.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_gui_extra.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

-include $(OBJECTS:%.o=%.d)
