#include <math.h>

#include "print_funcs.h"

#include "fix.h"
#include "memory.h"
#include "inputs.h"

/// make all control tables the same size...
// maximum input value:  24 bits
#define IN_MAX		0xffffff
// table size ( 12 bits )
#define TABLE_SIZE	0x1000
#define TABLE_SIZE_1    0xfff

// input -> idx right shift amount
#define TABLE_RSHIFT 	12
// input / idx ratio
#define TABLE_STRIDE 	0x1000
// stride - 1
#define TABLE_MASK 	0xfff

//------------------------------
//---- types and variables

// value-delta table for very fast lookup.
// size and input range are arbitrary powers of 2
typedef struct _table {
  //  u32 size; 	// size of table
  //  u32 stride; 	// input / idx ratio
  //  u8 rshift; 	
  s32 data[TABLE_SIZE];
  s32 delta[TABLE_SIZE];
} table;

table* tabAmp;
table* tabDb;
table* tabHz;
table* tabCoeff;

table* tabPan;

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
  double f;
  for(i=0; i < (TABLE_SIZE - 1); i++) {
    f = (double)(tab->data[i+1] - tab->data[i]) / (double)TABLE_MASK;
    tab->delta[i] = (s32)f;
    print_dbg("\r\n delta: 0x");
    print_dbg_hex( tab->delta[i] );
  }
}

//----------------------------------------
//---- extern functions

// initialize tables

// FIXME: abstract some of this table stuff
//  and put it somewhere else
// maybe in aleph/audio
extern void inputs_init(void) {
  u32 i;	// int index
  double f; 	// float index
  double finc = 1.0 / (double)(TABLE_SIZE - 1);
  double fx;	// variables
  double fy;
  double fz;
  double fn;
  s32 itmp;
  // breakpoint for linear segment in amp/db table
  const double ampBreak = 0.1;
  const double dbBreak = -20.0;
  //-- tmp
  // 1 / (1 - ampBreak)
  const double recip_inv_ampBreak = 1.0 / 0.9;  
  const double log10 = log(10.0);
  // 2 ** (1/12)
  const double tempered = 1.0594630943593;
  // freq range (in note numbers)
  const double noteMin = 0;
  const double noteMax = 131;

  ////// test
  char strbuf[12];
  strbuf[11] = 0;
   /////

   // allocate memory
   tabAmp = (table*)alloc_mem(sizeof(table));
   tabDb = (table*)alloc_mem(sizeof(table));
   tabHz = (table*)alloc_mem(sizeof(table));
   tabCoeff = (table*)alloc_mem(sizeof(table));

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
   // fill hz / coeff tables
   
   f = 0.0;
   for(i=0; i<TABLE_SIZE; i++) {

     // note numbers (difference)s
     fn = f * ((double)noteMax - (double)noteMin) - 69.0;
     itmp = (s32)fn;
     //     itmp = (s32)fn - 69;
     // octave multiplier
     fy = powf(2.0, (double)(itmp / 12));
     //fy = powf(2.0, (double)(fn / 12.0));

     fy *= 440.0;
     //     fz = fy * powf(tempered, (float)(itmp % 12) );
     fz = fy * powf(tempered, fmod(fn, 12.0)); //(float)(itmp % 12) );
     tabHz->data[i] = fix16_from_float(fz);     
     // svf coefficient
     // HACK : assume samplerate == 48000 ...
     fx = fz / 48000.0;

     //     print_dbg("\r\n normalized freq: 0x");
     //     print_dbg_hex((s32)(fx * FR32_MAX));

     // stability limit:
     if(fx < 0.25) { fx = 0.25; } 
     fy = sinf(M_PI * fx) * 2.f;
     tabCoeff->data[i] = (s32)((double)(FR32_MAX) * fy);
     
     // test:
     print_fix16(strbuf, tabHz->data[i]);

     print_dbg("\r\n idx: ");
     print_dbg_ulong(i);
     print_dbg(", note : ");
     print_dbg_ulong((s32)fn + 69);
     print_dbg(", hz (fix16) : ");
     print_dbg(strbuf);
     print_dbg(" coeff : 0x");
     print_dbg_hex(tabDb->data[i]);

     f += finc;
   }   
  
   /// hack the endpoints
   tabCoeff->data[0] = tabCoeff->data[1] + (tabCoeff->data[1] - tabCoeff->data[2]);
   tabCoeff->data[TABLE_SIZE - 1] = tabCoeff->data[TABLE_SIZE - 2]
     + (tabCoeff->data[TABLE_SIZE - 2] - tabCoeff->data[TABLE_SIZE - 3]);

  // fill pan table

  /// calculate deltas for all tables
   table_calc_deltas(tabCoeff);
}

// get amplitude
extern fract32 input_amp(u32 in) {
}

// get db
extern fix16 input_db(u32 in) {
}

// get note number from input
extern fix16 input_note (u32 in) {
  
}
