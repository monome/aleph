#ifndef _ALEPH_BEES_OP_WW_H_
#define _ALEPH_BEES_OP_WW_H_

#include "net_monome.h"
#include "net_poll.h"
#include "op.h"
#include "op_math.h"
#include "types.h"

typedef enum {
  mTrig, mMap, mSeries
} edit_modes;

typedef enum {
  mForward, mReverse, mDrunk, mRandom
} step_modes;

typedef struct {
  u8 loop_start, loop_end, loop_len, loop_dir;
  u16 step_choice;
  u8 cv_mode[2];
  u8 tr_mode;
  step_modes step_mode;
  u8 steps[16];
  u8 step_probs[16];
  u16 cv_values[16];
  u16 cv_steps[2][16];
  u16 cv_curves[2][16];
  u8 cv_probs[2][16];
} whale_pattern;

typedef struct {
  whale_pattern wp[16];
  u16 series_list[64];
  u8 series_start, series_end;
  u8 tr_mute[4];
  u8 cv_mute[2];
} whale_set;

typedef void(*re_t)(op_monome_t *op_monome);

typedef struct {
  edit_modes edit_mode;
  u8 edit_cv_step;
  u8 edit_cv_ch;
  s8 edit_cv_value;
  u8 edit_prob;
  u8 live_in;
  u8 scale_select;
  u8 pattern;
  u8 next_pattern;
  u8 pattern_jump;
  
  u8 series_pos;
  u8 series_next;
  u8 series_jump;
  u8 series_playing;
  u8 screll;
  u8 screll_pos;
  
  u8 key_alt;
  u8 center;
  u8 held_keys[32];
  u8 key_count;
  u8 key_times[256];
  u8 keyfirst_pos;
  s8 keycount_pos;
  u8 keycount_series;
  u8 keycount_cv;
  
  s8 pos;
  s8 cut_pos;
  s8 next_pos;
  s8 drunk_step;
  s8 triggered;
  u8 cv_chosen[2];
  u16 cv0;
  u16 cv1;
  
  u8 param_accept;
  u8 *param_dest8;
  u16 clip;
  u16 *param_dest;
  
  u8 series_step;
  
  u8 SIZE;
  u8 LENGTH;
  u8 VARI;
  re_t re;
  u8 dirty;
  
  u8 tr[4];
  u16 param;
} whale_extra;

//--- white whale
typedef struct op_ww_struct {
  op_t super;
  op_monome_t monome;
  // inputs: mode, focus, step
  volatile io_t focus;
  volatile io_t clk;
  volatile io_t param;
  volatile io_t* in_val[3];
  // outputs: a,b,c,d
  op_out_t outs[7];
  // internal:


    // timer data
  softTimer_t timer;
  // polled operator superclass
  op_poll_t op_poll;
  whale_set w;
  whale_extra x;
} op_ww_t;

// init
void op_ww_init(void* op);
// de-init
void op_ww_deinit(void* op);

#endif // header guard
