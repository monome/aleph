// asf
#include "print_funcs.h"
// bees
#include "app_timers.h"
#include "net_protected.h"
#include "net_poll.h"
#include "op_ww.h"

//-------------------------------------------------
//----- static variables

//---- descriptor strings
static const char* op_ww_instring = "FOCUS\0  CLOCK\0  PARAM\0 ";
static const char* op_ww_outstring ="TR0\0    TR1\0    TR2\0    TR3\0    CVA\0    CVB\0    POS\0    ";
static const char* op_ww_opstring = "WW";

//-------------------------------------------------
//----- static function declaration

//---- input functions

//// network inputs: 
static void op_ww_in_focus(op_ww_t* grid, const io_t val);
static void op_ww_in_clock(op_ww_t* grid, const io_t val);
static void op_ww_in_param(op_ww_t* grid, const io_t val);

// pickles
static u8* op_ww_pickle(op_ww_t* enc, u8* dst);
static const u8* op_ww_unpickle(op_ww_t* enc, const u8* src);

// timer manipulation
static inline void op_ww_set_timer(op_ww_t* ww);
static inline void op_ww_unset_timer(op_ww_t* ww);


// polled-operator handler
static void op_ww_poll_handler(void* op);

/// monome event handler
static void op_ww_handler(op_monome_t* op_monome, u32 data);

static void op_ww_redraw(void);

static u32 rnd(void);

// input func pointer array
static op_in_fn op_ww_in_fn[3] = {
  (op_in_fn)&op_ww_in_focus,
  (op_in_fn)&op_ww_in_clock,
  (op_in_fn)&op_ww_in_param,
};

static u32 a1 = 0x19660d;
static u32 c1 = 0x3c6ef35f;
static u32 x1 = 1234567;  // seed
static u32 a2 = 0x19660d;
static u32 c2 = 0x3c6ef35f;
static u32 x2 = 7654321;  // seed




//////////////////////////////////////////////////

const u16 SCALES[24][16] = {

  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },                        // ZERO
  { 0, 68, 136, 170, 238, 306, 375, 409, 477, 545, 579, 647, 715, 784, 818, 886, },         // ionian [2, 2, 1, 2, 2, 2, 1]
  { 0, 68, 102, 170, 238, 306, 340, 409, 477, 511, 579, 647, 715, 750, 818, 886, },         // dorian [2, 1, 2, 2, 2, 1, 2]
  { 0, 34, 102, 170, 238, 272, 340, 409, 443, 511, 579, 647, 681, 750, 818, 852, },         // phrygian [1, 2, 2, 2, 1, 2, 2]
  { 0, 68, 136, 204, 238, 306, 375, 409, 477, 545, 613, 647, 715, 784, 818, 886, },         // lydian [2, 2, 2, 1, 2, 2, 1]
  { 0, 68, 136, 170, 238, 306, 340, 409, 477, 545, 579, 647, 715, 750, 818, 886, },         // mixolydian [2, 2, 1, 2, 2, 1, 2]
  { 0, 68, 136, 170, 238, 306, 340, 409, 477, 545, 579, 647, 715, 750, 818, 886, },         // aeolian [2, 1, 2, 2, 1, 2, 2]
  { 0, 34, 102, 170, 204, 272, 340, 409, 443, 511, 579, 613, 681, 750, 818, 852, },         // locrian [1, 2, 2, 1, 2, 2, 2]
  { 
    0, 34, 68, 102, 136, 170, 204, 238, 272, 306, 341, 375, 409, 443, 477, 511, },          // chromatic
  { 0, 68, 136, 204, 272, 341, 409, 477, 545, 613, 682, 750, 818, 886, 954, 1022, 	},        // whole
  { 0, 170, 340, 511, 681, 852, 1022, 1193, 1363, 1534, 1704, 1875, 2045, 2215, 2386, 2556, },    // fourths
  { 0, 238, 477, 715, 954, 1193, 1431, 1670, 1909, 2147, 2386, 2625, 2863, 3102, 3340, 3579, 	},   // fifths
  { 0, 17, 34, 51, 68, 85, 102, 119, 136, 153, 170, 187, 204, 221, 238, 255, },           // quarter
  { 0, 8, 17, 25, 34, 42, 51, 59, 68, 76, 85, 93, 102, 110, 119, 127, 	},              // eighth
  { 0, 61, 122, 163, 245, 327, 429, 491, 552, 613, 654, 736, 818, 920, 982, 1105, },          // just
  { 0, 61, 130, 163, 245, 337, 441, 491, 552, 621, 654, 736, 828, 932, 982, 1105, 	},        // pythagorean
  { 
    0, 272, 545, 818, 1090, 1363, 1636, 1909, 2181, 2454, 2727, 3000, 3272, 3545, 3818, 4091, },  // equal 10v
  { 0, 136, 272, 409, 545, 682, 818, 955, 1091, 1228, 1364, 1501, 1637, 1774, 1910, 2047, 	},    // equal 5v
  { 0, 68, 136, 204, 272, 341, 409, 477, 545, 613, 682, 750, 818, 886, 954, 1023, },        // equal 2.5v
  { 0, 34, 68, 102, 136, 170, 204, 238, 272, 306, 340, 374, 408, 442, 476, 511, 	},          // equal 1.25v
  { 0, 53, 118, 196, 291, 405, 542, 708, 908, 1149, 1441, 1792, 2216, 2728, 3345, 4091, },      // log-ish 10v
  { 0, 136, 272, 409, 545, 682, 818, 955, 1091, 1228, 1364, 1501, 1637, 1774, 1910, 2047, 	},    // log-ish 5v
  { 0, 745, 1362, 1874, 2298, 2649, 2941, 3182, 3382, 3548, 3685, 3799, 3894, 3972, 4037, 4091, },  // exp-ish 10v
  { 0, 372, 681, 937, 1150, 1325, 1471, 1592, 1692, 1775, 1844, 1901, 1948, 1987, 2020, 2047 }    // exp-ish 5v

};

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


static whale_set w;

static edit_modes edit_mode;
static u8 edit_cv_step, edit_cv_ch;
static s8 edit_cv_value;
static u8 edit_prob, live_in, scale_select;
static u8 pattern, next_pattern, pattern_jump;

static u8 series_pos, series_next, series_jump, series_playing, screll, screll_pos;

static u8 key_alt, center;
static u8 held_keys[32], key_count, key_times[256];
static u8 keyfirst_pos;
static s8 keycount_pos, keycount_series, keycount_cv;

static s8 pos, cut_pos, next_pos, drunk_step, triggered;
static u8 cv_chosen[2];
static u16 cv0, cv1;

static u8 param_accept, *param_dest8;
static u16 clip;
static u16 *param_dest;

static u8 series_step;

static u8 SIZE, LENGTH, VARI;

typedef void(*re_t)(void);
static re_t re;

static u8 dirty;

static u8 tr[4];
static u16 param;




////////////////////////////////////////////////////////////////////////////////
// prototypes

static void ww_refresh(void);
static void ww_refresh_mono(void);

// static void handler_KeyTimer(s32 data);




