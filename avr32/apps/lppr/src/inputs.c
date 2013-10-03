/* inputs.c
   lppr
   aleph-avr32

   some controller mappings via weird delta-lookup method.

   not using fract32 arithmetic on avr32.
   so instead of a typical dual-lookup scheme
   (table of values, tab of deltas),
   these "deltas" are an integer multiplier of the (input % tablesize)
   such that interpolated value = values[input/tabsize]
                 + (deltas[input/tabsize] * input%(inmax/tabsize - 1)
   
   division being accomplished with >>, 
   given that both input range and tablesize are constrained to 2**n.

   FIXME: move this somewhere easier for multiple projects to access
   FIXME: it would be more accurate, at some expense,
   to use proper fract32 interpolation.
   generic dsplib from the ASF would work, 
   and is also a more unified n.n fixedpoint framework for other types.
   
*/



#include <math.h>

#include "print_funcs.h"

#include "fix.h"
#include "memory.h"
#include "inputs.h"

// table size ( 2^10 words )
#define TABLE_SIZE	0x400
#define TABLE_MAX	0x3ff

// input -> idx right shift amount
#define TABLE_RSHIFT 	6
// input / idx ratio
#define TABLE_STRIDE 	64
// stride - 1
#define TABLE_MASK 	63

// freq range (in note numbers)
static const double noteMin = 19;  	// ~ 24 hz
static const double noteMax = 116; 	// ~ 6.64 khz

//static // fix representation
static fix16 noteMin_fix;
static fix16 noteMax_fix;
static fix16 noteSpan_fix;

//------------------------------
//---- types and variables

// value-delta table for very fast lookup.
// size and input range are arbitrary powers of 2
typedef struct _table {
  //  u32 size; 	// size of table
  //  u32 stride; 	// input / idx ratio
  //  u8 rshift; 	
  s32 data[TABLE_SIZE];		// data points
  s32 delta[TABLE_SIZE];	// "deltas"
} table;

table* tabAmp;	// input -> raw amplitude
table* tabDb;	// input -> decibels
table* tabHz;   // input -> hertz
table* tabFreq; // input -> corner frequency coefficient
table* tabPan;	// input -> pan law

//--------------------------------------
//----- static functions

// lookup 
static s32 table_look(table* tab, u32 in) {
  u32 idx = in >> TABLE_RSHIFT;
  s32 mul = in & TABLE_MASK;
  s32 val = tab->data[idx] + (mul * tab->delta[idx]);
  return (s32)val;
}

// fill deltas (values already filled)
static void table_calc_deltas(table* tab) {
  u32 i;
  s32 sdif;
  double f;
  //  print_dbg("\r\n calc_deltas : 0x");
  //  print_dbg_hex((u32)tab);
  for(i=0; i < (TABLE_MAX); i++) {
    sdif = tab->data[i+1] - tab->data[i];
    f = (double)(tab->data[i+1] - tab->data[i]) / (double)TABLE_MASK;
    tab->delta[i] = (s32)f;
    /* print_dbg("\r\n [0x"); */
    /* print_dbg_hex(i); */
    /* print_dbg("] : 0x"); */
    /* print_dbg_hex((u32)tab->data[i+1]); */
    /* print_dbg(" - 0x"); */
    /* print_dbg_hex((u32)tab->data[i]); */
    /* print_dbg(" = 0x"); */
    /* print_dbg_hex((u32)sdif); */
    /* print_dbg(" ; delta: 0x"); */
    /* print_dbg_hex( tab->delta[i] ); */
  }
  tab->delta[TABLE_MAX] = tab->delta[TABLE_MAX - 1];
}

static void fill_amp_db_tables(void) {
  u32 i;	// int index
  double f; 	// float index
  double finc = 1.0 / (double)(TABLE_SIZE - 1);
  double fx;	// variables
  double fy;
  //s32 itmp;
  // breakpoint for linear segment in amp/db table
  const double ampBreak = 0.1;
  const double dbBreak = -20.0;
  //-- tmp
  // 1 / (1 - ampBreak)
    const double recip_inv_ampBreak = 1.0 / 0.9;  
    const double log10 = log(10.0);
  // fill amp/db tables
  f = 0.0;
  for(i=0; i<TABLE_SIZE; i++) {
    /// linear ramp in [0, breakDb]
    if (i == 0) {
      // zero the bottom
      tabAmp->data[i] = 0;
      // fix16 lacks '-inf' ...
      tabDb->data[i] = fix16_min;
    } else if(f <= ampBreak) {
      tabAmp->data[i] = (s32)((double)(FR32_MAX) * f);
      tabDb->data[i] = fix16_from_float((float)(log(f) / log10 * 20.0));
    } else {
      //// audio taper in (breakDb, 1]
      // map db linearly to input
      fy = (1.0 - (f - ampBreak) * recip_inv_ampBreak) * dbBreak;
      // convert to amplitude
      fx = powf(10.0, fy * 0.05);
      // amp in fract32
      tabAmp->data[i] = (s32)((double)(FR32_MAX) * fx);
      // check underflow
      if(tabAmp->data[i] < 0) { tabAmp->data[i] = 0; }
      // clamp to 0db
      tabDb->data[i] = fix16_from_float((float)fy); 
      if(tabDb->data[i] > 0) { tabDb->data[i] = 0; }
    }
    /* // test: */
    /* print_fix16(strbuf, tabDb->data[i]); */
    /* print_dbg("\r\n idx: "); */
    /* print_dbg_ulong(i); */
    /* print_dbg(", amp = 0x"); */
    /* print_dbg_hex(tabAmp->data[i]); */
    /* print_dbg(", db (fix16) : "); */
    /* print_dbg(strbuf); */
    /* print_dbg(" ( "); */
    /* print_dbg_hex(tabDb->data[i]); */
    /* print_dbg(" )"); */
    f += finc;
  }
  // calculate deltas
  table_calc_deltas(tabAmp);
  table_calc_deltas(tabDb);
}



