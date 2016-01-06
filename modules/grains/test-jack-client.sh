gcc -ljack -lm \
    bfin_emu/fract32_jack.c \
    bfin_emu/jack_pitch_tracker.c \
    ../../dsp/grain.c \
    ../../dsp/echoTap.c \
    ../../dsp/scrubTap.c \
    ../../dsp/ricks_tricks.c \
    ../../dsp/pan.c \
    ../../dsp/buffer.c \
        -I ../../dsp -I ./bfin_emu -I ./ -I ../../bfin_lib/src/ -I ../../bfin_lib/src/libfixmath -I ../../common\
    -o jack_pitch_tracker;
# ./jack_pitch_tracker