//-------------------------------------------------
//----- extern function definition
void op_ww_init(void* mem) {
  u8 i1,i2;
  //  print_dbg("\r\n op_ww_init ");
  op_ww_t* op = (op_ww_t*)mem;

  // superclass functions
  //--- op
  op->super.in_fn = op_ww_in_fn;
  op->super.pickle = (op_pickle_fn) (&op_ww_pickle);
  op->super.unpickle = (op_unpickle_fn) (&op_ww_unpickle);

      // polled operator superclass
  op->op_poll.handler = (poll_handler_t)(&op_ww_poll_handler);
  op->op_poll.op = op;

  //--- monome
  op->monome.handler = (monome_handler_t)&op_ww_handler;
  op->monome.op = op;

  // superclass state

  op->super.type = eOpWW;
  op->super.flags |= (1 << eOpFlagMonomeGrid);

  op->super.numInputs = 3;
  op->super.numOutputs = 7;

  op->super.in_val = op->in_val;
  op->super.out = op->outs;

  op->super.opString = op_ww_opstring;
  op->super.inString = op_ww_instring;
  op->super.outString = op_ww_outstring;

  op->in_val[0] = &(op->focus);
  op->in_val[1] = &(op->clk);  
  op->in_val[2] = &(op->param);  
  op->outs[0] = -1;
  op->outs[1] = -1;
  op->outs[2] = -1;
  op->outs[3] = -1;
  op->outs[4] = -1;
  op->outs[5] = -1;
  op->outs[6] = -1;


  op->focus = 0;

  re = &ww_refresh;

      // clear out some reasonable defaults
    for(i1=0;i1<16;i1++) {
      for(i2=0;i2<16;i2++) {
        w.wp[i1].steps[i2] = 0;
        w.wp[i1].step_probs[i2] = 255;
        w.wp[i1].cv_probs[0][i2] = 255;
        w.wp[i1].cv_probs[1][i2] = 255;
        w.wp[i1].cv_curves[0][i2] = 0;
        w.wp[i1].cv_curves[1][i2] = 0;
        w.wp[i1].cv_values[i2] = SCALES[2][i2];
        w.wp[i1].cv_steps[0][i2] = 1<<i2;
        w.wp[i1].cv_steps[1][i2] = 1<<i2;
      }
      w.wp[i1].step_choice = 0;
      w.wp[i1].loop_end = 15;
      w.wp[i1].loop_len = 15;
      w.wp[i1].loop_start = 0;
      w.wp[i1].loop_dir = 0;
      w.wp[i1].step_mode = mForward;
      w.wp[i1].cv_mode[0] = 0;
      w.wp[i1].cv_mode[1] = 0;
      w.wp[i1].tr_mode = 0;
    }

    w.series_start = 0;
    w.series_end = 3;

    w.tr_mute[0] = 1;
    w.tr_mute[1] = 1;
    w.tr_mute[2] = 1;
    w.tr_mute[3] = 1;
    w.cv_mute[0] = 1;
    w.cv_mute[1] = 1;

    for(i1=0;i1<64;i1++)
      w.series_list[i1] = 1;

  LENGTH = 15;
  SIZE = 16;
  VARI = 1;







  net_monome_set_focus( &(op->monome), op->focus > 0);
  
  // init monome drawing
  op_ww_redraw();

  op_ww_set_timer(op);
}

// de-init
void op_ww_deinit(void* op) {
  // release focus
  net_monome_set_focus(&(((op_ww_t*)op)->monome), 0);
  op_ww_unset_timer(op);
}

//-------------------------------------------------
//----- static function definition

//--- network input functions
static void op_ww_in_focus(op_ww_t* op, const io_t v) {
  if((v) > 0) {
    op->focus = OP_ONE;
  } else {
    // if(op->focus>0) { net_monome_grid_clear(); }
    op->focus = 0;
  }

  u8 i1;
  // print_dbg("\r\n// monome connect /////////////////"); 
  keycount_pos = 0;
  key_count = 0;
#if BEEKEEP
#else
  SIZE = monome_size_x();
#endif
  LENGTH = SIZE - 1;
  // print_dbg("\r monome size: ");
  // print_dbg_ulong(SIZE);
#if BEEKEEP
#else
  VARI = monome_is_vari();
#endif
  // print_dbg("\r monome vari: ");
  // print_dbg_ulong(VARI);

  if(VARI) re = &ww_refresh;
  else re = &ww_refresh_mono;

  for(i1=0;i1<16;i1++)
    if(w.wp[i1].loop_end > LENGTH)
      w.wp[i1].loop_end = LENGTH;



  // monome_size_x();
  net_monome_set_focus( &(op->monome), op->focus > 0);
}

