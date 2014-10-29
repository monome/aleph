README-OP-GUIDE

Overview on programming "operators" for Bees


Toolchain:

Mac: https://services.droparea.de/blog/avr32-toolchain-for-mac-os-x
Linux: http://www.atmel.com/tools/ATMELAVRTOOLCHAINFORLINUX.aspx (get 32bit)
Windows: http://atmel.com (get AVR Stuio)


An operator is a control module that can be dynamically created within Bees. It has INPUTS and OUTPUTS that accept and send parameter data, respectively. Bees networks are created by connecting OUTPUTS of operators to other operators' INPUTS.

Operators typically transform input somehow. But they can also modify the screen, talk USB, run timers, etc. For the scope of this introduction we'll simply do some math on the inputs. See exisitng operators for examples of other tasks.



Here are the steps to create a new operator. We'll be making an operator to crossfade between two input values.

The goal: three inputs A, B, and X. One output VAL. X is 0-128. When X is 0, VAL is A. When X equals 100, VAL equals B. When X equals 64, VAL is a split mix of A and B. Intermediate values will linearly mix A and B to VAL.



1. Copy an existing operator as a starting point.

from /aleph/apps/bees/

cp src/ops/op_add.c src/ops/op_fade.c
cp src/ops/op_add.h src/ops/op_fade.h




2. Edit config.mk

Add the following around line 54 (keep alphabetized):

	$(APP_DIR)/src/ops/op_fade.c \




3. Edit src/op.c

Add this line to the userOpTypes list (around line 31):

  eOpFade,

And then add to the op_registry around line 280:

  {
    .name = "FADE",
    .size = sizeof(op_fade_t),
    .init = &op_fade_init,
    .deinit = NULL 
  },




4. Edit src/op.h

Increase the number NUM_USER_OP_TYPES by one (around line 23)

Then add your new enum to the END of the list around line 83):

  eOpFade,




5. Edit src/op_derived.h

Add to the header file list:

#include "ops/op_fade.h"




5. Edit src/op_fade.h

See file. Basically all instances of "add" changed to "fade"

Changed internal variables for A, B, X




6. Edit src/op_fade.c

Finally, where the functionality happens. See code for inline comments.

Of fundamental importance are the functions op_fade_in_a() etc. This is where something happens when input is received.