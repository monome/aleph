// asf
#include "print_funcs.h"
// bees
#include "app_timers.h"
#include "net_protected.h"
#include "net_poll.h"
#include "op_kria.h"
#include "timers.h"

//-------------------------------------------------
//----- static variables

//---- descriptor strings
static const char* op_kria_instring = "FOCUS\0  CLOCK\0  OCTAVE\0 TUNING\0 ";
static const char* op_kria_outstring ="TR0\0    NOTE0\0  TR1\0    NOTE1\0  ";
static const char* op_kria_opstring = "KRIA";

//-------------------------------------------------
//----- static function declaration

//---- input functions

//// network inputs:
static void op_kria_in_focus(op_kria_t* grid, const io_t val);
static void op_kria_in_clock(op_kria_t* grid, const io_t val);
static void op_kria_in_octave(op_kria_t* grid, const io_t val);
static void op_kria_in_tuning(op_kria_t* grid, const io_t val);

// pickles
static u8* op_kria_pickle(op_kria_t* enc, u8* dst);
static const u8* op_kria_unpickle(op_kria_t* enc, const u8* src);

// timer manipulation
static inline void op_kria_set_timer(op_kria_t* kria);
static inline void op_kria_unset_timer(op_kria_t* kria);


// polled-operator handler
static void op_kria_poll_handler(void* op);

/// monome event handler
static void op_kria_handler(op_monome_t* op_monome, u32 data);

// input func pointer array
static op_in_fn op_kria_in_fn[4] = {
  (op_in_fn)&op_kria_in_focus,
  (op_in_fn)&op_kria_in_clock,
  (op_in_fn)&op_kria_in_octave,
  (op_in_fn)&op_kria_in_tuning,
};

//////////////////////////////////////////////////

#define L2 12
#define L1 8
#define L0 4

const u8 SCALE[49] = {
  2, 2, 1, 2, 2, 2, 1,	// ionian
  2, 1, 2, 2, 2, 1, 2,	// dorian
  1, 2, 2, 2, 1, 2, 2,	// phyrgian
  2, 2, 2, 1, 2, 2, 1,	// lydian
  2, 2, 1, 2, 2, 1, 2,	// mixolydian
  2, 1, 2, 2, 1, 2, 2,	// aeolian
  1, 2, 2, 1, 2, 2, 2	// locrian
};

typedef enum {
  mTr, mDur, mNote, mScale, mTrans, mScaleEdit, mPattern
} modes;

typedef enum {
  tTr, tAc, tOct, tDur, tNote, tTrans, tScale
} time_params;

typedef enum {
  modNone, modLoop, modTime
} mod_modes;

#define NUM_PARAMS 7

typedef struct {
  u8 tr[16];
  u8 ac[16];
  u8 oct[16];
  u8 note[16];
  u8 dur[16];
  u8 sc[16];
  u8 trans[16];

  u8 dur_mul;

  u8 lstart[NUM_PARAMS];
  u8 lend[NUM_PARAMS];
  u8 llen[NUM_PARAMS];
  u8 lswap[NUM_PARAMS];
  time_params tmul[NUM_PARAMS];
  // time_params tdiv[NUM_PARAMS];
} kria_pattern;

// TO 96
typedef struct {
  kria_pattern kp[2][16];
  u8 pscale[7];
} kria_set;
typedef const struct {
  u8 fresh;
  modes mode;
  u8 preset_select;
  kria_set k[8];
  u8 scales[42][7];
} nvram_data_t;

kria_set k;

u8 preset_mode, preset_select, front_timer;

modes mode = mTr;
mod_modes mod_mode = modNone;
u8 p, p_next;
u8 ch;
u8 pos[2][NUM_PARAMS];
u8 trans_edit;
u8 pscale_edit;
u8 scales[42][7];

u8 key_alt, mod1, mod2;
u8 held_keys[32], key_count, key_times[256];
u8 keyfirst_pos, keysecond_pos;
s8 keycount_pos;
s8 loop_count;
u8 loop_edit;

u8 clock_phase;
u16 clock_time, clock_temp;

u32 basetime;
u32 calctimes[2][NUM_PARAMS];
u32 timeerrors[2][NUM_PARAMS];
u8 need0off;
u8 need1off;

u8 cur_scale[2][7];

u16 adc[4];

u8 tr[2];

u8 ac[2];
u8 oct[2];
u16 dur[2];
u8 note[2];
u8 sc[2];
u16 trans[2];
u16 cv[2];

u8 dirty = 0;

/* // NVRAM data structure located in the flash array. */
/* __attribute__((__section__(".flash_nvram"))) */
/* static nvram_data_t flashy; */

u8 pos_mul[2][NUM_PARAMS];

static void adjust_loop_start(u8 x, u8 m);
static void adjust_loop_end(u8 x, u8 m);

static void calc_scale(u8 c);

static void phase_reset0(void);
static void phase_reset1(void);



bool kria_next_step(uint8_t t, uint8_t p);


////////////////////////////////////////////////////////////////////////////////
// timers

static softTimer_t clockTimer = { .next = NULL, .prev = NULL };
static softTimer_t keyTimer = { .next = NULL, .prev = NULL };
static softTimer_t adcTimer = { .next = NULL, .prev = NULL };
static softTimer_t monomePollTimer = { .next = NULL, .prev = NULL };
static softTimer_t monomeRefreshTimer  = { .next = NULL, .prev = NULL };

static softTimer_t note0offTimer = { .next = NULL, .prev = NULL };
static softTimer_t note1offTimer = { .next = NULL, .prev = NULL };