static void op_ww_in_clock(op_ww_t* op, const io_t v) {
  static u8 i1, count;
  static u16 found[16];

  if(v<0) op->clk = 0;
  else if(v>1) op->clk = 1;
  else op->clk = v;

  if(op->clk == 1) {
    if(pattern_jump) {
      pattern = next_pattern;
      next_pos = w.wp[pattern].loop_start;
      pattern_jump = 0;
    }
    // for series mode and delayed pattern change
    if(series_jump) {
      series_pos = series_next;
      if(series_pos == w.series_end)
        series_next = w.series_start;
      else
        series_next++;

      // print_dbg("\r\nSERIES next ");
      // print_dbg_ulong(series_next);
      // print_dbg(" pos ");
      // print_dbg_ulong(series_pos);

      count = 0;
      for(i1=0;i1<16;i1++) {
        if((w.series_list[series_pos] >> i1) & 1) {
          found[count] = i1;
          count++;
        }
      }

      if(count == 1)
        next_pattern = found[0];
      else {

        next_pattern = found[rnd()%count];
      }

      pattern = next_pattern;
      series_playing = pattern;
      if(w.wp[pattern].step_mode == mReverse)
        next_pos = w.wp[pattern].loop_end;
      else
        next_pos = w.wp[pattern].loop_start;

      series_jump = 0;
      series_step = 0;
    }

    pos = next_pos;

    // live param record
    if(param_accept && live_in) {
      param_dest = &w.wp[pattern].cv_curves[edit_cv_ch][pos];
      w.wp[pattern].cv_curves[edit_cv_ch][pos] = op->param;
    }

    // calc next step
    if(w.wp[pattern].step_mode == mForward) {     // FORWARD
      if(pos == w.wp[pattern].loop_end) next_pos = w.wp[pattern].loop_start;
      else if(pos >= LENGTH) next_pos = 0;
      else next_pos++;
      cut_pos = 0;
    }
    else if(w.wp[pattern].step_mode == mReverse) {  // REVERSE
      if(pos == w.wp[pattern].loop_start)
        next_pos = w.wp[pattern].loop_end;
      else if(pos <= 0)
        next_pos = LENGTH;
      else next_pos--;
      cut_pos = 0;
    }
    else if(w.wp[pattern].step_mode == mDrunk) {  // DRUNK
      drunk_step += (rnd() % 3) - 1; // -1 to 1
      if(drunk_step < -1) drunk_step = -1;
      else if(drunk_step > 1) drunk_step = 1;

      next_pos += drunk_step;
      if(next_pos < 0) 
        next_pos = LENGTH;
      else if(next_pos > LENGTH) 
        next_pos = 0;
      else if(w.wp[pattern].loop_dir == 1 && next_pos < w.wp[pattern].loop_start)
        next_pos = w.wp[pattern].loop_end;
      else if(w.wp[pattern].loop_dir == 1 && next_pos > w.wp[pattern].loop_end)
        next_pos = w.wp[pattern].loop_start;
      else if(w.wp[pattern].loop_dir == 2 && next_pos < w.wp[pattern].loop_start && next_pos > w.wp[pattern].loop_end) {
        if(drunk_step == 1)
          next_pos = w.wp[pattern].loop_start;
        else
          next_pos = w.wp[pattern].loop_end;
      }

      cut_pos = 1;
    }
    else if(w.wp[pattern].step_mode == mRandom) { // RANDOM
      next_pos = (rnd() % (w.wp[pattern].loop_len + 1)) + w.wp[pattern].loop_start;
      // print_dbg("\r\nnext pos:");
      // print_dbg_ulong(next_pos);
      if(next_pos > LENGTH) next_pos -= LENGTH + 1;
      cut_pos = 1;
    }

    // next pattern?
    if(pos == w.wp[pattern].loop_end && w.wp[pattern].step_mode == mForward) {
      if(edit_mode == mSeries) 
        series_jump++;
      else if(next_pattern != pattern)
        pattern_jump++;
    }
    else if(pos == w.wp[pattern].loop_start && w.wp[pattern].step_mode == mReverse) {
      if(edit_mode == mSeries) 
        series_jump++;
      else if(next_pattern != pattern)
        pattern_jump++;
    }
    else if(series_step == w.wp[pattern].loop_len) {
      series_jump++;
    }

    if(edit_mode == mSeries)
      series_step++;


    // TRIGGER
    triggered = 0;
    if((rnd() % 255) < w.wp[pattern].step_probs[pos]) {
      
      if(w.wp[pattern].step_choice & 1<<pos) {
        count = 0;
        for(i1=0;i1<4;i1++)
          if(w.wp[pattern].steps[pos] >> i1 & 1) {
            found[count] = i1;
            count++;
          }

        if(count == 0)
          triggered = 0;
        else if(count == 1)
          triggered = 1<<found[0];
        else
          triggered = 1<<found[rnd()%count];
      } 
      else {
        triggered = w.wp[pattern].steps[pos];
      }
      
      if(w.wp[pattern].tr_mode == 0) {
        if(triggered & 0x1 && w.tr_mute[0]) tr[0] = 1;
        if(triggered & 0x2 && w.tr_mute[1]) tr[1] = 1;
        if(triggered & 0x4 && w.tr_mute[2]) tr[2] = 1;
        if(triggered & 0x8 && w.tr_mute[3]) tr[3] = 1;
      } else {
        if(w.tr_mute[0]) {
          if(triggered & 0x1) tr[0] = 1;
          else tr[0] = 0;
        }
        if(w.tr_mute[1]) {
          if(triggered & 0x2) tr[1] = 1;
          else tr[1] = 0;
        }
        if(w.tr_mute[2]) {
          if(triggered & 0x4) tr[2] = 1;
          else tr[2] = 0;
        }
        if(w.tr_mute[3]) {
          if(triggered & 0x8) tr[3] = 1;
          else tr[3] = 0;
        }

      }
    }

    dirty++;


    // PARAM 0
    if((rnd() % 255) < w.wp[pattern].cv_probs[0][pos] && w.cv_mute[0]) {
      if(w.wp[pattern].cv_mode[0] == 0) {
        cv0 = w.wp[pattern].cv_curves[0][pos];
      }
      else {
        count = 0;
        for(i1=0;i1<16;i1++)
          if(w.wp[pattern].cv_steps[0][pos] & (1<<i1)) {
            found[count] = i1;
            count++;
          }
        if(count == 1) 
          cv_chosen[0] = found[0];
        else
          cv_chosen[0] = found[rnd() % count];
        cv0 = w.wp[pattern].cv_values[cv_chosen[0]];      
      }
    }

    // PARAM 1
    if((rnd() % 255) < w.wp[pattern].cv_probs[1][pos] && w.cv_mute[1]) {
      if(w.wp[pattern].cv_mode[1] == 0) {
        cv1 = w.wp[pattern].cv_curves[1][pos];
      }
      else {
        count = 0;
        for(i1=0;i1<16;i1++)
          if(w.wp[pattern].cv_steps[1][pos] & (1<<i1)) {
            found[count] = i1;
            count++;
          }
        if(count == 1) 
          cv_chosen[1] = found[0];
        else
          cv_chosen[1] = found[rnd() % count];

        cv1 = w.wp[pattern].cv_values[cv_chosen[1]];      
      }
    }
 
  }
  else {
    if(w.wp[pattern].tr_mode == 0) {
      tr[0] = 0;
      tr[1] = 0;
      tr[2] = 0;
      tr[3] = 0;
    }
  }



  if(tr[0] != op->outs[0]) net_activate(op->outs[0], tr[0], op);
  if(tr[1] != op->outs[1]) net_activate(op->outs[1], tr[1], op);
  if(tr[2] != op->outs[2]) net_activate(op->outs[2], tr[2], op);
  if(tr[3] != op->outs[3]) net_activate(op->outs[3], tr[3], op);
  if(cv0 != op->outs[4]) net_activate(op->outs[4], cv0, op);
  if(cv1 != op->outs[5]) net_activate(op->outs[5], cv1, op);
  // print_dbg("\r\n pos: ");
  // print_dbg_ulong(pos);
  net_activate(op->outs[6], pos, op);
}

static void op_ww_in_param(op_ww_t* op, const io_t v) {
  if(v < 0) op->param = 0;
  else if(v > 4095) op->param = 4095;
  else op->param = v;

  param = op->param;

  u8 i;

  // PARAM POT INPUT
  if(param_accept && edit_prob) {
    *param_dest8 = op->param >> 4; // scale to 0-255;
    // print_dbg("\r\nnew prob: ");
    // print_dbg_ulong(*param_dest8);
    // print_dbg("\t" );
    // print_dbg_ulong(adc[1]);
  }
  else if(param_accept) {
    *param_dest = op->param;
    dirty++;
  }
  else if(screll) {
    i = op->param>>6;
    if(i > 58) i = 58;
    if(i != screll_pos) {
      screll_pos = i;
      dirty++;
      // print_dbg("\r screll pos: ");
      // print_dbg_ulong(screll_pos);
    }
  }



// net_activate(op->outs[i], 1, op);
}




// poll event handler
static void op_ww_poll_handler(void* op) {
  static u16 i1,x,n1;
  op_ww_t* ww = (op_ww_t*)op;
  if(ww->focus != 0) {

    for(i1=0;i1<key_count;i1++) {
      if(key_times[held_keys[i1]])
      if(--key_times[held_keys[i1]]==0) {
        if(edit_mode != mSeries) {
          // preset copy
          if(held_keys[i1] / 16 == 2) {
            x = held_keys[i1] % 16;
            for(n1=0;n1<16;n1++) {
              w.wp[x].steps[n1] = w.wp[pattern].steps[n1];
              w.wp[x].step_probs[n1] = w.wp[pattern].step_probs[n1];
              w.wp[x].cv_values[n1] = w.wp[pattern].cv_values[n1];
              w.wp[x].cv_steps[0][n1] = w.wp[pattern].cv_steps[0][n1];
              w.wp[x].cv_curves[0][n1] = w.wp[pattern].cv_curves[0][n1];
              w.wp[x].cv_probs[0][n1] = w.wp[pattern].cv_probs[0][n1];
              w.wp[x].cv_steps[1][n1] = w.wp[pattern].cv_steps[1][n1];
              w.wp[x].cv_curves[1][n1] = w.wp[pattern].cv_curves[1][n1];
              w.wp[x].cv_probs[1][n1] = w.wp[pattern].cv_probs[1][n1];
            }

            w.wp[x].cv_mode[0] = w.wp[pattern].cv_mode[0];
            w.wp[x].cv_mode[1] = w.wp[pattern].cv_mode[1];

            w.wp[x].loop_start = w.wp[pattern].loop_start;
            w.wp[x].loop_end = w.wp[pattern].loop_end;
            w.wp[x].loop_len = w.wp[pattern].loop_len;
            w.wp[x].loop_dir = w.wp[pattern].loop_dir;

            pattern = x;
            next_pattern = x;
            dirty++;

            // print_dbg("\r\n saved pattern: ");
            // print_dbg_ulong(x);
          }
        }

        // print_dbg("\rlong press: "); 
        // print_dbg_ulong(held_keys[i1]);
      }
    }
    
    if(dirty) op_ww_redraw();

    dirty = 0;
  }
}


