TRACKER beta 0.0.2 for Aleph

install/filecopy to SD-CARD (ALEPH):
aleph-prgm.hex -> ALEPH/app/
aleph-prgm.ldr -> ALEPH/mod/
aleph-prgm.dsc -> ALEPH/mod/

REBOOT with aleph-prgm.hex



GLOBAL controls
sw4                 page select

I/O
cv outputs 0-3      cv
cv input0           step +1

page TRACKER
encoders            level fine
sw0 + encoders      level course
sw1 + encoders      level transpose (scaled)
sw2                 edit step -1
sw3                 edit step +1

page ENVELOPE
encoders            time
encoders + sw0      curve
encoders + sw1      destination
encoders + sw2      destination presets; 0v, next source, 10v
sw3                 play current step



sound examples @ https://soundcloud.com/epla