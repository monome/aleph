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
static const char* op_kria_instring = "FOCUS\0  CLOCK\0  ";
static const char* op_kria_outstring ="TR0\0    TR1\0    CVA\0    CVB\0    ";
static const char* op_kria_opstring = "KRIA";

//-------------------------------------------------
//----- static function declaration

//---- input functions

//// network inputs:
static void op_kria_in_focus(op_kria_t* grid, const io_t val);
static void op_kria_in_clock(op_kria_t* grid, const io_t val);

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

static u32 rnd(void);

// input func pointer array
static op_in_fn op_kria_in_fn[2] = {
  (op_in_fn)&op_kria_in_focus,
  (op_in_fn)&op_kria_in_clock,
};

static u32 a1 = 0x19660d;
static u32 c1 = 0x3c6ef35f;
static u32 x1 = 1234567;  // seed
static u32 a2 = 0x19660d;
static u32 c2 = 0x3c6ef35f;
static u32 x2 = 7654321;  // seed




//////////////////////////////////////////////////

#define L2 12
#define L1 8
#define L0 4

#define FIRSTRUN_KEY 0x22


// for i in range(0,120):
// print '%.f, ' % (i * 4092.0 / 120.0)
const u16 ET[120] = {
  0, 34, 68, 102, 136, 170, 205, 239, 273, 307, 341, 375, 409, 443, 477, 511, 545, 580, 614, 648, 682, 716, 750, 784, 818, 852, 886,
  920, 955, 989, 1023, 1057, 1091, 1125, 1159, 1193, 1227, 1261, 1295, 1330, 1364, 1398, 1432, 1466, 1500, 1534, 1568, 1602, 1636,
  1670, 1705, 1739, 1773, 1807, 1841, 1875, 1909, 1943, 1977, 2011, 2046, 2080, 2114, 2148, 2182, 2216, 2250, 2284, 2318, 2352,
  2386, 2421, 2455, 2489, 2523, 2557, 2591, 2625, 2659, 2693, 2727, 2761, 2796, 2830, 2864, 2898, 2932, 2966, 3000, 3034, 3068,
  3102, 3136, 3171, 3205, 3239, 3273, 3307, 3341, 3375, 3409, 3443, 3477, 3511, 3546, 3580, 3614, 3648, 3682, 3716, 3750, 3784,
  3818, 3852, 3886, 3921, 3955, 3989, 4023, 4057
};

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
  u8 glyph[8][8];
  kria_set k[8];
  u8 scales[42][7];
} nvram_data_t;

kria_set k;

u8 preset_mode, preset_select, front_timer;
u8 glyph[8];

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
u16 cv0, cv1;

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


// FIXME - hummmm what do these do?
/* static void clockTimer_callback(void* o) { */
/*   if(clock_external == 0) { */
/*     // print_dbg("\r\ntimer."); */

/*     clock_phase++; */
/*     if(clock_phase>1) clock_phase=0; */
/*     (*clock_pulse)(clock_phase); */
/*   } */
/* } */

/* static void keyTimer_callback(void* o) { */
/*   static event_t e; */
/*   e.type = kEventKeyTimer; */
/*   e.data = 0; */
/*   event_post(&e); */
/* } */

/* static void adcTimer_callback(void* o) { */
/*   static event_t e; */
/*   e.type = kEventPollADC; */
/*   e.data = 0; */
/*   event_post(&e); */
/* } */
static void note0offTimer_callback(void* o) {
  op_kria_t *kria = (op_kria_t *) o;
  if(need0off) {
    need0off = 0;
    timer_reset_set(&note0offTimer, 10000);
    net_activate(kria, 0, 0);
  }
}