static void note0offTimer_callback(void* o) {
  op_kria_t *kria = (op_kria_t *) o;
  if(need0off) {
    need0off = 0;
    net_activate(kria, 0, 0);
  }
}

static void note1offTimer_callback(void* o) {
  op_kria_t *kria = (op_kria_t *) o;
  if(need1off) {
    need1off = 0;
    net_activate(kria, 2, 0);
  }
}

////////////////////////////////////////////////////////////////////////////////
// prototypes

static void kria_refresh(op_monome_t *op_monome);
/* static void kria_refresh_mono(op_monome_t *op_monome); */

// static void handler_KeyTimer(s32 data);



static void phase_reset0() {
  for(u8 i1=0;i1<NUM_PARAMS;i1++)
    pos[0][i1] = k.kp[0][p].lend[i1];
}

static void phase_reset1() {
  for(u8 i1=0;i1<NUM_PARAMS;i1++)
    pos[1][i1] = k.kp[1][p].lend[1];
}



//-------------------------------------------------
//----- extern function definition
void op_kria_init(void* mem) {
  u8 i1,i2;
  op_kria_t* op = (op_kria_t*)mem;

  // superclass functions
  //--- op
  op->super.in_fn = op_kria_in_fn;
  op->super.pickle = (op_pickle_fn) (&op_kria_pickle);
  op->super.unpickle = (op_unpickle_fn) (&op_kria_unpickle);

  // polled operator superclass
  op->op_poll.handler = (poll_handler_t)(&op_kria_poll_handler);
  op->op_poll.op = op;

  //--- monome
  op->monome.handler = (monome_handler_t)&op_kria_handler;
  net_monome_init(&op->monome, op);

  // superclass state

  op->super.type = eOpKria;
  op->super.flags |= (1 << eOpFlagMonomeGrid);

  op->super.numInputs = 4;
  op->super.numOutputs = 4;

  op->super.in_val = op->in_val;
  op->super.out = op->outs;

  op->super.opString = op_kria_opstring;
  op->super.inString = op_kria_instring;
  op->super.outString = op_kria_outstring;

  op->in_val[0] = &(op->focus);
  op->monome.focus = &(op->focus);
  op->in_val[1] = &(op->clk);
  op->in_val[2] = &(op->octave);
  op->in_val[3] = &(op->tuning);
  op->outs[0] = -1;
  op->outs[1] = -1;
  op->outs[2] = -1;
  op->outs[3] = -1;

  op->focus = 0;
  op->clk = 0;
  op->octave = 12;
  op->tuning = 5;

  u8 c;
  // clear out some reasonable defaults
  for(c=0;c<2;c++) {
    for(i1=0;i1<16;i1++) {
      for(i2=0;i2<16;i2++) {
	k.kp[c][i1].tr[i2] = 0;
	k.kp[c][i1].ac[i2] = 0;
	k.kp[c][i1].oct[i2] = 0;
	k.kp[c][i1].dur[i2] = 0;
	k.kp[c][i1].note[i2] = 0;
	k.kp[c][i1].sc[i2] = 0;
	k.kp[c][i1].trans[i2] = 0;
      }

      k.kp[c][i1].dur_mul = 4;

      for(i2=0;i2<NUM_PARAMS;i2++) {
	k.kp[c][i1].lstart[i2] = 0;
	k.kp[c][i1].lend[i2] = 5;
	k.kp[c][i1].llen[i2] = 6;
	k.kp[c][i1].lswap[i2] = 0;
	k.kp[c][i1].tmul[i2] = 1;
      }
    }
  }

  for(i1=0;i1<7;i1++) {
    k.pscale[i1] = i1;
  }

  for(i1=0;i1<42;i1++) {
    scales[i1][0] = 0;
    for(i2=0;i2<6;i2++)
      scales[i1][i2+1] = 1;
  }



  net_monome_set_focus( &(op->monome), 1);

  // init monome drawing
  kria_refresh(&op->monome);

  op_kria_set_timer(op);
  timer_add(&note0offTimer,10000,&note0offTimer_callback, op);
  timer_add(&note1offTimer,10000,&note1offTimer_callback, op);
}

// de-init
void op_kria_deinit(void* op) {
  // release focus
  net_monome_set_focus(&(((op_kria_t*)op)->monome), 0);
  op_kria_unset_timer(op);
  timer_remove(&note0offTimer);
  timer_remove(&note1offTimer);
}

//-------------------------------------------------
//----- static function definition

//--- network input functions
static void op_kria_in_focus(op_kria_t* op, const io_t v) {
  if((v) > 0) {
    op->focus = OP_ONE;
  } else {
    op->focus = 0;
  }

  keycount_pos = 0;
  key_count = 0;
  net_monome_set_focus( &(op->monome), op->focus > 0);
}


// t = track
// param = mode_idx (enum modes)
bool kria_next_step(uint8_t t, uint8_t param) {
  pos_mul[t][param]++; // pos_mul is current sub-clock position?

  if(pos_mul[t][param] >= k.kp[t][p].tmul[param]) { // tmul is mode's clock divisor
    if(pos[t][param] == k.kp[t][p].lend[param]) // lend means "loop end", pos means current super-clock position
      pos[t][param] = k.kp[t][p].lstart[param]; // lstart means "loop start"
    else {
      pos[t][param]++;
      if(pos[t][param] > 15)
	pos[t][param] = 0;
    }
    pos_mul[t][param] = 0;
    return 1;
  }
  else
    return 0;
}