// process monome key input
static void op_ww_handler(op_monome_t* op_monome, u32 edata) {
  u8 x, y, z, index, i1, found, count;
  s16 delta;
  monome_grid_key_parse_event_data(edata, &x, &y, &z);
  // print_dbg("\r\n monome event; x: "); 
  // print_dbg_hex(x); 
  // print_dbg("; y: 0x"); 
  // print_dbg_hex(y); 
  // print_dbg("; z: 0x"); 
  // print_dbg_hex(z);

  //// TRACK LONG PRESSES
  index = y*16 + x;
  if(z) {
    held_keys[key_count] = index;
    key_count++;
    key_times[index] = 10;    //// THRESHOLD key hold time
  } else {
    found = 0; // "found"
    for(i1 = 0; i1<key_count; i1++) {
      if(held_keys[i1] == index) 
        found++;
      if(found) 
        held_keys[i1] = held_keys[i1+1];
    }
    key_count--;

    // FAST PRESS
    if(key_times[index] > 0) {
      if(edit_mode != mSeries) {
        if(index/16 == 2) {
          i1 = index % 16;
          if(key_alt)
            next_pattern = i1;
          else {
            pattern = i1;
            next_pattern = i1;
          }
        }
      }
      // print_dbg("\r\nfast press: ");
      // print_dbg_ulong(index);
      // print_dbg(": ");
      // print_dbg_ulong(key_times[index]);
    }
  }


    // OPTIMIZE: order this if-branch by common priority/use
    //// SORT

  // cut position
  if(y == 1) {
    keycount_pos += z * 2 - 1;
    if(keycount_pos < 0) keycount_pos = 0;
    // print_dbg("\r\nkeycount: "); 
    // print_dbg_ulong(keycount_pos);

    if(keycount_pos == 1 && z) {
      if(key_alt == 0) {
        next_pos = x;
        cut_pos++;
        dirty++;
        keyfirst_pos = x;
      }
      else if(key_alt == 1) {
        if(x == LENGTH)
          w.wp[pattern].step_mode = mForward;
        else if(x == LENGTH-1)
          w.wp[pattern].step_mode = mReverse;
        else if(x == LENGTH-2)
          w.wp[pattern].step_mode = mDrunk;
        else if(x == LENGTH-3)
          w.wp[pattern].step_mode = mRandom;
        // FIXME
        else if(x == 0) {
          if(pos == w.wp[pattern].loop_start)
            next_pos = w.wp[pattern].loop_end;
          else if(pos == 0)
            next_pos = LENGTH;
          else next_pos--;
          cut_pos = 1;
          dirty++;
        }
        // FIXME
        else if(x == 1) {
          if(pos == w.wp[pattern].loop_end) next_pos = w.wp[pattern].loop_start;
          else if(pos == LENGTH) next_pos = 0;
          else next_pos++;
          cut_pos = 1;
          dirty++;
        }
        else if(x == 2 ) {
          next_pos = (rnd() % (w.wp[pattern].loop_len + 1)) + w.wp[pattern].loop_start;
          cut_pos = 1;
          dirty++;         
        }
      }
    }
    else if(keycount_pos == 2 && z) {
      w.wp[pattern].loop_start = keyfirst_pos;
      w.wp[pattern].loop_end = x;
      dirty++;
      if(w.wp[pattern].loop_start > w.wp[pattern].loop_end) w.wp[pattern].loop_dir = 2;
      else if(w.wp[pattern].loop_start == 0 && w.wp[pattern].loop_end == LENGTH) w.wp[pattern].loop_dir = 0;
      else w.wp[pattern].loop_dir = 1;

      w.wp[pattern].loop_len = w.wp[pattern].loop_end - w.wp[pattern].loop_start;

      if(w.wp[pattern].loop_dir == 2)
        w.wp[pattern].loop_len = (LENGTH - w.wp[pattern].loop_start) + w.wp[pattern].loop_end + 1;

      // print_dbg("\r\nloop_len: "); 
      // print_dbg_ulong(w.wp[pattern].loop_len);
    }
  }

  // top row
  else if(y == 0) {
    if(x == LENGTH) {
      key_alt = z;
      if(z == 0) {
        param_accept = 0;
        live_in = 0;
      }
      dirty++;
    }
    else if(x < 4 && z) {
      if(key_alt)
        w.wp[pattern].tr_mode ^= 1;
      else if(screll)
        w.tr_mute[x] ^= 1;
      else edit_mode = mTrig;
      edit_prob = 0;
      param_accept = 0;
      dirty++;
    }
    else if(SIZE==16 && x > 3 && x < 12 && z) {
      param_accept = 0;
      edit_cv_ch = (x-4)/4;
      edit_mode = mMap;
      edit_prob = 0;

      if(key_alt)
        w.wp[pattern].cv_mode[edit_cv_ch] ^= 1;
      else if(screll)
        w.cv_mute[edit_cv_ch] ^= 1;

      dirty++;
    }
    else if(SIZE==8 && (x == 4 || x == 5) && z) {
      param_accept = 0;
      edit_cv_ch = x-4;
      edit_mode = mMap;
      edit_prob = 0;

      if(key_alt)
        w.wp[pattern].cv_mode[edit_cv_ch] ^= 1;
      else if(screll)
        w.cv_mute[edit_cv_ch] ^= 1;

      dirty++;
    }
    else if(x == LENGTH-1 && z && key_alt) {
      edit_mode = mSeries;
      dirty++;
    }
    else if(x == LENGTH-1)
      screll = z;
  }


  // toggle steps and prob control
  else if(edit_mode == mTrig) {
    if(z && y>3 && edit_prob == 0) {
      if(key_alt)
        w.wp[pattern].steps[pos] |=  1 << (y-4);
      else if(screll) {
        w.wp[pattern].step_choice ^= (1<<x);
      }
      else
        w.wp[pattern].steps[x] ^= (1<<(y-4));
      dirty++;
    }
    // step probs
    else if(z && y==3) {
      if(key_alt)
        edit_prob = 1;
      else {
        if(w.wp[pattern].step_probs[x] == 255) w.wp[pattern].step_probs[x] = 0;
        else w.wp[pattern].step_probs[x] = 255;
      } 
      dirty++;
    }
    else if(edit_prob == 1) {
      if(z) {
        if(y == 4) w.wp[pattern].step_probs[x] = 192;
        else if(y == 5) w.wp[pattern].step_probs[x] = 128;
        else if(y == 6) w.wp[pattern].step_probs[x] = 64;
        else w.wp[pattern].step_probs[x] = 0;
      }
    }
  } 
  
  // edit map and probs
  else if(edit_mode == mMap) {
    // step probs
    if(z && y==3) {
      if(key_alt)
        edit_prob = 1;
      else  {
        if(w.wp[pattern].cv_probs[edit_cv_ch][x] == 255) w.wp[pattern].cv_probs[edit_cv_ch][x] = 0;
        else w.wp[pattern].cv_probs[edit_cv_ch][x] = 255;
      }
        
      dirty++;
    }
    // edit data
    else if(edit_prob == 0) {
      // CURVES
      if(w.wp[pattern].cv_mode[edit_cv_ch] == 0) {
        if(y == 4 && z) {
          if(center) 
            delta = 3;
          else if(key_alt)
            delta = 409;
          else            
            delta = 34;

          // saturate
          if(w.wp[pattern].cv_curves[edit_cv_ch][x] + delta < 4092)
            w.wp[pattern].cv_curves[edit_cv_ch][x] += delta;
          else
            w.wp[pattern].cv_curves[edit_cv_ch][x] = 4092;
        }
        else if(y == 6 && z) {
          if(center)
            delta = 3;
          else if(key_alt)
            delta = 409;
          else
            delta = 34;

          // saturate
          if(w.wp[pattern].cv_curves[edit_cv_ch][x] > delta)
            w.wp[pattern].cv_curves[edit_cv_ch][x] -= delta;
          else
            w.wp[pattern].cv_curves[edit_cv_ch][x] = 0;

        }
        else if(y == 5) {
          if(z == 1) {
            center = 1;
            if(key_alt)
              w.wp[pattern].cv_curves[edit_cv_ch][x] = clip;
            else
              clip = w.wp[pattern].cv_curves[edit_cv_ch][x];
          }
          else
            center = 0;
        }
        else if(y == 7) {
          if(key_alt && z) {
            param_dest = &w.wp[pattern].cv_curves[edit_cv_ch][pos];
            w.wp[pattern].cv_curves[edit_cv_ch][pos] = param;
            param_accept = 1;
            live_in = 1;
          }
          else if(center && z) {
            w.wp[pattern].cv_curves[edit_cv_ch][x] = rand() % 4092;
          }
          else {
            param_accept = z;
            param_dest = &w.wp[pattern].cv_curves[edit_cv_ch][x];
            if(z) w.wp[pattern].cv_curves[edit_cv_ch][x] = param;
          }
          dirty++;
        }
      }
      // MAP
      else {
        if(scale_select && z) {
          // index -= 64;
          index = (y-4) * 8 + x;
          if(index < 24 && y<8) {
            for(i1=0;i1<16;i1++)
              w.wp[pattern].cv_values[i1] = SCALES[index][i1];
            print_dbg("\rNEW SCALE ");
            print_dbg_ulong(index);
          }

          scale_select = 0;
          dirty++;
        }
        else {
          if(z && y==4) {
            edit_cv_step = x;
            count = 0;
            for(i1=0;i1<16;i1++)
                if((w.wp[pattern].cv_steps[edit_cv_ch][edit_cv_step] >> i1) & 1) {
                  count++;
                  edit_cv_value = i1;
                }
            if(count>1)
              edit_cv_value = -1;

            keycount_cv = 0;

            dirty++;
          }
          // load scale
          else if(key_alt && y==7 && x == 0 && z) {
            scale_select++;
            dirty++;
          }
          // read pot         
          else if(y==7 && key_alt && edit_cv_value != -1 && x==LENGTH) {
            param_accept = z;
            param_dest = &(w.wp[pattern].cv_values[edit_cv_value]);
            // print_dbg("\r\nparam: ");
            // print_dbg_ulong(*param_dest);
          }
          else if((y == 5 || y == 6) && z && x<4 && edit_cv_step != -1) {
            delta = 0;
            if(x == 0)
              delta = 409;
            else if(x == 1)
              delta = 239;
            else if(x == 2)
              delta = 34;
            else if(x == 3)
              delta = 3;

            if(y == 6)
              delta *= -1;
            
            if(key_alt) {
              for(i1=0;i1<16;i1++) {
                if(w.wp[pattern].cv_values[i1] + delta > 4092)
                  w.wp[pattern].cv_values[i1] = 4092;
                else if(delta < 0 && w.wp[pattern].cv_values[i1] < -1*delta)
                  w.wp[pattern].cv_values[i1] = 0;
                else
                  w.wp[pattern].cv_values[i1] += delta;
              }
            }
            else {
              if(w.wp[pattern].cv_values[edit_cv_value] + delta > 4092)
                w.wp[pattern].cv_values[edit_cv_value] = 4092;
              else if(delta < 0 && w.wp[pattern].cv_values[edit_cv_value] < -1*delta)
                w.wp[pattern].cv_values[edit_cv_value] = 0;
              else
                w.wp[pattern].cv_values[edit_cv_value] += delta;
            }

            dirty++;
          }
          // choose values
          else if(y==7) {
            keycount_cv += z*2-1;
            if(keycount_cv < 0)
              keycount_cv = 0;

            if(z) {
              count = 0;
              for(i1=0;i1<16;i1++)
                if((w.wp[pattern].cv_steps[edit_cv_ch][edit_cv_step] >> i1) & 1)
                  count++;

              // single press toggle
              if(keycount_cv == 1 && count < 2) {
                w.wp[pattern].cv_steps[edit_cv_ch][edit_cv_step] = (1<<x);
                edit_cv_value = x;
              }
              // multiselect
              else if(keycount_cv > 1 || count > 1) {
                w.wp[pattern].cv_steps[edit_cv_ch][edit_cv_step] ^= (1<<x);

                if(!w.wp[pattern].cv_steps[edit_cv_ch][edit_cv_step])
                  w.wp[pattern].cv_steps[edit_cv_ch][edit_cv_step] = (1<<x);

                count = 0;
                for(i1=0;i1<16;i1++)
                  if((w.wp[pattern].cv_steps[edit_cv_ch][edit_cv_step] >> i1) & 1) {
                    count++;
                    edit_cv_value = i1;
                  }

                if(count > 1)
                  edit_cv_value = -1;
              }

              dirty++;
            }
          }
        }
      }
    }
    else if(edit_prob == 1) {
      if(z) {
        if(y == 4) w.wp[pattern].cv_probs[edit_cv_ch][x] = 192;
        else if(y == 5) w.wp[pattern].cv_probs[edit_cv_ch][x] = 128;
        else if(y == 6) w.wp[pattern].cv_probs[edit_cv_ch][x] = 64;
        else w.wp[pattern].cv_probs[edit_cv_ch][x] = 0;
      }
    }
  }

  // series mode
  else if(edit_mode == mSeries) {
    if(z && key_alt) {
      if(x == 0)
        series_next = y-2+screll_pos;
      else if(x == LENGTH-1)
        w.series_start = y-2+screll_pos;
      else if(x == LENGTH)
        w.series_end = y-2+screll_pos;

      if(w.series_end < w.series_start)
        w.series_end = w.series_start;
    }
    else {
      keycount_series += z*2-1;
      if(keycount_series < 0)
        keycount_series = 0;

      if(z) {
        count = 0;
        for(i1=0;i1<16;i1++)
          count += (w.series_list[y-2+screll_pos] >> i1) & 1;

        // single press toggle
        if(keycount_series == 1 && count < 2) {
          w.series_list[y-2+screll_pos] = (1<<x);
        }
        // multi-select
        else if(keycount_series > 1 || count > 1) {
          w.series_list[y-2+screll_pos] ^= (1<<x);

          // ensure not fully clear
          if(!w.series_list[y-2+screll_pos])
            w.series_list[y-2+screll_pos] = (1<<x);
        }
      }
    }

    dirty++;
  }
}