static void note1offTimer_callback(void* o) {
  op_kria_t *kria = (op_kria_t *) o;
  if(need1off) {
    need1off = 0;
    timer_reset_set(&note1offTimer, 10000);
    net_activate(kria, 1, 0);
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
  //  print_dbg("\r\n op_kria_init ");
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

  op->super.numInputs = 2;
  op->super.numOutputs = 4;

  op->super.in_val = op->in_val;
  op->super.out = op->outs;

  op->super.opString = op_kria_opstring;
  op->super.inString = op_kria_instring;
  op->super.outString = op_kria_outstring;

  op->in_val[0] = &(op->focus);
  op->monome.focus = &(op->focus);
  op->in_val[1] = &(op->clk);
  op->outs[0] = -1;
  op->outs[1] = -1;
  op->outs[2] = -1;
  op->outs[3] = -1;


  op->focus = 0;
  op->clk = 0;
  op->param = 0;


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

      // k.kp[c][i1].sync = 1;
      k.kp[c][i1].dur_mul = 4;

      for(i2=0;i2<NUM_PARAMS;i2++) {
	k.kp[c][i1].lstart[i2] = 0;
	k.kp[c][i1].lend[i2] = 5;
	k.kp[c][i1].llen[i2] = 6;
	k.kp[c][i1].lswap[i2] = 0;
	k.kp[c][i1].tmul[i2] = 1;
	// k.kp[c][i1].tdiv[i2] = 1;
      }
    }
  }

  for(i1=0;i1<7;i1++)
    k.pscale[i1] = i1;

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
    // if(op->focus>0) { net_monome_grid_clear(); }
    op->focus = 0;
  }

  u8 i1;
  // print_dbg("\r\n// monome connect /////////////////");
  keycount_pos = 0;
  key_count = 0;
  /* SIZE = monome_size_x(); */
  /* LENGTH = SIZE - 1; */
  /* // print_dbg("\r monome size: "); */
  /* // print_dbg_ulong(SIZE); */
  /* VARI = monome_is_vari(); */
  /* // print_dbg("\r monome vari: "); */
  /* // print_dbg_ulong(VARI); */

  /* if(VARI) re = &kria_refresh; */
  /* else re = &kria_refresh_mono; */

  /* for(i1=0;i1<16;i1++) */
  /*   if(w.wp[i1].loop_end > LENGTH) */
  /*     w.wp[i1].loop_end = LENGTH; */


  /* re = &refresh; */
  // monome_size_x();
  net_monome_set_focus( &(op->monome), op->focus > 0);
}


