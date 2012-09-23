BEES 

"bees" is the UI and control processing program for Aleph.

this README describes the operation of the current proposed development prototype.


//==========================================
// ==== CONTROLS
//==========================================

------ key bindings for testing (from ui.c)  :

#define KEY_QUIT        'q'
#define KEY_ENC_A_UP    '='
#define KEY_ENC_A_DOWN  '-'
#define KEY_ENC_B_UP    ']'
#define KEY_ENC_B_DOWN  '['
#define KEY_ENC_C_UP    '\''
#define KEY_ENC_C_DOWN  ';'
#define KEY_ENC_D_UP    '/'
#define KEY_ENC_D_DOWN  '.'
#define KEY_FN_A        'z'
#define KEY_FN_B        'x'
#define KEY_FN_C        'c'
#define KEY_FN_D        'v'


encoder A (top left) always scrolls between pages.

encoder B (top right) always scrolls within whatever list of things the current page provides.

encoders C and D (bottom) are defined contextually by the current page.

the function keys (A, B, C, and D) are also contextually mapped. they should follow a logical patter wherever possible - for example, FN_D usually means to toggle the currently selected thing (input, target of output, all inputs of operator) in or out of inclusion by the current preset.

a separate EDIT/PLAY switch toggles in and out of PLAY mode.
in this mode, all 4 encoders are mapped to Encoder operators in the operator network.
we will come up with some default display behavior
[ example: show values for the last DSP parameter(s) touched/modulated? ]
and eventually perhaps some user-defined behavior using Display operators.

//==========================================
// ==== MENUS
//==========================================

current proposal for menu structure, may change a lot!

each page description:

NAME : description
list format
encoder mappings
function mappings

---------------------------------------------
OPS : list / edit operators

index _ name _ any inputs in preset ? _ inputs connected ? _ outputs connected?
...

fnA  : go to inputs
fnB  : go to outputs
fnC  : create (below selection)
fnD  : delete

encC : move selected up/down in list (affects execution order)
encD : select new operator to be created (by fnC)

---------------------------------------------
INS - list / edit operator input values

index _ op/name _ value _ preset ? _ connected ? 
...

fnA  : gather
fnB  : disconnect (all)
fnC  : overwrite preset value (and include)
fnD  : toggle preset inclusion

encC : change value (slow)
encD : change value (fast)


---------------
OUTS - list / edit operator outputs and connections

index _ name _ target input name
...

fnA  : follow
fnB  : disconnect
fnC  : re-send value [overwriting preset?]
fnD  : toggle preset inclusion (this connection)

encC : scroll target
encD : ?

////////////////////////////////////
//// TODO!

---------------------------------------------
PRESETS - list / edit presets 
[a PRESET is a collection of input values and connections]

index _ name _ values dirty? _ op list dirty?
...
[NEW]

fnA  : clear
fnB  : copy
fnC  : store 
fnD  : recall

encC : select position in name
encD : change character in selected position
 
---------------------------------------------
SETUPS - list / edit setups 
[a SETUP stores a list of operators, a dsp program, and a bank of presets

index _ name _ values dirty? _ op list dirty?
...
[NEW]

fnA  : clear 
fnB  : copy
fnC  : store
fnD  : recall

encC : select position in name
encD : change character in selected position

---------------------------------------------
DSP - list / load DSP programs

index _ name _ current?
...

fnA  : assign to current setup
fnB  : 
fnC  : load default setup
fnD  : ?

encC : ?
encD : ?



+++++++++++++++++++++++++++++++++++
NOTES:

- scrolling pages with an encoder means that

