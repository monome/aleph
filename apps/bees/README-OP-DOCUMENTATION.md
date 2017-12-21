# OPERATORS

Description of operators available in a bees network, including list
of inputs and outputs. at bottom is a list of proposed ops yet to be
implemented.  if you're interested in developing new operators using
the C language, head to the bees development page.

# ACCUM
Accumulates input values to a stored number. Can be set to wrap the
count around the MIN and MAX values.
## Inputs
- INC: receives the number to add to the current VAL.
- VAL: sets the stored number’s current state.
- MIN: sets the minimum value for the stored number.
- MAX: sets the maximum value for the stored number.
- WRAP: if set, the output will wrap to opposing boundary when passing
  the MAX/MIN limit.
## Outputs
- VAL: outputs the current stored number whenever it changes.
- WRAP: when WRAP input is 1, sends a 0 whenever the count wraps to
  the opposing boundary. when WRAP input is 0, will output the amount
  by which the input exceeded the limit (aka overflow).

# ADC

See CV-IN

# ADD
Adds input A and input B. outputs the sum.
## Inputs
- A: added to input B and causes output.
- B: sets the value to be added to input A. does not cause output.
- B_TRIG: when 1, a new value sent to input B will also cause output.
## Outputs
- SUM: outputs the sum of A + B.

# ARC
Allows input from monome arc devices. Provides rudimentary led
feedback for further expansion. All outputs occur for any arc
movement.
## Inputs
- FOCUS: when 1 operator is active. when 0 operator ignores arc.
- LOOP: loopback mode. when set, rotating the encoder will set and
  display an internal value [0, 255]. when disabled, the leds and
  encoders are decoupled.
- RING: input selects which ring to send led data to [0, 2/4].
- POS: input selects which led to address in the selected ring
  [0, 63].
- VAL: input sends brightness value to selected led [0, 15].
## Outputs
- NUM: outputs which ring has been rotated.
- DELTA: rotation amount for input to an ACCUM op.
- VAL: internally stored value between 0 and 255.

# BARS
Display four horizontal bar graphs in PLAY mode Values are from 0-127,
so some pre-scaling (using MUL or DIV) may be necessary.
## Inputs
- ENABLE: toggle on/off
- PERIOD: refresh rate
- MODE: unused currently (for future development)
- A/B/C/D: values displayed, top to bottom
## Outputs
(none)

# BARS8
Display eight horizontal bar graphs in PLAY mode Values are from
0-127, so some pre-scaling (using MUL or DIV) may be necessary.
## Inputs
- ENABLE: toggle on/off
- PERIOD: refresh rate
- MODE: unused currently (for future development)
- A/B/C/D/E/F/G/H: values displayed, top to bottom
## Outputs
(none)

# BIGNUM
Display a large number in PLAY mode
## Inputs
- ENABLE: toggle on/off
- PERIOD: refresh rate
- VAL: number to display in PLAY mode
- X/Y: display position on screen
## Outputs
(none)

# BITS
Convert a single decimal to many bits, and many bits to one decimal
## Inputs
- IN: decimal input to covert to bits
- I0-I7: bits to convert to decimal
## Outputs
- OUT: decimal from input bits
- O0-O7: bits from decimal input
This op is useful for using decimal numbers as “storage” for many
binary numbers (on/off) which could be considered triggers, ie. for
drums. So decimal 3 is binary 00000011, which could mean play note 0
and note 1– decimal 13 is binary 00001101… note 0,2,3. etc.

# CHANGE
A gate that opens only when values change
## Inputs
- VAL: input number
- LAST: value to compare the input number to
## Outputs
- VAL: input number if different from LAST

# CKDIV (Clock-dividing METRO)
A version of METRO (see METRO) which exactly subdivides the time of
input PERIOD into DIV ticks.  Useful for subdividing clocks of known
duration e.g taptempo based off timer
## Inputs
- ENABLE: turn CKDIV on/off & reset timer
- PERIOD: set the duration (in ms) between TICKs
- VAL: the value to send on TICK & TOCK
- DIV: the number of TOCKs to output for each tick
## Outputs
- TICK: outputs VAL every PERIOD ms since last tick or ENABLE
- TOCK: outputs VAL every PERIOD/DIV ms since last TICK or TOCK

