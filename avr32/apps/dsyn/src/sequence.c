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
// length
u8 len = 8;
// end
u8 end = start + len;
// page
u8 page = 0;
// page step offset (page * width of grid)
u8 page_step_offset = 0;

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
    //  for(i=0; i<SEQ_NSTAGES; i++) {
      // binary... 

    if(stages[v][pos + page_step_offset] > 0) {
      // gate on
      ctl_voice_param( v, eParamGate, 1 );
      print_dbg("0");
    } else {     
      print_dbg("1");
    }
  }
  print_dbg(" ] ");
  
  if(next >= start+len) {
    next= start;
  }

  // rollover
  if(next >= gw) {
    next = 0;
  }

  pos = next;
  next++;
}

// set stage value 
void seq_set_stage(u8 vid, u8 sid, u8 val) {
  stages[vid][sid + page_step_offset] = val;
}

// toggle stage value 
 u8 seq_tog_stage(u8 vid, u8 sid) {
  stages[vid][sid + page_step_offset] ^= 0xff;
  return stages[vid][sid];
}


// set the next stage
void seq_set_next(u8 x) {
  // next stage;
  next = x;  
}

// get pointer to stage data at given voice
 const u8* seq_get_voice_data(u8 vid) {
  return (const u8*)(stages[vid+page_step_offset]);
}

// set loop start
void seq_set_start(u8 x) {
  start = x;  
}

// set loop length
void seq_set_len(u8 x) {
  len = x;  
}

// get current page
void u8 seq_set_page(u8 x) {
  page = x;
  page_step_offset = x << 3;  // FIXME this should be grid width
}

// get current position
const u8 seq_get_pos(void) {
  return pos;
}

// get start position
const u8 seq_get_start(void) {
  return start;
}

// get sequence length
const u8 seq_get_len(void) {
  return len;
}

// get current page
const u8 seq_get_page(void) {
  return page;
}


/* // get next position */
/* const u8 seq_get_next(void) { */
/*   return next; */
/* } */