// t = track
static inline void op_kria_track_tick (op_kria_t *kria, u8 t) {
  if(p_next != p) {
    p = p_next;
    phase_reset0();
    phase_reset1();
    return;
  }

  if(kria_next_step(t,tTrans)) {
    trans[t] = k.kp[t][p].trans[pos[t][tTrans]];
  }

  if(kria_next_step(t, tScale)) {
    if(sc[t] != k.kp[t][p].sc[pos[t][tScale]]) {
      sc[t] = k.kp[t][p].sc[pos[t][tScale]];
      calc_scale(t);
    }
  }

  if(kria_next_step(t, tDur)) {
    dur[t] = (k.kp[t][p].dur[pos[t][tDur]]+1) * (k.kp[t][p].dur_mul<<2);
  }
    
  if(kria_next_step(t, tOct)) {
    oct[t] = k.kp[t][p].oct[pos[t][tOct]];
  }

  if(kria_next_step(t, tNote)) {
    note[t] = k.kp[t][p].note[pos[t][tNote]];
  }

  if(kria_next_step(t, tAc)) {
    ac[t] = k.kp[t][p].ac[pos[t][tAc]];
  }

  if(kria_next_step(t,tTr)) {
    if(k.kp[t][p].tr[pos[t][tTr]]) {
      tr[t] = 1 + ac[t];
      cv[t] = cur_scale[t][note[t]] + (oct[t] * kria->octave) + (trans[t] & 0xf) + ((trans[t] >> 4)*kria->tuning);
    }
    else {
      tr[t] = 0;
    }
  }
}

static void op_kria_in_octave(op_kria_t* op, const io_t v) {
  op->octave = v;
}

static void op_kria_in_tuning(op_kria_t* op, const io_t v) {
  op->tuning = v;
}

static void op_kria_in_clock(op_kria_t* op, const io_t v) {
  op->clk = 0;
  op_kria_track_tick(op, 0);
  op_kria_track_tick(op, 1);
  if(tr[0]) {
    net_activate(op, 1, cv[0]);
    net_activate(op, 0, tr[0]);
    need0off = 1;
    timer_reset_set(&note0offTimer, dur[0]);

  }
  if(tr[1]) {
    net_activate(op, 3, cv[1]);
    net_activate(op, 2, tr[1]);
    need1off = 1;
    timer_reset_set(&note1offTimer, dur[1]);
  }
  tr[0] = 0;
  tr[1] = 0;

  // FIXME not optimal - but for now total redraw on every clock
  dirty = 1;
}


// poll event handler
static void op_kria_poll_handler(void* op) {
  op_kria_t *kria = (op_kria_t *) op;
  if(dirty) {
    kria_refresh(&kria->monome);
  }
  dirty = 0;
}

static inline void handle_bottom_row_key(u8 x, u8 z) {
  if(z) {
    if(x == 3) {
      mode = mTr;
    }
    else if(x == 4) {
      mode = mDur;
    }
    else if(x == 5) {
      mode = mNote;
    }
    else if(x == 6) {
      mode = mScale;
    }
    else if(x == 7) {
      mode = mTrans;
    }
    else if(x == 9) {
      mod_mode = modLoop;
      loop_count = 0;
    }
    else if (x == 10) {
      mod_mode = modTime;
    }
    else if(x == 12) {
      mode = mScaleEdit;
    }
    else if(x == 13) {
      mode = mPattern;
    }
    if(x == 15) {
      key_alt = 1;
    }
    else if(x == 0) {
      ch = 0;
      if(key_alt)
	phase_reset0();
    }
    else if(x == 1) {
      ch = 1;
      if(key_alt)
	phase_reset1();
    }
  }
  else {
    if(x == 9) {
      mod_mode = modNone;
    }
    else if (x == 10) {
      mod_mode = modNone;
    }
    if(x == 15) {
      key_alt = 1;
    }
  }
}

static inline void mode_mTr_handle_key(u8 x, u8 y, u8 z) {
  if(mod_mode == modNone) {
    if(z) {
      if(y==0)
	k.kp[ch][p].tr[x] ^= 1;
      else if(y==1)
	k.kp[ch][p].ac[x] ^= 1;
      else if(y>1)
	k.kp[ch][p].oct[x] = 6-y;
      monomeFrameDirty++;
    }
  }
  else if(mod_mode == modLoop) {
    if(z) {
      loop_count++;

      if(key_alt) {
	if(y==0) {
	  adjust_loop_start(x, tTr);
	  adjust_loop_end(x, tTr);
	}
	else if(y==1) {
	  adjust_loop_start(x, tAc);
	  adjust_loop_end(x, tAc);
	}
	else if(y>1) {
	  adjust_loop_start(x, tOct);
	  adjust_loop_end(x, tOct);
	}
      }
      else if(loop_count == 1) {
	loop_edit = y;
	if(y==0)
	  adjust_loop_start(x, tTr);
	else if(y==1)
	  adjust_loop_start(x, tAc);
	else if(y>1)
	  adjust_loop_start(x, tOct);
      }
      else if(y == loop_edit) {
	if(y==0)
	  adjust_loop_end(x, tTr);
	else if(y==1)
	  adjust_loop_end(x, tAc);
	else if(y>1)
	  adjust_loop_end(x, tOct);
      }
      monomeFrameDirty++;
    }
    else {
      loop_count--;
    }
  }
  else if(mod_mode == modTime) {
    if(z) {
      if(y == 0) {
	k.kp[ch][p].tmul[tTr] = x+1;
      }
      else if(y == 2) {
	k.kp[ch][p].tmul[tAc] = x+1;
      }
      else if(y == 4) {
	k.kp[ch][p].tmul[tOct] = x+1;
      }
    }
  }
}