# CV-IN
Receives control input from the CV input jacks.
## Inputs
- ENABLE: a value of 1 begins polling of the ADC inputs. 0 turns off
  polling
- PERIOD: sets number of milliseconds between each poll of the inputs
- MODE: 0 is continuous input. 1 is trigger input which only changes
  on transition, threshold is set around 2.5V
## Outputs
- VAL0-VAL3: outputs a stream of values from the respective input,
  corresponding to the received input voltage. range is [0,4095] when
  mode=0, and 0/1 when mode=1.

# DELAY
Delay a value by a specified time. Each input is sent to the output
after TIME (ms). If an input arrives while another input is being
delayed, this old value will be overwritten and the delay restarted.
## Inputs
- VAL: the input value to delay
- TIME: amount of time in ms
- CLEAR: cancel any values currently waiting.
## Outputs
- VAL: output after set time

# DIV
Divides input A by input B. Outputs the quotient.
## Inputs
- A: divided by input B and causes output.
- B: sets the B value. does not cause output.
- B_TRIG: when 1, a new value sent to input B will also cause output.
## Outputs
- VAL: outputs the quotient of A / B.

# DIVr
Divides input A with input B. outputs the quotient and remainder. A
combination of DIV and MOD.
## Inputs
- A: divided by input B and causes output.
- B: sets the B value. does not cause output.
- B_TRIG: when 1, a new value sent to input B will also cause output.
## Outputs
- VAL: outputs the quotient of A / B.
- R: outputs the integer remainder of A / B.

# ENC
Allows the aleph’s encoders to be routed in the bees network.
## Inputs
- MIN: sets the minimum output value.
- MAX: sets the maximum output value.
- STEP: sets the increment/decrement value for each step.
- VAL: sets the internally stored value.
## Outputs
- VAL: outputs the current internally stored value.
- DELTA: outputs rotational values when the encoder is turned. can be
  routed to numerous ACCUM operators for multi-modal encoders.

# FADE
Crossfades between inputs A and B according to input X using a linear
curve. All inputs cause output to be updated.
## Inputs
- A: sets the A input value.
- B: sets the B input value.
- X: fades between input A (X=0) and input B (X=128). values outside
  the 0-128 range are clamped to the range.
## Outputs
- VAL: the output of the crossfader.

# FS [1-2]
Allows the footswitch inputs to be routed in the control network.
## Inputs
- TOG: when 0, output is momentarily high when held. when 1, output
  will toggle between states on each press.
- MUL: this value is output when the footswitch is active. 0 when
inactive.
## Outputs
- VAL: the current state value of the footswitch (0 or MUL).

# GATE
Pass or block the input value to the output.
## Inputs
- VALUE: the value to be passed or blocked.
- GATE: 1 passes all values to output. 0 blocks input.
- STORE: when 1, a value of 0 sent to the GATE input will trigger
  output of the most recent value (aka sample and hold).
Outputs
- GATED: echoes the VALUE input when GATE is set to 1.

# GRID
Receives key data from a monome grid. Displays the current state on
the device.
## Inputs
- FOCUS: when 1, operator is active. when 0, operator ignores grid.
- TOG: when 0, all keys are momentary. when 1, cells toggle between
  states on each press.
- MONO: when 1, only 1 led will be lit at a time.
## Outputs
- COL: on each event, sends the column value (0-15).
- ROW: on each event, sends the row value (0-15).
- VAL: on each event, sends the state (1: press, 0: release).

