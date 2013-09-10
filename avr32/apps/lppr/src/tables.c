#include <math.h>

#include "print_funcs.h"

#include "fix.h"
#include "memory.h"
#include "tables.h"


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

//--------------------------------------
//----- static functions

// lookup 
static s32 table_look(table* tab, u32 in) {
  u32 idx = in >> TABLE_RSHIFT;
  s32 mul = in & TABLE_MASK;
  s32 val = tab->data[idx] + (mul * tab->delta[idx]);
  return (s32)val;
}


//----------------------------------------
//---- extern functions

// initialize tables
extern void tables_init(void) {
  u32 i;
  double f;
  double finc = 1.0 / (double)(TABLE_SIZE - 1);
  double fdb, famp;
  // breakpoint for linear segment in amp/db table
  const double ampBreak = 0.1;
  const double dbBreak = -20.0;
  const double log10 = log(10.0);

  ////// test
  char strbuf[32];
  /////

  print_dbg("\r\n TABLE INIT \r\n");
  print_dbg("\r\n TABLE SIZE: \r\n");
  print_dbg_ulong(TABLE_SIZE);

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
      tabAmp->data[i] = 0;
      tabDb->data[i] = fix16_min;
    } else if(f <= ampBreak) {
      tabAmp->data[i] = (s32)((double)(FR32_MAX) * f);
      tabDb->data[i] = fix16_from_float((float)(log(f) / log10 * 20.0));
    } else {
      //// audio taper in (breakDb, 1]
      //      fdb = ((f - 0.01) / 0.99) * dbBreak + dbBreak;
      fdb = (f - ampBreak) / (1.0 - ampBreak) * dbBreak + dbBreak;
      famp = powf(10.0, fdb * 0.05);
      // amp in fract32
      tabAmp->data[i] = (s32)((double)(FR32_MAX) * famp);
      tabDb->data[i] = fix16_from_float((float)fdb); 
    }

    // test:

    print_fix16(strbuf, tabDb->data[i]);
    print_dbg("\r\n idx: ");
    print_dbg_ulong(i);
    print_dbg(", amp: 0x");
    print_dbg_hex(tabAmp->data[i]);
    print_dbg(", db (fix16) : ");
    print_dbg(strbuf);
    print_dbg("\r\n ");

    f += finc;
  }
  // fill hz / note / coeff tables
  

  /// calculate deltas
}

// get amplitude
extern fract32 lookup_amp(u32 in) {
}

// get db
extern fix16 lookup_db(u32 in) {
}