static inline void mode_mDur_handle_key (u8 x, u8 y, u8 z) {
  if(z) {
    if(mod_mode != modTime) {
      if(y==0)
	k.kp[ch][p].dur_mul = x+1;
      else {
	if(mod_mode == modNone)
	  k.kp[ch][p].dur[x] = y-1;
	else if(mod_mode == modLoop) {
	  loop_count++;
	  if(key_alt) {
	    adjust_loop_start(x, tDur);
	    adjust_loop_end(x, tDur);
	  }
	  else if(loop_count == 1)
	    adjust_loop_start(x, tDur);
	  else adjust_loop_end(x, tDur);
	}
      }
    }
    else {
      if(y == 0) {
	k.kp[ch][p].tmul[tDur] = x+1;
      }
    }
  }
  else if(mod_mode == modLoop) loop_count--;
}

static inline void mode_mNote_handle_key (u8 x, u8 y, u8 z) {
  if(z) {
    if(mod_mode != modTime) {
      if(mod_mode == modNone)
	k.kp[ch][p].note[x] = 6-y;
      else if(mod_mode == modLoop) {
	loop_count++;
	if(key_alt) {
	  adjust_loop_start(x, tNote);
	  adjust_loop_end(x, tNote);
	}
	else if(loop_count == 1)
	  adjust_loop_start(x, tNote);
	else adjust_loop_end(x, tNote);
      }
    }
    else {
      if(y == 0) {
	k.kp[ch][p].tmul[tNote] = x+1;
      }
    }
  }
  else if(mod_mode == modLoop) loop_count--;
}

static inline void mode_mScale_handle_key (u8 x, u8 y, u8 z) {
  if(z) {
    if(mod_mode != modTime) {
      if(mod_mode == modNone)
	k.kp[ch][p].sc[x] = y;
      else if(mod_mode == modLoop) {
	loop_count++;
	if(key_alt) {
	  adjust_loop_start(x, tScale);
	  adjust_loop_end(x, tScale);
	}
	else if(loop_count == 1)
	  adjust_loop_start(x, tScale);
	else adjust_loop_end(x, tScale);
      }
    }
    else {
      if(y == 0) {
	k.kp[ch][p].tmul[tScale] = x+1;
	// calctimes[ch][tScale] = (basetime * k.kp[ch][p].tmul[tScale]) / k.kp[ch][p].tdiv[tScale];
      }
      // else if(y == 1) {
      //	k.kp[ch][p].tdiv[tScale] = x+1;
      //	calctimes[ch][tScale] = (basetime * k.kp[ch][p].tmul[tScale]) / k.kp[ch][p].tdiv[tScale];
      // }
    }

    /* printf("setting dirty\n"); */
    monomeFrameDirty++;
  }
  else if(mod_mode == modLoop) loop_count--;
}
static inline void mode_mTrans_handle_key (u8 x, u8 y, u8 z) {
  if(z) {
    if(mod_mode != modTime) {
      if(y == 0) {
	if(mod_mode == modNone)
	  trans_edit = x;
	else if(mod_mode == modLoop) {
	  loop_count++;
	  if(key_alt) {
	    adjust_loop_start(x, tTrans);
	    adjust_loop_end(x, tTrans);
	  }
	  else if(loop_count == 1)
	    adjust_loop_start(x, tTrans);
	  else adjust_loop_end(x, tTrans);
	}
      }
      else
	k.kp[ch][p].trans[trans_edit] = x + ((6-y)<<4);
    }
    else {
      if(y == 0) {
	k.kp[ch][p].tmul[tTrans] = x+1;
      }
    }
  }
  else if(mod_mode == modLoop) loop_count--;
}

static inline void mode_mScaleEdit_handle_key (u8 x, u8 y, u8 z) {
  u8 i1;
  if(z) {
    if(x==0) {
      pscale_edit = y;
    }
    else if(y == 6 && x < 8) {
      for(i1=0;i1<6;i1++)
	scales[k.pscale[pscale_edit]][i1+1] = SCALE[(x-1)*7+i1];
      scales[k.pscale[pscale_edit]][0] = 0;

      if(sc[0] == pscale_edit)
	calc_scale(0);
      if(sc[1] == pscale_edit)
	calc_scale(1);
    }
    else if(x > 0 && x < 8) {
      if(key_alt) {
	for(i1=0;i1<7;i1++)
	  scales[k.pscale[x-1 + y*7]][i1] = scales[k.pscale[pscale_edit]][i1];
      }

      k.pscale[pscale_edit] = x-1 + y*7;

      if(sc[0] == pscale_edit)
	calc_scale(0);
      if(sc[1] == pscale_edit)
	calc_scale(1);

    }
    else if(x>7) {
      if(key_alt) {
	if(y!=0) {
	  s8 diff, change;
	  diff = (x-8) - scales[k.pscale[pscale_edit]][6-y];
	  change = scales[k.pscale[pscale_edit]][6-y+1] - diff;
	  if(change<0) change = 0;
	  if(change>7) change = 7;
	  scales[k.pscale[pscale_edit]][6-y+1] = change;
	}

	scales[k.pscale[pscale_edit]][6-y] = x-8;
      }
      else scales[k.pscale[pscale_edit]][6-y] = x-8;

      if(sc[0] == pscale_edit)
	calc_scale(0);
      if(sc[1] == pscale_edit)
	calc_scale(1);
    }
  }
  else if(mod_mode == modLoop) loop_count--;
}

