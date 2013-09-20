#include "ctl.h"
#include "sequence.h"

// stage data
static u8 stages[DSYN_NVOICES][SEQ_NSTAGES];
// index
static u32 idx = 0;

// init
extern void seq_init(void);

// advance to the next stage for all voices
extern void seq_next(void) {
  u8 v;
  for(v=0; v<DSYN_NVOICES; v++) {
    // binary... 
    if(stages[v][i] > 0) {
      // gate on
    }
  }
  
  idx++;
  if(idx == SEQ_NSTAGES) {
    idx = 0;
  }
}

// set stage value 
extern void seq_set_stage(u8 vid, u8 sid);
