/* op_derived.h
   bees
   aleph
   
   this header should include all extant derived operator classes.
*/

#ifndef _OP_DERIVED_H_
#define _OP_DERIVED_H_

// derived op classes
#include "ops/op_add.h"
#include "ops/op_accum.h"
#include "ops/op_adc.h"
#include "ops/op_bignum.h"
#include "ops/op_bits.h"
#include "ops/op_delay.h"
#include "ops/op_div.h"
#include "ops/op_enc.h"
#include "ops/op_gate.h"
#include "ops/op_history.h"
#include "ops/op_is.h"
#include "ops/op_life.h"
#include "ops/op_list2.h"
#include "ops/op_list8.h"
#include "ops/op_logic.h"
#include "ops/op_metro.h"
#include "ops/op_midi_note.h"
#include "ops/op_mod.h"
#include "ops/op_monome_grid_raw.h"
#include "ops/op_mul.h"
#include "ops/op_preset.h"
#include "ops/op_split.h"
#include "ops/op_split4.h"
#include "ops/op_sub.h"
#include "ops/op_random.h"
#include "ops/op_route.h"
#include "ops/op_screen.h"
#include "ops/op_sw.h"
#include "ops/op_thresh.h"
#include "ops/op_timer.h"
#include "ops/op_tog.h"

// let's please keep this list alphabetical so we can easily see whether something is included.
// the things that need to remain chronological are:
// - the class type enumeration in op.h
// - the class registry array in op.c

#endif