static inline void mode_mPattern_handle_key (u8 x, u8 y, u8 z) {
  u8 i1;
  if(z && y==0) {
    if(key_alt) {
      p_next = x;

      for(i1=0;i1<2;i1++) {
	for(u8 i2=0;i2<16;i2++) {
	  k.kp[i1][p_next].tr[i2] = k.kp[i1][p].tr[i2];
	  k.kp[i1][p_next].ac[i2] = k.kp[i1][p].ac[i2];
	  k.kp[i1][p_next].oct[i2] = k.kp[i1][p].oct[i2];
	  k.kp[i1][p_next].dur[i2] = k.kp[i1][p].dur[i2];
	  k.kp[i1][p_next].note[i2] = k.kp[i1][p].note[i2];
	  k.kp[i1][p_next].trans[i2] = k.kp[i1][p].trans[i2];
	  k.kp[i1][p_next].sc[i2] = k.kp[i1][p].sc[i2];
	}

	k.kp[i1][p_next].dur_mul = k.kp[i1][p].dur_mul;

	for(u8 i2=0;i2<NUM_PARAMS;i2++) {
	  k.kp[i1][p_next].lstart[i2] = k.kp[i1][p].lstart[i2];
	  k.kp[i1][p_next].lend[i2] = k.kp[i1][p].lend[i2];
	  k.kp[i1][p_next].llen[i2] = k.kp[i1][p].llen[i2];
	  k.kp[i1][p_next].lswap[i2] = k.kp[i1][p].lswap[i2];
	  k.kp[i1][p_next].tmul[i2] = k.kp[i1][p].tmul[i2];
	}
      }
      p = p_next;
    }
    else {
      p_next = x;
    }
  }
}

// process monome key input
static void op_kria_handler(op_monome_t* op_monome, u32 data) {
  u8 x, y, z;
  monome_grid_key_parse_event_data(data, &x, &y, &z);

  // bottom row
  if(y == 7) {
    handle_bottom_row_key(x, z);
  }
  // toggle steps
  else if(mode == mTr) {
    mode_mTr_handle_key(x, y, z);
  }
  else if(mode == mDur) {
    mode_mDur_handle_key(x, y, z);
  }
  else if(mode == mNote) {
    mode_mNote_handle_key(x, y, z);
  }
  else if(mode == mScale) {
    mode_mScale_handle_key (x, y, z);
  }
  else if(mode == mTrans) {
    mode_mTrans_handle_key(x, y, z);
  }
  else if(mode == mScaleEdit) {
    mode_mScaleEdit_handle_key(x, y, z);
  }
  else if(mode==mPattern) {
    mode_mPattern_handle_key(x, y, z);
  }
  // FIXME not optimal - but for now simply total redraw on every press
  dirty = 1;
}

static void adjust_loop_start(u8 x, u8 m) {
  s8 temp;

  temp = pos[ch][m] - k.kp[ch][p].lstart[m] + x;
  if(temp < 0) temp += 16;
  else if(temp > 15) temp -= 16;
  pos[ch][m] = temp;

  k.kp[ch][p].lstart[m] = x;
  temp = x + k.kp[ch][p].llen[m]-1;
  if(temp > 15) {
    k.kp[ch][p].lend[m] = temp - 16;
    k.kp[ch][p].lswap[m] = 1;
  }
  else {
    k.kp[ch][p].lend[m] = temp;
    k.kp[ch][p].lswap[m] = 0;
  }
}

static void adjust_loop_end(u8 x, u8 m) {
  s8 temp;

  k.kp[ch][p].lend[m] = x;
  temp = k.kp[ch][p].lend[m] - k.kp[ch][p].lstart[m];
  if(temp < 0) {
    k.kp[ch][p].llen[m] = temp + 17;
    k.kp[ch][p].lswap[m] = 1;
  }
  else {
    k.kp[ch][p].llen[m] = temp+1;
    k.kp[ch][p].lswap[m] = 0;
  }

  temp = pos[ch][m];
  if(k.kp[ch][p].lswap[m]) {
    if(temp < k.kp[ch][p].lstart[m] && temp > k.kp[ch][p].lend[m])
      pos[ch][m] = k.kp[ch][p].lstart[m];
  }
  else {
    if(temp < k.kp[ch][p].lstart[m] || temp > k.kp[ch][p].lend[m])
      pos[ch][m] = k.kp[ch][p].lstart[m];
  }
}

static void calc_scale(u8 c) {
  u8 i1;
  cur_scale[c][0] = scales[k.pscale[sc[c]]][0];

  for(i1=1;i1<7;i1++) {
    cur_scale[c][i1] = cur_scale[c][i1-1] + scales[k.pscale[sc[c]]][i1];
  }
}

static inline void bottom_strip_redraw (op_monome_t *op_monome) {
  u8 i1;
  op_monome->opLedBuffer[112+(ch==0)] = L0;
  op_monome->opLedBuffer[112+ch] = L2;

  for(i1=0;i1<5;i1++)
    op_monome->opLedBuffer[115+i1] = L0;
  if(mode <= mTrans)
    op_monome->opLedBuffer[115 + mode] = L2;

  op_monome->opLedBuffer[121] = L0 + ((mod_mode == modLoop) << 2);
  op_monome->opLedBuffer[122] = L0 + ((mod_mode == modTime) << 2);

  op_monome->opLedBuffer[124] = L0 + ((mode == mScaleEdit) << 2);
  op_monome->opLedBuffer[125] = L0 + ((mode == mPattern) << 2);

  if(key_alt) op_monome->opLedBuffer[127] = L2;
  else op_monome->opLedBuffer[127] = 0;
}

