//asf
#include "print_funcs.h"

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

// start
u8 start = 0;
// end
u8 end = SEQ_NSTAGES_1;

// init
 void seq_init(void) {
}

// advance to the next stage for all voices
 void seq_advance(void) {
   //  u32 i;
  u8 v;
  print_dbg(" [ ");
  print_dbg_ulong(pos);
  grid_show_pos();
  print_dbg(" : ");
  for(v=0; v<DSYN_NVOICES; v++) {
    if(stages[v][pos] > 0) {
      // gate on
      ctl_voice_param( v, eParamGate, 1 );
      print_dbg("1");
    } else {     
      print_dbg("0");
    }
  }
  print_dbg(" ] ");
  pos = next;
  next++;
  if(pos >  SEQ_NSTAGES_1) {
    pos = 0;
  }
  if(next >  SEQ_NSTAGES_1) {
    next= 0;
  }
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

/* // get next position */
/* const u8 seq_get_next(void) { */
/*   return next; */
/* } */

