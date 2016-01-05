gcc -ljack -lm fract32_jack.c jack_pitch_tracker.c ../../dsp/ricks_tricks.c -I ../../dsp -I ./ -o jack_pitch_tracker;
# ./jack_pitch_tracker