static inline void mode_mTr_redraw (op_monome_t *op_monome) {
  u8 i1, i2;
  if(mod_mode != modTime) {
    for(i1=0;i1<112;i1++)
      op_monome->opLedBuffer[i1] = 0;

    if(mod_mode == modLoop) {
      if(k.kp[ch][p].lswap[tTr]) {
	for(i1=0;i1<k.kp[ch][p].llen[tTr];i1++)
	  op_monome->opLedBuffer[(i1+k.kp[ch][p].lstart[tTr])%16] = L0;
      }
      else {
	for(i1=k.kp[ch][p].lstart[tTr];i1<=k.kp[ch][p].lend[tTr];i1++)
	  op_monome->opLedBuffer[i1] = L0;
      }
      if(k.kp[ch][p].lswap[tAc]) {
	for(i1=0;i1<k.kp[ch][p].llen[tAc];i1++)
	  op_monome->opLedBuffer[16 + (i1 + k.kp[ch][p].lstart[tAc]) % 16] = L0;
      }
      else {
	for(i1=k.kp[ch][p].lstart[tAc];i1<=k.kp[ch][p].lend[tAc];i1++)
	  op_monome->opLedBuffer[16 + i1] = L0;
      }
    }

    for(i1=0;i1<16;i1++) {
      if(k.kp[ch][p].tr[i1])
	op_monome->opLedBuffer[i1] = L1;

      if(k.kp[ch][p].ac[i1])
	op_monome->opLedBuffer[16+i1] = L1;

      for(i2=0;i2<=k.kp[ch][p].oct[i1];i2++)
	op_monome->opLedBuffer[96-16*i2+i1] = L0;

      if(i1 == pos[ch][tTr])
	op_monome->opLedBuffer[i1] += 4;
      if(i1 == pos[ch][tAc])
	op_monome->opLedBuffer[16+i1] += 4;
      if(i1 == pos[ch][tOct])
	op_monome->opLedBuffer[96 - k.kp[ch][p].oct[i1]*16 + i1] += 4;
    }

    if(k.kp[ch][p].lswap[tTr]) {
      for(i1=0;i1<16;i1++)
	if(op_monome->opLedBuffer[i1])
	  if((i1 < k.kp[ch][p].lstart[tTr]) && (i1 > k.kp[ch][p].lend[tTr]))
	    op_monome->opLedBuffer[i1] -= 6;
    }
    else {
      for(i1=0;i1<16;i1++)
	if(op_monome->opLedBuffer[i1])
	  if((i1 < k.kp[ch][p].lstart[tTr]) || (i1 > k.kp[ch][p].lend[tTr]))
	    op_monome->opLedBuffer[i1] -= 6;
    }

    if(k.kp[ch][p].lswap[tAc]) {
      for(i1=0;i1<16;i1++)
	if(op_monome->opLedBuffer[16+i1])
	  if((i1 < k.kp[ch][p].lstart[tAc]) && (i1 > k.kp[ch][p].lend[tAc]))
	    op_monome->opLedBuffer[16+i1] -= 6;
    }
    else {
      for(i1=0;i1<16;i1++)
	if(op_monome->opLedBuffer[16+i1])
	  if((i1 < k.kp[ch][p].lstart[tAc]) || (i1 > k.kp[ch][p].lend[tAc]))
	    op_monome->opLedBuffer[16+i1] -= 6;
    }

    if(k.kp[ch][p].lswap[tOct]) {
      for(i1=0;i1<16;i1++)
	if((i1 < k.kp[ch][p].lstart[tOct]) && (i1 > k.kp[ch][p].lend[tOct]))
	  for(i2=0;i2<=k.kp[ch][p].oct[i1];i2++)
	    op_monome->opLedBuffer[96-16*i2+i1] -= 2;
    }
    else {
      for(i1=0;i1<16;i1++)
	if((i1 < k.kp[ch][p].lstart[tOct]) || (i1 > k.kp[ch][p].lend[tOct]))
	  for(i2=0;i2<=k.kp[ch][p].oct[i1];i2++)
	    op_monome->opLedBuffer[96-16*i2+i1] -= 2;
    }
  }
  else {
    for(i1=0;i1<112;i1++)
      op_monome->opLedBuffer[i1] = 0;

    op_monome->opLedBuffer[pos[ch][tTr]] = L0;
    op_monome->opLedBuffer[k.kp[ch][p].tmul[tTr] - 1] = L2;

    op_monome->opLedBuffer[pos[ch][tAc]+32] = L0;
    op_monome->opLedBuffer[k.kp[ch][p].tmul[tAc] - 1 + 32] = L2;

    op_monome->opLedBuffer[pos[ch][tOct]+64] = L0;
    op_monome->opLedBuffer[k.kp[ch][p].tmul[tOct] - 1 + 64] = L2;
  }
}