// redraw monome grid
static void op_ww_redraw() {
  (*re)();
}



static void ww_refresh(void) {
  u8 i1,i2;


  // clear top, cut, pattern, prob
  for(i1=0;i1<16;i1++) {
    monomeLedBuffer[i1] = 0;
    monomeLedBuffer[16+i1] = 0;
    monomeLedBuffer[32+i1] = 4;
    monomeLedBuffer[48+i1] = 0;
  }

  // dim mode
  if(edit_mode == mTrig) {
    monomeLedBuffer[0] = 4;
    monomeLedBuffer[1] = 4;
    monomeLedBuffer[2] = 4;
    monomeLedBuffer[3] = 4;
  }
  else if(edit_mode == mMap) {
    if(SIZE==16) {
      monomeLedBuffer[4+(edit_cv_ch*4)] = 4;
      monomeLedBuffer[5+(edit_cv_ch*4)] = 4;
      monomeLedBuffer[6+(edit_cv_ch*4)] = 4;
      monomeLedBuffer[7+(edit_cv_ch*4)] = 4;
    }
    else
      monomeLedBuffer[4+edit_cv_ch] = 4;
  }
  else if(edit_mode == mSeries) {
    monomeLedBuffer[LENGTH-1] = 7;
  }

  // alt
  monomeLedBuffer[LENGTH] = 4;
  if(key_alt) monomeLedBuffer[LENGTH] = 11;

  // show mutes or on steps
  if(screll) {
    if(w.tr_mute[0]) monomeLedBuffer[0] = 11;
    if(w.tr_mute[1]) monomeLedBuffer[1] = 11;
    if(w.tr_mute[2]) monomeLedBuffer[2] = 11;
    if(w.tr_mute[3]) monomeLedBuffer[3] = 11;
  }
  else if(triggered) {
    if(triggered & 0x1 && w.tr_mute[0]) monomeLedBuffer[0] = 11 - 4 * w.wp[pattern].tr_mode;
    if(triggered & 0x2 && w.tr_mute[1]) monomeLedBuffer[1] = 11 - 4 * w.wp[pattern].tr_mode;
    if(triggered & 0x4 && w.tr_mute[2]) monomeLedBuffer[2] = 11 - 4 * w.wp[pattern].tr_mode;
    if(triggered & 0x8 && w.tr_mute[3]) monomeLedBuffer[3] = 11 - 4 * w.wp[pattern].tr_mode;
  }

  // cv indication
  if(screll) {
    if(SIZE==16) {
      if(w.cv_mute[0]) {
        monomeLedBuffer[4] = 11;
        monomeLedBuffer[5] = 11;
        monomeLedBuffer[6] = 11;
        monomeLedBuffer[7] = 11;
      }
      if(w.cv_mute[1]) {
        monomeLedBuffer[8] = 11;
        monomeLedBuffer[9] = 11;
        monomeLedBuffer[10] = 11;
        monomeLedBuffer[11] = 11;
      }
    }
    else {
      if(w.cv_mute[0])
        monomeLedBuffer[4] = 11;
      if(w.cv_mute[1])
        monomeLedBuffer[5] = 11;
    }

  }
  else if(SIZE==16) {
    monomeLedBuffer[cv0 / 1024 + 4] = 11;
    monomeLedBuffer[cv1 / 1024 + 8] = 11;
  }

  // show pos loop dim
  if(w.wp[pattern].loop_dir) {  
    for(i1=0;i1<SIZE;i1++) {
      if(w.wp[pattern].loop_dir == 1 && i1 >= w.wp[pattern].loop_start && i1 <= w.wp[pattern].loop_end)
        monomeLedBuffer[16+i1] = 4;
      else if(w.wp[pattern].loop_dir == 2 && (i1 <= w.wp[pattern].loop_end || i1 >= w.wp[pattern].loop_start)) 
        monomeLedBuffer[16+i1] = 4;
    }
  }

  // show position and next cut
  if(cut_pos) monomeLedBuffer[16+next_pos] = 7;
  monomeLedBuffer[16+pos] = 15;

  // show pattern
  monomeLedBuffer[32+pattern] = 11;
  if(pattern != next_pattern) monomeLedBuffer[32+next_pattern] = 7;

  // show step data
  if(edit_mode == mTrig) {
    if(edit_prob == 0) {
      for(i1=0;i1<SIZE;i1++) {
        for(i2=0;i2<4;i2++) {
          if((w.wp[pattern].steps[i1] & (1<<i2)) && i1 == pos && (triggered & 1<<i2) && w.tr_mute[i2]) monomeLedBuffer[(i2+4)*16+i1] = 11;
          else if(w.wp[pattern].steps[i1] & (1<<i2) && (w.wp[pattern].step_choice & 1<<i1)) monomeLedBuffer[(i2+4)*16+i1] = 4;
          else if(w.wp[pattern].steps[i1] & (1<<i2)) monomeLedBuffer[(i2+4)*16+i1] = 7;
          else if(i1 == pos) monomeLedBuffer[(i2+4)*16+i1] = 4;
          else monomeLedBuffer[(i2+4)*16+i1] = 0;
        }

        // probs
        if(w.wp[pattern].step_probs[i1] == 255) monomeLedBuffer[48+i1] = 11;
        else if(w.wp[pattern].step_probs[i1] > 0) monomeLedBuffer[48+i1] = 4;
      }
    }
    else if(edit_prob == 1) {
      for(i1=0;i1<SIZE;i1++) {
        monomeLedBuffer[64+i1] = 4;
        monomeLedBuffer[80+i1] = 4;
        monomeLedBuffer[96+i1] = 4;
        monomeLedBuffer[112+i1] = 4;

        if(w.wp[pattern].step_probs[i1] == 255)
          monomeLedBuffer[48+i1] = 11;
        else if(w.wp[pattern].step_probs[i1] == 0) {
          monomeLedBuffer[48+i1] = 0;
          monomeLedBuffer[112+i1] = 7;
        }
        else if(w.wp[pattern].step_probs[i1]) {
          monomeLedBuffer[48+i1] = 4;
          monomeLedBuffer[64+16*(3-(w.wp[pattern].step_probs[i1]>>6))+i1] = 7;
        }
      }
    }
  }

  // show map
  else if(edit_mode == mMap) {
    if(edit_prob == 0) {
      // CURVES
      if(w.wp[pattern].cv_mode[edit_cv_ch] == 0) {
        for(i1=0;i1<SIZE;i1++) {
          // probs
          if(w.wp[pattern].cv_probs[edit_cv_ch][i1] == 255) monomeLedBuffer[48+i1] = 11;
          else if(w.wp[pattern].cv_probs[edit_cv_ch][i1] > 0) monomeLedBuffer[48+i1] = 7;

          monomeLedBuffer[112+i1] = (w.wp[pattern].cv_curves[edit_cv_ch][i1] > 1023) * 7;
          monomeLedBuffer[96+i1] = (w.wp[pattern].cv_curves[edit_cv_ch][i1] > 2047) * 7;
          monomeLedBuffer[80+i1] = (w.wp[pattern].cv_curves[edit_cv_ch][i1] > 3071) * 7;
          monomeLedBuffer[64+i1] = 0;
          monomeLedBuffer[64+16*(3-(w.wp[pattern].cv_curves[edit_cv_ch][i1]>>10))+i1] = (w.wp[pattern].cv_curves[edit_cv_ch][i1]>>7) & 0x7;
        }

        // play step highlight
        monomeLedBuffer[64+pos] += 4;
        monomeLedBuffer[80+pos] += 4;
        monomeLedBuffer[96+pos] += 4;
        monomeLedBuffer[112+pos] += 4;
      }
      // MAP
      else {
        if(!scale_select) {
          for(i1=0;i1<SIZE;i1++) {
            // probs
            if(w.wp[pattern].cv_probs[edit_cv_ch][i1] == 255) monomeLedBuffer[48+i1] = 11;
            else if(w.wp[pattern].cv_probs[edit_cv_ch][i1] > 0) monomeLedBuffer[48+i1] = 7;

            // clear edit select line
            monomeLedBuffer[64+i1] = 4;

            // show current edit value, selected
            if(edit_cv_value != -1) {
              if((w.wp[pattern].cv_values[edit_cv_value] >> 8) >= i1)
                monomeLedBuffer[80+i1] = 7;
              else
                monomeLedBuffer[80+i1] = 0;

              if(((w.wp[pattern].cv_values[edit_cv_value] >> 4) & 0xf) >= i1)
                monomeLedBuffer[96+i1] = 4;
              else
                monomeLedBuffer[96+i1] = 0;
            }
            else {
              monomeLedBuffer[80+i1] = 0;
              monomeLedBuffer[96+i1] = 0;
            }

            // show steps
            if(w.wp[pattern].cv_steps[edit_cv_ch][edit_cv_step] & (1<<i1)) monomeLedBuffer[112+i1] = 7;
            else monomeLedBuffer[112+i1] = 0;
          }

          // show play position
          monomeLedBuffer[64+pos] = 7;
          // show edit position
          monomeLedBuffer[64+edit_cv_step] = 11;
          // show playing note
          monomeLedBuffer[112+cv_chosen[edit_cv_ch]] = 11;
        }
        else {
          for(i1=0;i1<SIZE;i1++) {
            // probs
            if(w.wp[pattern].cv_probs[edit_cv_ch][i1] == 255) monomeLedBuffer[48+i1] = 11;
            else if(w.wp[pattern].cv_probs[edit_cv_ch][i1] > 0) monomeLedBuffer[48+i1] = 7;

            monomeLedBuffer[64+i1] = (i1<8) * 4;            
            monomeLedBuffer[80+i1] = (i1<8) * 4;            
            monomeLedBuffer[96+i1] = (i1<8) * 4;            
            monomeLedBuffer[112+i1] = 0;
          }

          monomeLedBuffer[112] = 7;
        }

      }
    }
    else if(edit_prob == 1) {
      for(i1=0;i1<SIZE;i1++) {
        monomeLedBuffer[64+i1] = 4;
        monomeLedBuffer[80+i1] = 4;
        monomeLedBuffer[96+i1] = 4;
        monomeLedBuffer[112+i1] = 4;

        if(w.wp[pattern].cv_probs[edit_cv_ch][i1] == 255)
          monomeLedBuffer[48+i1] = 11;
        else if(w.wp[pattern].cv_probs[edit_cv_ch][i1] == 0) {
          monomeLedBuffer[48+i1] = 0;
          monomeLedBuffer[112+i1] = 7;
        }
        else if(w.wp[pattern].cv_probs[edit_cv_ch][i1]) {
          monomeLedBuffer[48+i1] = 4;
          monomeLedBuffer[64+16*(3-(w.wp[pattern].cv_probs[edit_cv_ch][i1]>>6))+i1] = 7;
        }
      }
    }

  }

  // series
  else if(edit_mode == mSeries) {
    for(i1 = 0;i1<6;i1++) {
      for(i2=0;i2<SIZE;i2++) {
        // start/end bars, clear
        if(i1+screll_pos == w.series_start || i1+screll_pos == w.series_end) monomeLedBuffer[32+i1*16+i2] = 4;
        else monomeLedBuffer[32+i1*16+i2] = 0;
      }

      // screll position helper
      monomeLedBuffer[32+i1*16+((screll_pos+i1)/(64/SIZE))] = 4;
      
      // sidebar selection indicators
      if(i1+screll_pos > w.series_start && i1+screll_pos < w.series_end) {
        monomeLedBuffer[32+i1*16] = 4;
        monomeLedBuffer[32+i1*16+LENGTH] = 4;
      }

      for(i2=0;i2<SIZE;i2++) {
        // show possible states
        if((w.series_list[i1+screll_pos] >> i2) & 1)
          monomeLedBuffer[32+(i1*16)+i2] = 7;
      }

    }

    // highlight playhead
    if(series_pos >= screll_pos && series_pos < screll_pos+6) {
      monomeLedBuffer[32+(series_pos-screll_pos)*16+series_playing] = 11;
    }
  }

  monome_set_quadrant_flag(0);
  monome_set_quadrant_flag(1);
}