# GRIDRAW
Receives key data from a monome grid and allows bidirectional
information with the Aleph
## Inputs
- FOCUS: when 1, operator is active. when 0, operator ignores grid
- X: column value (0-15) sent to the grid
- Y: row value (0-7) sent to the grid
- VAL: brightness value (0-15) sent to the varibright grid only 
- ON: button on/off value sent to the grid, equivalent to a value of 15
## Outputs
- X: on each grid event, sends the column value (0-15).
- Y: on each grid event, sends the row value (0-15).
- BUT: on each grid event, sends the state (1: press, 0: release).

# HARRY
A microtonal ‘tonality diamond’ operator: a harmonic/melodic construct
arranging all the distinct permutations of consonant ratios within the
canonical octave. These ratios are laid out in a square or diamond -
you can play it as an instrument or trigger the ratios by their x-y
coordinates from any BEES op.
## Inputs: 
- X: Set X coordinate on tonality diamond.
- Y: Set Y coordinate on tonality diamond.
- BONK: Output the Hz value at X,Y.
- SIZE: Set size of tonality diamond (max value of X/Y)
## Outputs: 
- HZ: Outputs a logarithmic pitch value for the member of tonality
  diamond at X,Y.  256 corresponds to one semitone, so this can be
  connected directly to, e.g Hz input on waves.

# HID
Connect a Human Interface Device over USB. Multiple operators are
required to monitor different bytes. Connect a gamepad, shnth or
manta.
## Inputs
- BYTES: sets which HID byte to monitor
- SIZE: sets the bytesize of the parameter to 8/16 bits.
# Outputs
- VAL: the output value received from the HID.

# HISTORY
Stores a running list of the last 8 values input, provides the
average.
## Inputs
- IN: the input.
## Outputs
- AVG: average of the last 8 values.
- O0-7: O0 is the input, O1 the previous, O2 … O7. a queue.

# IS
Comparisons. Equals, not equals, greater than, less than.
## Inputs
- A: sets first value.
- B: sets second value. does not cause output.
- B_TRIG: when 1, a new value sent to input B will also cause output.
- EDGE: when 1, output triggered only on transition between states.
## Outputs
- EQ: 1 if A == B (A equals B)
- NE: 1 if A != B (A does not equal B)
- GT: 1 if A > B (A is greater than B)
- LT: 1 if A < B (A is less than B)

# ITER
bangs a value n times, outputting n, then val each time, and finally
bangs done
## Inputs:
- TIMES: number of times (0-16) a value is sent to the VAL output
- VAL: the value sent to the VAL output. also triggers the whole ITER
  sequence
- STOP: if banged during the ITER sequence, brings an early out from
the iteration
## Outputs:
- IDX: iterates from 0 to TIMES
- VAL: value sent for each iteration
- DONE: sends 1 when done

A single bang to value triggers the following sequence of bangs:
- 0->idx, value->value_out
- 1->idx, value->value_out
...
- times->idx, value->value_out

# KRIA
Parameter-phased modulator. Step sequencer arpeggiator. Two voices
with independent loop positions and speed per parameter: trigger,
accent, duration, note, octave, transposition, scale.
# Inputs: 
- FOCUS: when 1, operator is active. when 0, operator ignores grid
- CLOCK: advances the step clock by 1. expects alternating 0/1 from a
  METRO op
- OCTAVE: Sets the number of semitones in an octave on first page.  So
  for example you could set OCTAVE to 7, then ‘octave’ column on first
  page would transpose by 7 semitones instead of 12.
- TUNING: Sets the semitone jump between rows on the transpose page.
Defaults to 5, like a bass guitar standard tuning.
- PATTERN: currently not-implemented, feature scheduled for 0.8.1
update
## Outputs:
- TR0: Trigger output for Note 0 – sends 1 for ‘note on’, 2 for
  ‘accent’, 0 for ‘note off’
- NOTE0:  Outputs the note value (in semitones) of Track 0
- TR1: Trigger output for Note 1 – sends 1 for ‘note on’, 2 for
  ‘accent’, 0 for ‘note off’
- NOTE1: Outputs the note value (in semitones) of Track 1