static inline void mode_mDur_redraw (op_monome_t *op_monome) {
  u8 i1, i2;
  if(mod_mode != modTime) {
    for(i1=0;i1<112;i1++)
      op_monome->opLedBuffer[i1] = 0;

    op_monome->opLedBuffer[k.kp[ch][p].dur_mul - 1] = L1;

    for(i1=0;i1<16;i1++) {
      for(i2=0;i2<=k.kp[ch][p].dur[i1];i2++)
	op_monome->opLedBuffer[16+16*i2+i1] = L0;

      if(i1 == pos[ch][tDur])
	op_monome->opLedBuffer[16+i1+16*k.kp[ch][p].dur[i1]] += 4;
    }

    if(k.kp[ch][p].lswap[tDur]) {
      for(i1=0;i1<16;i1++)
	if((i1 < k.kp[ch][p].lstart[tDur]) && (i1 > k.kp[ch][p].lend[tDur]))
	  for(i2=0;i2<=k.kp[ch][p].dur[i1];i2++)
	    op_monome->opLedBuffer[16+16*i2+i1] -= 2;
    }
    else {
      for(i1=0;i1<16;i1++)
	if((i1 < k.kp[ch][p].lstart[tDur]) || (i1 > k.kp[ch][p].lend[tDur]))
	  for(i2=0;i2<=k.kp[ch][p].dur[i1];i2++)
	    op_monome->opLedBuffer[16+16*i2+i1] -= 2;
    }
  }
  else {
    for(i1=0;i1<112;i1++)
      op_monome->opLedBuffer[i1] = 0;

    op_monome->opLedBuffer[pos[ch][tDur]] = L0;

    op_monome->opLedBuffer[k.kp[ch][p].tmul[tDur] - 1] = L2;
  }
}

static inline void mode_mNote_redraw (op_monome_t *op_monome) {
  u8 i1;
  if(mod_mode != modTime) {
    for(i1=0;i1<112;i1++)
      op_monome->opLedBuffer[i1] = 0;
    for(i1=0;i1<16;i1++)
      op_monome->opLedBuffer[i1+(6-k.kp[ch][p].note[i1])*16] = L1;
    op_monome->opLedBuffer[pos[ch][tNote] + (6-k.kp[ch][p].note[pos[ch][tNote]])*16] += 4;

    if(k.kp[ch][p].lswap[tNote]) {
      for(i1=0;i1<16;i1++)
	if((i1 < k.kp[ch][p].lstart[tNote]) && (i1 > k.kp[ch][p].lend[tNote]))
	  op_monome->opLedBuffer[i1+(6-k.kp[ch][p].note[i1])*16] -= 4;
    }
    else {
      for(i1=0;i1<16;i1++)
	if((i1 < k.kp[ch][p].lstart[tNote]) || (i1 > k.kp[ch][p].lend[tNote]))
	  op_monome->opLedBuffer[i1+(6-k.kp[ch][p].note[i1])*16] -= 4;
    }
  }
  else {
    for(i1=0;i1<112;i1++)
      op_monome->opLedBuffer[i1] = 0;

    op_monome->opLedBuffer[pos[ch][tNote]] = L0;

    op_monome->opLedBuffer[k.kp[ch][p].tmul[tNote] - 1] = L2;
  }
}

static inline void mode_mScale_redraw (op_monome_t *op_monome) {
  u8 i1, i2;
  if(mod_mode != modTime) {
    for(i1=0;i1<112;i1++)
      op_monome->opLedBuffer[i1] = 0;

    for(i1=0;i1<16;i1++) {
      for(i2=0;i2<=k.kp[ch][p].sc[i1];i2++)
	op_monome->opLedBuffer[16*i2+i1] = L0;

      op_monome->opLedBuffer[i1+16*k.kp[ch][p].sc[i1]] = L1;

      if(i1 == pos[ch][tScale])
	op_monome->opLedBuffer[i1+16*k.kp[ch][p].sc[i1]] += 4;
    }

    if(k.kp[ch][p].lswap[tScale]) {
      for(i1=0;i1<16;i1++)
	if((i1 < k.kp[ch][p].lstart[tScale]) && (i1 > k.kp[ch][p].lend[tScale]))
	  for(i2=0;i2<=k.kp[ch][p].sc[i1];i2++)
	    op_monome->opLedBuffer[16*i2+i1] -= 4;
    }
    else {
      for(i1=0;i1<16;i1++)
	if((i1 < k.kp[ch][p].lstart[tScale]) || (i1 > k.kp[ch][p].lend[tScale]))
	  for(i2=0;i2<=k.kp[ch][p].sc[i1];i2++)
	    op_monome->opLedBuffer[16*i2+i1] -= 4;;
    }
  }
  else {
    for(i1=0;i1<112;i1++)
      op_monome->opLedBuffer[i1] = 0;

    op_monome->opLedBuffer[pos[ch][tScale]] = L0;

    op_monome->opLedBuffer[k.kp[ch][p].tmul[tScale] - 1] = L2;
  }
}