static void ww_refresh_mono(void) {
  u8 i1,i2;

  // clear top, cut, pattern, prob
  for(i1=0;i1<16;i1++) {
    monomeLedBuffer[i1] = 0;
    monomeLedBuffer[16+i1] = 0;
    monomeLedBuffer[32+i1] = 0;
    monomeLedBuffer[48+i1] = 0;
  }

  // show mode
  if(edit_mode == mTrig) {
    monomeLedBuffer[0] = 11;
    monomeLedBuffer[1] = 11;
    monomeLedBuffer[2] = 11;
    monomeLedBuffer[3] = 11;
  }
  else if(edit_mode == mMap) {
    if(SIZE==16) {
      monomeLedBuffer[4+(edit_cv_ch*4)] = 11;
      monomeLedBuffer[5+(edit_cv_ch*4)] = 11;
      monomeLedBuffer[6+(edit_cv_ch*4)] = 11;
      monomeLedBuffer[7+(edit_cv_ch*4)] = 11;
    }
    else
      monomeLedBuffer[4+edit_cv_ch] = 11;
  }
  else if(edit_mode == mSeries) {
    monomeLedBuffer[LENGTH-1] = 11;
  }

  if(screll) {
    monomeLedBuffer[0] = 11 * w.tr_mute[0];
    monomeLedBuffer[1] = 11 * w.tr_mute[1];
    monomeLedBuffer[2] = 11 * w.tr_mute[2];
    monomeLedBuffer[3] = 11 * w.tr_mute[3];

    if(SIZE == 16) {
      monomeLedBuffer[4] = 11 * w.cv_mute[0];
      monomeLedBuffer[5] = 11 * w.cv_mute[0];
      monomeLedBuffer[6] = 11 * w.cv_mute[0];
      monomeLedBuffer[7] = 11 * w.cv_mute[0];
      monomeLedBuffer[8] = 11 * w.cv_mute[1];
      monomeLedBuffer[9] = 11 * w.cv_mute[1];
      monomeLedBuffer[10] = 11 * w.cv_mute[1];
      monomeLedBuffer[11] = 11 * w.cv_mute[1];
    } else {
      monomeLedBuffer[4] = 11 * w.cv_mute[0];
      monomeLedBuffer[5] = 11 * w.cv_mute[1];
    }


  }

  // alt
  if(key_alt) monomeLedBuffer[LENGTH] = 11;

  // show position
  monomeLedBuffer[16+pos] = 15;

  // show pattern
  monomeLedBuffer[32+pattern] = 11;

  // show step data
  if(edit_mode == mTrig) {
    if(edit_prob == 0) {
      for(i1=0;i1<SIZE;i1++) {
        for(i2=0;i2<4;i2++) {
          if(w.wp[pattern].steps[i1] & (1<<i2)) monomeLedBuffer[(i2+4)*16+i1] = 11;
          else monomeLedBuffer[(i2+4)*16+i1] = 0;
        }

        // probs
        if(w.wp[pattern].step_probs[i1] > 0) monomeLedBuffer[48+i1] = 11;
      }
    }
    else if(edit_prob == 1) {
      for(i1=0;i1<SIZE;i1++) {
        monomeLedBuffer[64+i1] = 0;
        monomeLedBuffer[80+i1] = 0;
        monomeLedBuffer[96+i1] = 0;
        monomeLedBuffer[112+i1] = 0;

        if(w.wp[pattern].step_probs[i1] == 255)
          monomeLedBuffer[48+i1] = 11;
        else if(w.wp[pattern].step_probs[i1] == 0) {
          monomeLedBuffer[48+i1] = 0;
          monomeLedBuffer[112+i1] = 11;
        }
        else if(w.wp[pattern].step_probs[i1]) {
          monomeLedBuffer[48+i1] = 11;
          monomeLedBuffer[64+16*(3-(w.wp[pattern].step_probs[i1]>>6))+i1] = 11;
        }
      }
    }
  }

  // show map
  else if(edit_mode == mMap) {
    if(edit_prob == 0) {
      // CURVES
      if(w.wp[pattern].cv_mode[edit_cv_ch] == 0) {
        for(i1=0;i1<SIZE;i1++) {
          // probs
          if(w.wp[pattern].cv_probs[edit_cv_ch][i1] > 0) monomeLedBuffer[48+i1] = 11;

          monomeLedBuffer[112+i1] = (w.wp[pattern].cv_curves[edit_cv_ch][i1] > 511) * 11;
          monomeLedBuffer[96+i1] = (w.wp[pattern].cv_curves[edit_cv_ch][i1] > 1535) * 11;
          monomeLedBuffer[80+i1] = (w.wp[pattern].cv_curves[edit_cv_ch][i1] > 2559) * 11;
          monomeLedBuffer[64+i1] = (w.wp[pattern].cv_curves[edit_cv_ch][i1] > 3583) * 11;
        }
      }
      // MAP
      else {
        if(!scale_select) {
          for(i1=0;i1<SIZE;i1++) {
            // probs
            if(w.wp[pattern].cv_probs[edit_cv_ch][i1] > 0) monomeLedBuffer[48+i1] = 11;

            // clear edit row
            monomeLedBuffer[64+i1] = 0;

            // show current edit value, selected
            if(edit_cv_value != -1) {
              if((w.wp[pattern].cv_values[edit_cv_value] >> 8) >= i1)
                monomeLedBuffer[80+i1] = 11;
              else
                monomeLedBuffer[80+i1] = 0;

              if(((w.wp[pattern].cv_values[edit_cv_value] >> 4) & 0xf) >= i1)
                monomeLedBuffer[96+i1] = 11;
              else
                monomeLedBuffer[96+i1] = 0;
            }
            else {
              monomeLedBuffer[80+i1] = 0;
              monomeLedBuffer[96+i1] = 0;
            }

            // show steps
            if(w.wp[pattern].cv_steps[edit_cv_ch][edit_cv_step] & (1<<i1)) monomeLedBuffer[112+i1] = 11;
            else monomeLedBuffer[112+i1] = 0;
          }

          // show edit position
          monomeLedBuffer[64+edit_cv_step] = 11;
          // show playing note
          monomeLedBuffer[112+cv_chosen[edit_cv_ch]] = 11;
        }
        else {
          for(i1=0;i1<SIZE;i1++) {
            // probs
            if(w.wp[pattern].cv_probs[edit_cv_ch][i1] > 0) monomeLedBuffer[48+i1] = 11;

            monomeLedBuffer[64+i1] = 0;           
            monomeLedBuffer[80+i1] = 0;           
            monomeLedBuffer[96+i1] = 0;           
            monomeLedBuffer[112+i1] = 0;
          }

          monomeLedBuffer[112] = 11;
        }

      }
    }
    else if(edit_prob == 1) {
      for(i1=0;i1<SIZE;i1++) {
        monomeLedBuffer[64+i1] = 0;
        monomeLedBuffer[80+i1] = 0;
        monomeLedBuffer[96+i1] = 0;
        monomeLedBuffer[112+i1] = 0;

        if(w.wp[pattern].cv_probs[edit_cv_ch][i1] == 255)
          monomeLedBuffer[48+i1] = 11;
        else if(w.wp[pattern].cv_probs[edit_cv_ch][i1] == 0) {
          monomeLedBuffer[48+i1] = 0;
          monomeLedBuffer[112+i1] = 11;
        }
        else if(w.wp[pattern].cv_probs[edit_cv_ch][i1]) {
          monomeLedBuffer[48+i1] = 11;
          monomeLedBuffer[64+16*(3-(w.wp[pattern].cv_probs[edit_cv_ch][i1]>>6))+i1] = 11;
        }
      }
    }

  }

  // series
  else if(edit_mode == mSeries) {
    for(i1 = 0;i1<6;i1++) {
      for(i2=0;i2<SIZE;i2++) {
        // start/end bars, clear
        if((screll || key_alt) && (i1+screll_pos == w.series_start || i1+screll_pos == w.series_end)) monomeLedBuffer[32+i1*16+i2] = 11;
        else monomeLedBuffer[32+i1*16+i2] = 0;
      }

      // screll position helper
      // monomeLedBuffer[32+i1*16+((screll_pos+i1)/(64/SIZE))] = 4;
      
      // sidebar selection indicators
      if((screll || key_alt) && i1+screll_pos > w.series_start && i1+screll_pos < w.series_end) {
        monomeLedBuffer[32+i1*16] = 11;
        monomeLedBuffer[32+i1*16+LENGTH] = 11;
      }

      for(i2=0;i2<SIZE;i2++) {
        // show possible states
        if((w.series_list[i1+screll_pos] >> i2) & 1)
          monomeLedBuffer[32+(i1*16)+i2] = 11;
      }

    }

    // highlight playhead
    if(series_pos >= screll_pos && series_pos < screll_pos+6 && (pos & 1)) {
      monomeLedBuffer[32+(series_pos-screll_pos)*16+series_playing] = 0;
    }
  }

  monome_set_quadrant_flag(0);
  monome_set_quadrant_flag(1);

}