// t = track
// param = mode_idx (enum modes)
bool kria_next_step(uint8_t t, uint8_t param) {
  /* printf("entered kria_next_step\n"); */
  pos_mul[t][param]++; // pos_mul is current sub-clock position?

  // if(pos_mul[t][p] >= k.p[k.pattern].t[t].tmul[p]) {
  if(pos_mul[t][param] >= k.kp[t][p].tmul[param]) { // tmul is mode's clock divisor
    // if(pos[t][p] == k.p[k.pattern].t[t].lend[p])
    if(pos[t][param] == k.kp[t][p].lend[param]) // lend means "loop end", pos means current super-clock position
      // pos[t][p] = k.p[k.pattern].t[t].lstart[p]; 
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

static void op_kria_in_clock(op_kria_t* op, const io_t v) {
  if(v<0) op->clk = 0;
  else if(v>1) op->clk = 1;
  else op->clk = v;

  if(op->clk == 1) {
    /* gpio_set_gpio_pin(B10); */

    if(p_next != p) {
      p = p_next;
      phase_reset0();
      phase_reset1();
      return;
    }

    if(kria_next_step(0,tTrans)) {
      trans[0] = k.kp[0][p].trans[pos[0][tTrans]];
    }

    if(kria_next_step(0, tScale)) {
      if(sc[0] != k.kp[0][p].sc[pos[0][tScale]]) {
	sc[0] = k.kp[0][p].sc[pos[0][tScale]];
	calc_scale(0);
      }
    }

    if(kria_next_step(0, tDur)) {
      dur[0] = (k.kp[0][p].dur[pos[0][tDur]]+1) * (k.kp[0][p].dur_mul<<2);
    }
    
    if(kria_next_step(0, tOct)) {
      oct[0] = k.kp[0][p].oct[pos[0][tOct]];
    }

    if(kria_next_step(0, tNote)) {
      note[0] = k.kp[0][p].note[pos[0][tNote]];
    }

    if(kria_next_step(0, tAc)) {
      ac[0] = k.kp[0][p].ac[pos[0][tAc]];
    }

    if(kria_next_step(0,tTr)) {
      if(k.kp[0][p].tr[pos[0][tTr]]) {
	tr[0] = 1 + ac[0];
	cv0 = ET[cur_scale[0][note[0]] + (oct[0] * 12) + (trans[0] & 0xf) + ((trans[0] >> 4)*5)];
      }
      else {
	tr[0] = 0;
      }
    }

    if(p_next != p) {
      /* printf("triggered p_next != p\n"); */
      p = p_next;
      phase_reset0();
      phase_reset1();
      return;
    }

    if(kria_next_step(1,tTrans))
      trans[1] = k.kp[1][p].trans[pos[1][tTrans]];

    if(kria_next_step(1, tScale)) {
      if(sc[1] != k.kp[1][p].sc[pos[1][tScale]]) {
	sc[1] = k.kp[1][p].sc[pos[1][tScale]];
	calc_scale(1);
      }
    }

    if(kria_next_step(1, tDur))
      dur[1] = (k.kp[1][p].dur[pos[1][tDur]]+1) * (k.kp[1][p].dur_mul<<2);

    if(kria_next_step(1, tOct))
      oct[1] = k.kp[1][p].oct[pos[1][tOct]];

    if(kria_next_step(1, tNote))
      note[1] = k.kp[1][p].note[pos[1][tNote]];

    if(kria_next_step(1, tAc))
      ac[1] = k.kp[1][p].ac[pos[1][tAc]];

    if(kria_next_step(1,tTr)) {
      if(k.kp[1][p].tr[pos[1][tTr]]) {
	tr[1] = 1 + ac[1];
	cv1 = ET[cur_scale[1][note[1]] + (oct[1] * 12) + (trans[1] & 0xf) + ((trans[1] >> 4)*5)];
      }
      else {
	tr[1] = 0;
      }
    }

    monomeFrameDirty++;

  }
  else  {
  }
  if(tr[0]) {
    net_activate(op, 2, cv0);
    net_activate(op, 0, tr[0]);
    
    need0off = 1;
    timer_reset_set(&note0offTimer, dur[0]);

  }
  if(tr[1]) {
    net_activate(op, 3, cv1);
    net_activate(op, 1, tr[1]);
    need1off = 1;
    timer_reset_set(&note1offTimer, dur[1]);
  }
  // print_dbg("\r\n pos: ");
  // print_dbg_ulong(pos);
  tr[0] = 0;
  tr[1] = 0;

  /* gpio_clr_gpio_pin(B10); */

  // print_dbg("\r\nt ");
  // print_dbg_ulong(timer_ticks(&clockTimer));

  // not optimal - but for now simply request redraw on every clock
  dirty = 1;
}


// poll event handler
static void op_kria_poll_handler(void* op) {
  static u16 i1;
  /* printf("kria polled\n"); */
  if(front_timer) {
    if(front_timer == 1) {
      /* static event_t e; */
      /* e.type = kEventSaveFlash; */
      /* event_post(&e); */

      preset_mode = 0;
      front_timer--;
    }
    else front_timer--;
  }

  for(i1=0;i1<key_count;i1++) {
    if(key_times[held_keys[i1]])
      if(--key_times[held_keys[i1]]==0) {
	if(preset_mode == 0) {
	  /*
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

	  w.wp[x].tr_mode = w.wp[pattern].tr_mode;
	  w.wp[x].step_mode = w.wp[pattern].step_mode;

	  pattern = x;
	  next_pattern = x;

	  monomeFrameDirty++;

	  // print_dbg("\r\n saved pattern: ");
	  // print_dbg_ulong(x);
	  }
	  */
	}
	else if(preset_mode == 1) {
	  if(held_keys[i1] % 16 == 0) {
	    preset_select = held_keys[i1] / 16;
	    // flash_write();
	    /* static event_t e; */
	    /* e.type = kEventSaveFlash; */
	    /* event_post(&e); */
	    preset_mode = 0;
	  }
	}

	// print_dbg("\rlong press: ");
	// print_dbg_ulong(held_keys[i1]);
      }
  }
  op_kria_t *kria = (op_kria_t *) op;
  if(dirty) {
    kria_refresh(&kria->monome);
  }
  dirty = 0;
}


// process monome key input
static void op_kria_handler(op_monome_t* op_monome, u32 data) {
  u8 x, y, z, index, i1, found;
  monome_grid_key_parse_event_data(data, &x, &y, &z);
  /* printf("monome key pressed: %d, %d, %d\n", x, y, z); */
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
    key_times[index] = 10;		//// THRESHOLD key hold time
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
      // PRESET MODE FAST PRESS DETECT
      if(preset_mode == 1) {
	if(x == 0 && y != preset_select) {
	  preset_select = y;
	  /* for(i1=0;i1<8;i1++) */
	  /*   glyph[i1] = flashy.glyph[preset_select][i1]; */
	}
	else if(x==0 && y == preset_select) {
	  /* flash_read(); */

	  preset_mode = 0;
	}
      }
      // print_dbg("\r\nfast press: ");
      // print_dbg_ulong(index);
      // print_dbg(": ");
      // print_dbg_ulong(key_times[index]);
    }
  }

  // PRESET SCREEN
  if(preset_mode) {
    // glyph magic
    if(z && x>7) {
      glyph[y] ^= 1<<(x-8);
      /* printf("setting dirty"); */
      monomeFrameDirty++;
    }
  }
  // NOT PRESET
  else {
    // bottom row
    if(y == 7) {
      if(x == 15)
	key_alt = z;
      else if(x == 3) {
	if(z) mode = mTr;
      }
      else if(x == 4) {
	if(z) mode = mDur;
      }
      else if(x == 5) {
	if(z) mode = mNote;
      }
      else if(x == 6) {
	if(z) mode = mScale;
      }
      else if(x == 7) {
	if(z) mode = mTrans;
      }
      else if(x == 9) {
	if(z) {
	  mod_mode = modLoop;
	  loop_count = 0;
	}
	else
	  mod_mode = modNone;
      }
      else if(x == 10) {
	if(z)
	  mod_mode = modTime;
	else
	  mod_mode = modNone;
      }
      else if(x == 0) {
	if(z) {
	  ch = 0;
	  if(key_alt)
	    phase_reset0();
	}
      }
      else if(x == 1) {
	if(z) {
	  ch = 1;
	  if(key_alt)
	    phase_reset1();
	}
      }
      else if(x == 12) {
	if(z) mode = mScaleEdit;
      }
      else if(x == 13) {
	if(z) mode = mPattern;
      }
      /* printf("setting dirty\n"); */
      monomeFrameDirty++;
    }


    // toggle steps
    else if(mode == mTr) {
      if(mod_mode == modNone) {
	if(z) {
	  if(y==0)
	    k.kp[ch][p].tr[x] ^= 1;
	  else if(y==1)
	    k.kp[ch][p].ac[x] ^= 1;
	  else if(y>1)
	    k.kp[ch][p].oct[x] = 6-y;
      /* printf("setting dirty\n"); */
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
      /* printf("setting dirty\n"); */
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
	    // calctimes[ch][tTr] = (basetime * k.kp[ch][p].tmul[tTr]) / k.kp[ch][p].tdiv[tTr];
	  }
	  // else if(y == 1) {
	  //	k.kp[ch][p].tdiv[tTr] = x+1;
	  //	calctimes[ch][tTr] = (basetime * k.kp[ch][p].tmul[tTr]) / k.kp[ch][p].tdiv[tTr];
	  // }
	  else if(y == 2) {
	    k.kp[ch][p].tmul[tAc] = x+1;
	    // calctimes[ch][tAc] = (basetime * k.kp[ch][p].tmul[tAc]) / k.kp[ch][p].tdiv[tAc];
	  }
	  // else if(y == 3) {
	  //	k.kp[ch][p].tdiv[tAc] = x+1;
	  //	calctimes[ch][tAc] = (basetime * k.kp[ch][p].tmul[tAc]) / k.kp[ch][p].tdiv[tAc];
	  // }
	  else if(y == 4) {
	    k.kp[ch][p].tmul[tOct] = x+1;
	    // calctimes[ch][tOct] = (basetime * k.kp[ch][p].tmul[tOct]) / k.kp[ch][p].tdiv[tOct];
	  }
	  // else if(y == 5) {
	  //	k.kp[ch][p].tdiv[tOct] = x+1;
	  //	calctimes[ch][tOct] = (basetime * k.kp[ch][p].tmul[tOct]) / k.kp[ch][p].tdiv[tOct];
	  // }
	}
      }
    }
    else if(mode == mDur) {
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
	    // calctimes[ch][tDur] = (basetime * k.kp[ch][p].tmul[tDur]) / k.kp[ch][p].tdiv[tDur];
	  }
	  // else if(y == 1) {
	  //	k.kp[ch][p].tdiv[tDur] = x+1;
	  //	calctimes[ch][tDur] = (basetime * k.kp[ch][p].tmul[tDur]) / k.kp[ch][p].tdiv[tDur];
	  // }
	}

      /* printf("setting dirty\n"); */
	monomeFrameDirty++;
      }
      else if(mod_mode == modLoop) loop_count--;
    }
    else if(mode == mNote) {
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
	    // calctimes[ch][tNote] = (basetime * k.kp[ch][p].tmul[tNote]) / k.kp[ch][p].tdiv[tNote];
	  }
	  // else if(y == 1) {
	  //	k.kp[ch][p].tdiv[tNote] = x+1;
	  //	calctimes[ch][tNote] = (basetime * k.kp[ch][p].tmul[tNote]) / k.kp[ch][p].tdiv[tNote];
	  // }
	}

      /* printf("setting dirty\n"); */
	monomeFrameDirty++;
      }
      else if(mod_mode == modLoop) loop_count--;
    }
    else if(mode == mScale) {
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
    else if(mode == mTrans) {
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
	    // calctimes[ch][tTrans] = (basetime * k.kp[ch][p].tmul[tTrans]) / k.kp[ch][p].tdiv[tTrans];
	  }
	  // else if(y == 1) {
	  //	k.kp[ch][p].tdiv[tTrans] = x+1;
	  //	calctimes[ch][tTrans] = (basetime * k.kp[ch][p].tmul[tTrans]) / k.kp[ch][p].tdiv[tTrans];
	  // }
	}
      /* printf("setting dirty\n"); */
	monomeFrameDirty++;
      }
      else if(mod_mode == modLoop) loop_count--;
    }
    else if(mode == mScaleEdit) {
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

      /* printf("setting dirty\n"); */
	monomeFrameDirty++;
      }
      else if(mod_mode == modLoop) loop_count--;
    }
    else if(mode==mPattern) {
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
	      // k.kp[i1][p_next].tdiv[i2] = k.kp[i1][p].tdiv[i2];
	    }
	  }

	  p = p_next;
	}
	else
	  p_next = x;

      /* printf("setting dirty\n"); */
	monomeFrameDirty++;
      }
    }
  }
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
  static u8 i1;
  // cur_scale[c][0] = SCALE[sc[c]*7];
  cur_scale[c][0] = scales[k.pscale[sc[c]]][0];

  for(i1=1;i1<7;i1++)
    // cur_scale[c][i1] = cur_scale[c][i1-1] + SCALE[sc[c]*7 + i1];
    cur_scale[c][i1] = cur_scale[c][i1-1] + scales[k.pscale[sc[c]]][i1];
  // print_dbg("\r\nscale: ");
  // for(i1=0;i1<7;i1++) {
  //	print_dbg_ulong(cur_scale[c][i1]);
  //	print_dbg(" ");
  // }
}