static inline void mode_mTrans_redraw (op_monome_t *op_monome) {
  u8 i1;
  if(mod_mode != modTime) {
    for(i1=0;i1<112;i1++)
      op_monome->opLedBuffer[i1] = 0;

    if(mod_mode == modLoop) {
      if(k.kp[ch][p].lswap[tTrans]) {
	for(i1=0;i1<k.kp[ch][p].llen[tTrans];i1++)
	  op_monome->opLedBuffer[(i1+k.kp[ch][p].lstart[tTrans])%16] = L0;
      }
      else {
	for(i1=k.kp[ch][p].lstart[tTrans];i1<=k.kp[ch][p].lend[tTrans];i1++)
	  op_monome->opLedBuffer[i1] = L0;
      }
    }

    op_monome->opLedBuffer[trans_edit] = L1;
    op_monome->opLedBuffer[pos[ch][tTrans]] += 4;

    for(i1=0;i1<16;i1++) {
      op_monome->opLedBuffer[(k.kp[ch][p].trans[i1] & 0xf) + 16*(6-(k.kp[ch][p].trans[i1] >> 4))] = L0;
    }

    op_monome->opLedBuffer[(k.kp[ch][p].trans[pos[ch][tTrans]] & 0xf) + 16*(6-(k.kp[ch][p].trans[pos[ch][tTrans]] >> 4))] += 4;
    op_monome->opLedBuffer[(k.kp[ch][p].trans[trans_edit] & 0xf) + 16*(6-(k.kp[ch][p].trans[trans_edit] >> 4))] = L2;
  }
  else {
    for(i1=0;i1<112;i1++)
      op_monome->opLedBuffer[i1] = 0;

    op_monome->opLedBuffer[pos[ch][tTrans]] = L0;

    op_monome->opLedBuffer[k.kp[ch][p].tmul[tTrans] - 1] = L2;
  }
}

static inline void mode_mScaleEdit_redraw (op_monome_t *op_monome) {
  u8 i1;
  op_monome->opLedBuffer[112] = L1;
  op_monome->opLedBuffer[113] = L1;

  for(i1=0;i1<112;i1++)
    op_monome->opLedBuffer[i1] = 0;
  for(i1=0;i1<7;i1++) {
    op_monome->opLedBuffer[i1*16] = 4;
    op_monome->opLedBuffer[97+i1] = L0;
    op_monome->opLedBuffer[8+16*i1] = L0;
  }
  op_monome->opLedBuffer[k.kp[0][p].sc[pos[0][tScale]] * 16] = L1;
  op_monome->opLedBuffer[k.kp[1][p].sc[pos[1][tScale]] * 16] = L1;
  op_monome->opLedBuffer[pscale_edit * 16] = L2;

  op_monome->opLedBuffer[(k.pscale[pscale_edit] / 7) * 16 + 1 + (k.pscale[pscale_edit] % 7)] = L2;

  for(i1=0;i1<7;i1++)
    op_monome->opLedBuffer[scales[k.pscale[pscale_edit]][i1] + 8 + (6-i1)*16] = L1;

  if(sc[0] == pscale_edit && tr[0]) {
    i1 = k.kp[0][p].note[pos[0][tNote]];
    op_monome->opLedBuffer[scales[k.pscale[pscale_edit]][i1] + 8 + (6-i1)*16] = L2;
  }

  if(sc[1] == pscale_edit && tr[1]) {
    i1 = k.kp[1][p].note[pos[1][tNote]];
    op_monome->opLedBuffer[scales[k.pscale[pscale_edit]][i1] + 8 + (6-i1)*16] = L2;
  }
}

static inline void mode_mPattern_redraw (op_monome_t *op_monome) {
  u8 i1;
  op_monome->opLedBuffer[112] = L1;
  op_monome->opLedBuffer[113] = L1;

  for(i1=0;i1<112;i1++)
    op_monome->opLedBuffer[i1] = 0;
  for(i1=0;i1<16;i1++)
    op_monome->opLedBuffer[i1] = L0;
  if(p_next != p)
    op_monome->opLedBuffer[p_next] = L1;
  op_monome->opLedBuffer[p] = L2;
}

static void kria_refresh(op_monome_t *op_monome) {
  bottom_strip_redraw(op_monome);

  if(mode == mTr) {
    mode_mTr_redraw(op_monome);
  }
  else if(mode == mDur) {
    mode_mDur_redraw(op_monome);
  }
  else if(mode == mNote) {
    mode_mNote_redraw(op_monome);
  }
  else if(mode == mScale) {
    mode_mScale_redraw(op_monome);
  }
  else if(mode == mTrans) {
    mode_mTrans_redraw(op_monome);
  }
  else if(mode==mScaleEdit) {
    mode_mScaleEdit_redraw(op_monome);
  }
  else if(mode==mPattern) {
    mode_mPattern_redraw(op_monome);
  }

  monome_set_quadrant_flag(0);
  monome_set_quadrant_flag(1);
}

// pickle / unpickle
u8* op_kria_pickle(op_kria_t* mgrid, u8* dst) {
  dst = pickle_io(mgrid->focus, dst);
  dst = pickle_io(mgrid->octave, dst);
  dst = pickle_io(mgrid->tuning, dst);

  u32 *kria_state = (u32*)&k;
  while ((u8*)kria_state < ((u8*) &k) + sizeof(kria_set)) {
    dst = pickle_32(*kria_state, dst);
    kria_state +=1;
  }

  return dst;
}

const u8* op_kria_unpickle(op_kria_t* mgrid, const u8* src) {
  src = unpickle_io(src, (u32*)&(mgrid->focus));
  src = unpickle_io(src, (u32*)&(mgrid->octave));
  src = unpickle_io(src, (u32*)&(mgrid->tuning));

  u32 *kria_state = (u32*)&k;
  while ((u8*)kria_state < ((u8*) &k) + sizeof(kria_set)) {
    src = unpickle_32(src, kria_state);
    kria_state +=1;
  }
  return src;
}

// timer manipulation
static inline void op_kria_set_timer(op_kria_t* kria) {
  timers_set_custom(&(kria->timer), 50, &(kria->op_poll) );
}

static inline void op_kria_unset_timer(op_kria_t* kria) {
  timer_remove(&(kria->timer));
}