# LIFE
Conway’s game of life. With monome grid support.
## Inputs
- NEXT: non-zero input triggers algorithm.
- XSIZE: size of field, 8 or 16
- YSIZE: size of field, 8 or 16
- X: postion for edit by SET, output of VAL
- Y: position for edit by SET, output of VAL
- SET: 0/1 set at X/Y
- NOISE: (not yet implemented)
- RULES: 0=normal, 1=nodeath, 2=nobirth (weird/fun)
- FOCUS: set 1 to focus monome
## Outputs
- VAL: the value at X/Y
- POP: total population
- DELTA: change in population this round

# LINLIN (LL)
Makes a linear mapping from an input range to an output
range. (replacing combinations of MUL/ADD/DIV in many situations)
## Inputs
- IN: input signal
- IMIN: lower limit of input range
- IMAX: upper limit of input range
- OMIN: lower limit of desired output range
- OMAX: upper limit of desired output range
## Outputs
- VAL: IN triggers the calculation:
  (IN - IMIN) / (IMAX - IMIN) * (OMAX - OMIN) + OMIN

## Inputs
- INDEX: (0-1) get value at given index, sent to output
- I0-I1: set input values to store
## Outputs
- VAL: the value at given index

# LIST2
Storage for two values, recallable by index. Good for use with a
binary (0-1) input, for setting individual values.
## Inputs
- INDEX: (0-1) get value at given index, sent to output
- I0-I1: set input values to store
## Outputs
- VAL: the value at given index

# LIST4
Storage for four values, recallable by index. Good for use with a
binary (0-1) input, for setting individual values.
## Inputs
- INDEX: (0-1) get value at given index, sent to output
- I0-I1: set input values to store
## Outputs
- VAL: the value at given index

# LIST8
Storage for eight values, recallable by index .
## Inputs
- INDEX: (0-7) get value at given index, sent to output
- I0-I7: set input values to store
## Outputs
- VAL: the value at given index

# LIST16
Storage for sixteen values, recallable by index.
## Inputs
- INDEX: (0-15) get value at given index, sent to output
- I0-I15: set input values to store
## Outputs
- VAL: the value at given index

# LOGIC
Logic comparisons. Takes binary input and outputs binary values.
## Inputs
- A: first value 0/1
- B: second value 0/1
- B_TRIG: 0/1, execute on B change if 1
- EDGE: 0/1, if 1, only output on transitions
- INVERT: 0/1, if 1, invert output
## Outputs
- AND: A && B (1 if A is 1 and B is 1)
- 
OR: A
 
B (1 if A is 1 or B is 1)
- XOR: A ^ B (1 if A is 1 or B is 1 but not both)

# MAGINC
A magical incrementation operator.  It ‘looks ahead’ in the network at
the input it’s connected to, and outputs an absolute value relative to
the current value of connected input.  Useful when you want encoders
to switch between controlling different params without getting jumps
in the VAL of the param due to jumps the encoder’s VAL.
## Inputs: 
- INC: value to increment for each bang
## Outputs: 
- VAL: connect this to the input you want to ‘magically increment’

# MEM0D
Allows to read/write/toggle a number
## Inputs: 
1. WRITE: writes value 
2. READ: sends the written value to the output
3. TOG: toggles the indexed memory element on/off when banged with a
number greater than 0. does not trigger an output ## Outputs:
1. VAL: outputs the written value

## MEM1D
 allows to write/read/toggle location in a 1D memory
## Inputs: 
1. N: address number (0-15)
2. WRITE: writes value into address N
3. READ: sends the value written at address N to the output
4. TOG: toggles the indexed memory element on/off when banged with a
number greater than 0. Does not trigger an output ## Outputs:
- N: outputs the address number (0-15)
- VAL: outputs the value written at address N

# MEM2D
 allows to write/read/toggle location in a 2D memory
## Inputs: 
- X: address number (0-15), typically grid column
- Y: address number (0-15), typically grid row
- WRITE: writes value into address X/Y
- READ: sends the value written at address X/Y to the output
- TOG: toggles the indexed memory element on/off when banged with a
number greater than 0. does not trigger an output ## Outputs:
- X: N: outputs the X (0-15) of memory address 
- Y: N: outputs the Y (0-15) of memory address 
- VAL: outputs the value written at address X/Y

