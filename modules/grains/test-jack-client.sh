gcc -ljack -lm bfin_emu/fract32_jack.c bfin_emu/jack_pitch_tracker.c ../../dsp/ricks_tricks.c -I ../../dsp -I ./bfin_emu -I ./ -o jack_pitch_tracker;
# ./jack_pitch_tracker