static u32 rnd() {
  x1 = x1 * c1 + a1;
  x2 = x2 * c2 + a2;
  return (x1>>16) | (x2>>16);
}


// pickle / unpickle
u8* op_ww_pickle(op_ww_t* mgrid, u8* dst) {
  dst = pickle_io(mgrid->focus, dst);
  dst = pickle_io(mgrid->param, dst);
  /// no state...???
  return dst;
}

const u8* op_ww_unpickle(op_ww_t* mgrid, const u8* src) {
  src = unpickle_io(src, (u32*)&(mgrid->focus));
  // FIXME should probably auto-detect grid size here::::
  src = unpickle_io(src, (u32*)&(mgrid->param));
  /*
    probably shouldn't call this here...
   if we assume that network monome device focus is null during unpickling,
   it will be ok.  that assumption should hold true, but if it doesn't, 
   or if we change something and forget to update this,
   the result is both and hard to track (dereferencing a garbage pointer.)
   we should just explicitly check for focused grid ops after scene recall, last one wins...
  */
  net_monome_set_focus( &(mgrid->monome), mgrid->focus > 0);
  return src;
}

// timer manipulation
static inline void op_ww_set_timer(op_ww_t* ww) {
  timers_set_custom(&(ww->timer), 50, &(ww->op_poll) );
}

static inline void op_ww_unset_timer(op_ww_t* ww) {
  timer_remove(&(ww->timer));
}