# METRO
Outputs a given value at a regular interval.
Inputs
- ENABLE: a value of 1 activates the output. 0 stops output.
- PERIOD: sets the time interval in milliseconds.
- VAL: sets the value that is sent on each tick.
## Outputs
- TICK: outputs the VAL at the end of each interval.

# MIDICC
Receives MIDI cc events from a USB connected device.
Inputs
- CHAN: sets the MIDI channel to listen to. -1 responds to all channels.
- NUM: sets CC number to listen to.
## Outputs
- VAL: outputs the cc value (0-127).

# MIDINOTE
Receives MIDI note events from a USB connected device.
## Inputs
- CHAN: sets the MIDI channel to listen to. -1 responds to all channels.
## Outputs
- NUM: outputs the MIDI note number (0-127).
- VEL: outputs the MIDI velocity (0-127, 0 is a release).
- PITCH: outputs MIDI pitchbend messages

# MIDICLK (MIDI Clock)
Receives MIDI realtime messages aka midi clock
## Inputs
(None)
## Outputs
- TICK: output 1 on MIDI tick message
- START: output 1 on MIDI start message
- CONT: output 1 on MIDI continue message
- STOP: output 1 on MIDI stop message

# MIDIPROG
Receives MIDI Program Change from a USB connected device
Inputs
- CHAN: sets the MIDI channel to listen on. -1 listens to all channels
## Outputs
- VAL: outputs MIDI Program Change sent to the USB midi interface

# MOUT_CC
 Sends MIDI CC to a USB connected device.
## Inputs: 
- CABLE: the USB-midi 'cable number' to send to (for devices with only
  1 USB-midi 'cable') leave this as 0.
- CHAN: sets the MIDI channel to send to
- NUM: CC number (0-127)
- VAL: value associated to CC number (0-127)
## Outputs: 
(none)

# MOUT_NOTE (MOUT_N)
Sends MIDI note events to a USB connected device.
## Inputs
- CABLE: the USB-midi 'cable number' to send to (for devices with only
  1 USB-midi 'cable') leave this as 0.
- CHAN: sets the MIDI channel to send to
- NUM: MIDI note number (0-127)
- VEL: MIDI velocity (0-127, 0 is a release)
## Outputs
(none)

# MOUTCK (MIDI Clock Output)
Sends MIDI realtime messages aka midi clock
## Inputs
- CABLE: the USB-midi 'cable number' to send to (for devices with only
  1 USB-midi 'cable') leave this as 0.
- TICK: sends MIDI tick message when banged
- START: sends MIDI start message when banged
- CONT: sends MIDI continue message when banged
- STOP: sends MIDI stop message when banged
## Outputs
(None)

# MOD
Divides input A by input B, outputs the remainder.
Inputs
- A: sets the numerator and causes output.
- B: sets the denominator. does not cause output.
- B_TRIG: when 1, a new value sent to input B will also cause output.
## Outputs
- VAL: outputs the remainder (mod) of A / B.

# MP
Meadowphysics, for grid. See meadowphysics
## Inputs
- FOCUS: when 1, operator is active. when 0, operator ignores grid
- SIZE: sets grid size ( 8 / 16 )
- STEP: move playhead ( + / - )
## Outputs
- O0-O7: trigger outputs corresponding to descending rows

# MUL
Multiplies input A by input B. Outputs their product.
## Inputs
- A: sets the number to be multiplied by input B and causes output.
- B: sets the number by which input A is multiplied. does not cause output.
- TRIG: when 1, a new value sent to input B will also cause output.
## Outputs
- VAL: outputs the product of A * B.

# PARAM
Gives access to the value in Bees (-32767 -> 32767) of each parameter
from each module.
## Inputs: 
- IDX: addresses any parameter from the chosen module
## Outputs: 
- VAL: outputs the value of that parameter

