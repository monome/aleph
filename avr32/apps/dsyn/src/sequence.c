#include "ctl.h"
#include "grid.h"
#include "sequence.h"

// stage data
static u8 stages[DSYN_NVOICES][SEQ_NSTAGES];
// FIXME: could allow voices to advance independently...
/// for now there is only one position counter.
// position
u8 pos = 0;
// next position
u8 next = 1;

// init
 void seq_init(void) {
}

// advance to the next stage for all voices
 void seq_advance(void) {
  u32 i;
  u8 v;
  for(v=0; v<DSYN_NVOICES; v++) {
    for(i=0; i<SEQ_NSTAGES; i++) {
      // binary... 
      if(stages[v][i] > 0) {
	// gate on
	ctl_voice_param( i, eParamGate, 1 );
      }
    }
  }
  pos = next;
  next++;
  if(pos >  SEQ_NSTAGES_1) {
    pos = 0;
  }
  if(next >  SEQ_NSTAGES_1) {
    next= 0;
  }
  grid_show_seq();
  grid_show_pos();
}

// set stage value 
 void seq_set_stage(u8 vid, u8 sid, u8 val) {
  stages[vid][sid] = val;
}

// toggle stage value 
 u8 seq_tog_stage(u8 vid, u8 sid) {
  stages[vid][sid] ^= 0xff;
  return stages[vid][sid];
}


// set the next stage
 void seq_set_next(u8 x) {
  // next stage;
  next = x;  
}

// get pointer to stage data at given voice
 const u8* seq_get_voice_data(u8 vid) {
  return (const u8*)(stages[vid]);
}



// get current position
const u8 seq_get_pos(void) {
  return pos;
}

// get next position
const u8 seq_get_next(void) {
  return next;
}

