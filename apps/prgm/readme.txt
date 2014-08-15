PRGM/TRACKER beta 0.0.1 for Aleph

install/filecopy to SD-CARD (ALEPH):
aleph-prgm.hex -> ALEPH/app/
aleph-prgm.ldr -> ALEPH/mod/
aleph-prgm.dsc -> ALEPH/mod/

REBOOT with aleph-prgm.hex



GLOBAL controls
sw4                 page select


I/O
audio outputs 0-3   oscillator 1-4 - outputs 3&4 are direct out, watch out for high levels!
foot1               sequence step+1
foot2               transpose step+1
cv input 0          sync trig
cv outputs 0-3      scaled pitch, 1v/oct


page TRACKER (4 steps, can be set to longer..)
encoders            scaled pitch
sw0 + enc0-3        transpose pitch
sw1                 sequence step+1
sw2                 transpose step+1
sw3 + enc2          sequence length 1-16
sw3 + enc3          transpose length 1-12

page PRGM
encoders            waveform select
encoders + sw0      phase trip point
encoders + sw1      blend wavetable A|B


sound examples @ https://soundcloud.com/epla