# POLY
managing note priority & triggering monophonic or polyphonic
modules from a midi kbd or grid
## Inputs: 
- PITCH:  Pitch index of note
- AMP: Amplitude of note (0 is interpreted as noteoff)
- OFF: ‘Panic button’ - to reset internal state & transmit note_off to all 
- MONO: monophonic mode.  0 is polyphonic, 1 is monophonic
## Outputs: 
- IDX: voice index
- PITCH: pitch index of note
- AMP: amplitude of note

# PRESET
Presets can be read or written in response to received input. Values
and routings need to be ‘included’ in the preset system before they
will be recalled.
## Inputs
- READ: any received input recalls the state of the number PRESET
  received.
- WRITE: any received input writes the current state to the number
  PRESET received.

# RANDOM
Generates a random number after receiving any input.
## Inputs
- TRIG: any input causes a new random number to output.
- MIN: sets the minimum value that can be output.
- MAX: sets the maximum value that can be output.
## Outputs
- VAL: sends the random number.

# ROUTE (ROUTE4)
Direct an input to one of four outputs.
## Inputs
- VAL: input value to route
- TO: which output to send VAL to (0-3)
## Outputs
- O0-O3: outputs

# ROUTE2
Direct an input to one of two outputs.
## Inputs
- VAL: input value to route
- TO: which output to send VAL to (0-1)
## Outputs
- O0-O1: outputs

# ROUTE8
Direct input to one of eight outputs.
## Inputs
- VAL: input value to route
- TO: which output to send VAL to (0-7)
## Outputs
- O0-O7: outputs

# ROUTE16
Direct input to one of sixteen outputs.
## Inputs
- VAL: input value to route
- TO: which output to send VAL to (0-15)
## Outputs
- O0-O15: outputs

# SCREEN
Draw pixels on PLAY screen.
## Inputs
- ENABLE: on/off
- PERIOD: refresh rate
- VAL: pixel value to draw (0-15)
- FILL: fill entire screen (0-15)
- X/Y: position to draw
## Outputs
(None)

# SERIAL
Send indexed pairs of numbers to host computer over USB serial
## Inputs
- ADDR: param index
- DATA: param value
## Outputs
- ADDR: currently not-implemented, feature scheduled for 0.8.1 update
- DATA: currently not-implemented, feature scheduled for 0.8.1 update

# SHL
 Left bit shift, mostly to use with BIT operator
## Inputs: 
- A: Decimal number to be shifted
- B: number of bit to shift to the left
- TRIG: when 1, a new value sent to input B will also cause output
## Outputs: 
- VAL: Decimal value, bit shifted to the left

# SHR
Right bit shift, mostly to use with BIT operator

## Inputs: 
- A: Decimal number to be shifted
- B: number of bit to shift to the left
- TRIG: when 1, a new value sent to input B will also cause output
## Outputs: 
- VAL: Decimal value, bit shifted to the right

# SPLIT (Y)
Receives one input and echoes it to two outputs.
Inputs
- VAL: receives an input value.
## Outputs
- OUT1: sends out the input value first.
- OUT2: sends out the input value second.

# SPLIT4 (Y4)
Receives one input and echoes it to four outputs.
## Inputs
- VAL: receives an input value.
## Outputs
- OUT1: sends out the input value first.
- OUT2: sends out the input value second.
- OUT3: sends out the input value third.
- OUT4: sends out the input value fourth.

# STEP
A step sequencer for monome grids.
## Inputs
- FOCUS: when 1, operator is active. when 0, operator ignores grid
- SIZE: sets grid size ( 8 / 16 )
- STEP: move playhead ( + / - )
## Outputs
- A : sends out value of button in row 4 at upper playhead position
  ( 0 / 1 )
- B : sends out value of button in row 5 at upper playhead position
  ( 0 / 1 )
- C : sends out value of button in row 6 at upper playhead position
  ( 0 / 1 )
- D : sends out value of button in row 7 at upper playhead position
  ( 0 / 1 )