///// perform test lookups and print
static void table_test_fix(table* tab) {
  u32 i;
  s32 v;
  u32 inc;
  u32 eq;
  u32 dec; // count of decreasing 
  s32 last = 0x80000000;
  char strbuf[11] = "           ";
  strbuf[10] = 0;
  print_dbg("\r\n testing table lookup... ");
  for(i=0; i<=IN_MAX; i++) {
    v = table_look(tab, i);
    if (v == last) {
      eq++;
    } else if( v > last) {
      inc++;
      last = v;
    } else {
      dec++;
      last = v;
    }
    /* print_dbg("\r\n ( 0x"); */
    /* print_dbg_hex(i); */
    /* print_dbg(" ) : "); */
    /* print_fix16(strbuf, (fix16)v); */
    /* print_dbg(strbuf); */
  }
  print_dbg("\r\n tested ");
  print_dbg_ulong(i - 1);
  print_dbg(" lookups. inc: ");
  print_dbg_ulong(inc);
  print_dbg(" , dec: ");
  print_dbg_ulong(dec);  
  print_dbg(" , eq: ");
  print_dbg_ulong(eq);
  
}
////

static void fill_hz_freq_tables(void) {
  u32 i;	// int index
  double f; 	// float index
  double finc = 1.0 / (double)(TABLE_SIZE - 1);
  double fx;	// variables
  double fy;
  double fz;
  double fn;
  s32 itmp;
  // 2 ** (1/12)
  const double tempered = 1.0594630943593;
  char strbuf[11] = "           ";

  noteMin_fix = (u32)noteMin << 16;
  noteMax_fix = (u32)noteMax << 16;
  /// IDIOT
  noteSpan_fix = fix16_sub(noteMax_fix, noteMin_fix);

  // fill hz / coeff tables 
  f = 0.0;
  for(i=0; i<TABLE_SIZE; i++) {
    // note numbers (difference)
    fn = f * ((double)noteMax - (double)noteMin) - 69.0 + noteMin;
    itmp = (s32)fn;
    // octave multiplier
    fy = powf(2.0, (double)(itmp / 12));
    // center frequency
    fy *= 440.0;
    // tempered ratio
    fz = fy * powf(tempered, fmod(fn, 12.0));
    // store hz
    tabHz->data[i] = fix16_from_float(fz);     
    // svf coefficient
    // HACK : assume samplerate == 48000 ...
    fx = (double)((double)fz / (double)48000.0);
    // stability limit:
    if(fx > 0.25) { fx = 0.25; }
    fy = (double)sinf(M_PI * fx) * 2.0;
    // store as fract32
    tabFreq->data[i] = (s32)(fy * (double)(FR32_MAX));
    // test:
    /* print_fix16(strbuf, tabHz->data[i]); */
    /* print_dbg("\r\n idx: "); */
    /* print_dbg_ulong(i); */
    /* print_dbg(", note : "); */
    /* print_dbg_ulong((s32)fn + 69); */
    /* print_dbg(", hz (fix16) : "); */
    /* print_dbg(strbuf); */
    /* print_dbg(" coeff : 0x"); */
    /* print_dbg_hex(tabFreq->data[i]); */
    f += finc;
  }   
  /// hack the endpoints of freq table
  tabFreq->data[0] = tabFreq->data[1] + (tabFreq->data[1] - tabFreq->data[2]);
  tabFreq->data[TABLE_SIZE - 1] = tabFreq->data[TABLE_SIZE - 2]
    + (tabFreq->data[TABLE_SIZE - 2] - tabFreq->data[TABLE_SIZE - 3]);
  /// calculate deltas 
  table_calc_deltas(tabHz);
  table_calc_deltas(tabFreq);
  
  // test
  print_dbg("\r\n testing hz lookup: ");
  table_test_fix(tabHz);
}

//----------------------------------------
//---- extern functions

// initialize tables

// FIXME: abstract some of this table stuff
//  and put it somewhere else
// maybe in aleph/audio
extern void inputs_init(void) {
  // allocate memory
  tabAmp = (table*)alloc_mem(sizeof(table));
  tabDb = (table*)alloc_mem(sizeof(table));
  tabHz = (table*)alloc_mem(sizeof(table));
  tabFreq = (table*)alloc_mem(sizeof(table));

  fill_amp_db_tables();
  fill_hz_freq_tables();

  // fill pan tables
  // ... TODO
}

// get amplitude
extern fract32 input_amp(u32 in) {
  return table_look(tabAmp, in);
}

// get db
extern fix16 input_db(u32 in) {
  return table_look(tabDb, in);
}

// get note number from input
extern fix16 input_note (u32 in) {
  return fix16_mul(IN_FR16(in), noteSpan_fix);
}

// get hz from input
extern fix16 input_hz (u32 in) {
  return table_look(tabHz, in);
}

// get frequency coefficient from input
extern fract32 input_freq (u32 in) {
  return table_look(tabHz, in);
}