static void kria_refresh(op_monome_t *op_monome){
  u8 i1,i2;
  /* printf("kria refresh\n"); */
  // bottom strip

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

  // modes

  if(mode == mTr) {
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
      // op_monome->opLedBuffer[pos[ch][tTr]+16] = L0;
      op_monome->opLedBuffer[k.kp[ch][p].tmul[tTr] - 1] = L2;
      // op_monome->opLedBuffer[k.kp[ch][p].tdiv[tTr] - 1 + 16] = L1;

      op_monome->opLedBuffer[pos[ch][tAc]+32] = L0;
      // op_monome->opLedBuffer[pos[ch][tAc]+48] = L0;
      op_monome->opLedBuffer[k.kp[ch][p].tmul[tAc] - 1 + 32] = L2;
      // op_monome->opLedBuffer[k.kp[ch][p].tdiv[tAc] - 1 + 48] = L1;

      op_monome->opLedBuffer[pos[ch][tOct]+64] = L0;
      // op_monome->opLedBuffer[pos[ch][tOct]+80] = L0;
      op_monome->opLedBuffer[k.kp[ch][p].tmul[tOct] - 1 + 64] = L2;
      // op_monome->opLedBuffer[k.kp[ch][p].tdiv[tOct] - 1 + 80] = L1;
    }
  }
  else if(mode == mDur) {
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
      // op_monome->opLedBuffer[pos[ch][tDur]+16] = L0;

      op_monome->opLedBuffer[k.kp[ch][p].tmul[tDur] - 1] = L2;
      // op_monome->opLedBuffer[k.kp[ch][p].tdiv[tDur] - 1 + 16] = L1;
    }
  }
  else if(mode == mNote) {
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
      // op_monome->opLedBuffer[pos[ch][tNote]+16] = L0;

      op_monome->opLedBuffer[k.kp[ch][p].tmul[tNote] - 1] = L2;
      // op_monome->opLedBuffer[k.kp[ch][p].tdiv[tNote] - 1 + 16] = L1;
    }
  }
  else if(mode == mScale) {
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
      // op_monome->opLedBuffer[pos[ch][tScale]+16] = L0;

      op_monome->opLedBuffer[k.kp[ch][p].tmul[tScale] - 1] = L2;
      // op_monome->opLedBuffer[k.kp[ch][p].tdiv[tScale] - 1 + 16] = L1;
    }
  }
  else if(mode == mTrans) {
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
      // op_monome->opLedBuffer[pos[ch][tTrans]+16] = L0;

      op_monome->opLedBuffer[k.kp[ch][p].tmul[tTrans] - 1] = L2;
      // op_monome->opLedBuffer[k.kp[ch][p].tdiv[tTrans] - 1 + 16] = L1;
    }
  }
  else if(mode==mScaleEdit) {
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
  else if(mode==mPattern) {
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

  monome_set_quadrant_flag(0);
  monome_set_quadrant_flag(1);
}



static u32 rnd() {
  x1 = x1 * c1 + a1;
  x2 = x2 * c2 + a2;
  return (x1>>16) | (x2>>16);
}


// pickle / unpickle
u8* op_kria_pickle(op_kria_t* mgrid, u8* dst) {
  dst = pickle_io(mgrid->focus, dst);
  dst = pickle_io(mgrid->param, dst);

  u32 *kria_state = (u32*)&k;
  while ((u8*)kria_state < ((u8*) &k) + sizeof(kria_set)) {
    dst = pickle_32(*kria_state, dst);
    kria_state +=1;
  }

  return dst;
}

const u8* op_kria_unpickle(op_kria_t* mgrid, const u8* src) {
  src = unpickle_io(src, (u32*)&(mgrid->focus));
  // FIXME should probably auto-detect grid size here::::
  src = unpickle_io(src, (u32*)&(mgrid->param));
  u32 *kria_state = (u32*)&k;
  while ((u8*)kria_state < ((u8*) &k) + sizeof(kria_set)) {
    src = unpickle_32(src, kria_state);
    kria_state +=1;
  }
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
static inline void op_kria_set_timer(op_kria_t* kria) {
  timers_set_custom(&(kria->timer), 50, &(kria->op_poll) );
}

static inline void op_kria_unset_timer(op_kria_t* kria) {
  timer_remove(&(kria->timer));
}