- MONO1 : bitmapped value of column at upper playhead position (see
  below)
- POS1 : current step of upper playhead
- MONO2 : bitmapped value of column at lower playhead position (see
  below)
- POS2 : current step of lower playhead.
## Grid
- row 0 = set position of upper playhead (also sets beginning of
  cycle)
- row 1 = set length of upper loop (if loop is longer than distance
  from playhead to end of row it will cycle around to beginning of
  row)
- row 2 = set position of lower playhead (also sets beginning of
  cycle)
- row 3 = set length of lower loop (if loop is longer than distance
  from playhead to end of row it will cycle around to beginning of
  row)
- row 4 - 7 = bitmapped values. MONO 1/2 outputs the sum of the 4
  rows, the individual value for active buttons are as follows
- row 4 = 1
- row 5 = 2
- row 6 = 4
- row 7 = 8 e.g. to get the value 5 you would have row 4 and row 6
  active, to get the value 9 you would have row 4 and row 7 active,
  and so on and so forth. it’s a binary-to-decimal conversion with row
  4-7 being bits 0-3 of the number.

# SUB
Subtracts input B from input A. outputs the difference.
## Inputs
- A: sets the number from which to subtract input B and causes output.
- B: sets the number to subtract from input A. does not cause output.
- B_TRIG: when 1, a new value sent to input B will also cause output.
## Outputs
- DIF: outputs the difference of A - B.

# SW [1-4]
Allows the aleph’s keys to be routed in the control network.
## Inputs
- TOG: when 0, switches are momentarily high when held. when 1,
  switches will toggle between states on each press.
- MUL: this value is output when the switch is active. 0 when
inactive.  ## Outputs
- VAL: the current state value of the switch (0 or MUL).
## Outputs
(None)

# THRESH
Sends input through 1st outlet if below threshold, 2nd if not .
## Inputs
1. VAL: value to be tested
2. LIM: the threshold (limit”)
## Outputs
- LO: output was < threshold
- HI: output was >= threshold

# TIMER
Outputs the time interval between the last two received events in milliseconds.
## Inputs
- EVENT: causes output, then restarts timer at zero
- RESET: restarts timer at zero without causing output
## Outputs
- TIME: outputs time interval between timer restart-to-zero & event

# TOG
Toggles between zero and a given number.
## Inputs
- BANG: any positive input causes the state to change. <1 outputs 0.
- MUL: sets the value to be output when toggle is on.
## Outputs
- VAL: outputs the current state of the toggle. ‘off’ outputs 0. ‘on’
  outputs MUL.

# WHITE WHALE (WW)
White whale, for grid. see white whale
## Inputs
- FOCUS: when 1, operator is active. when 0, operator ignores grid
- CLOCK: advances the step clock by 1. expects alternating 0/1 from a
  METRO op.
- PARAM: input parameter for setting CV values; range [0,4095].
- CUT: currently not-implemented, feature scheduled for 0.8.1 update
- PATTERN: currently not-implemented, feature scheduled for 0.8.1
  update
## Outputs
- TR0-TR3: trigger or gate outputs from the sequencer; [0,1].
- CVA-CVB: continuous outputs. for map mode MUL by 8 for CV
  output. For waves, MUL by 15, then DIV by 2.

# Proposed Operators:

TODO: add these operators!

# ECA
An elementary (1d binary) cellular automaton

# GRIDCOL
a monome grid operator for direct column manipulation.
## Inputs
- X
- COL
## Outputs
- X
- COL

# HIST2
Stores a running list of the last 2 values input, provides the
average.
## Inputs
- IN: the input.
## Outputs
- AVG: average of the last 2 values.
- O0-1: O0 is the input, O1 the previous.

# HIST8:
output the last 8 values in a circular buffer (8 outputs) - think this
is the same as currently implemented HISTORY

# HIST16:
output the last 16 values in a circular buffer (16 outputs)

# HIST:
should be renamed to MEAN or AVG. or maybe HISTX should also have
mean/median outputs. or should be rolled into more general STATS op.

