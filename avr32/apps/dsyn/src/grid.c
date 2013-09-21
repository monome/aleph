#include "string.h"

#include "grid.h"
#include "monome.h"
#include "sequence.h"

// scroll position 
// (location of x_0 on grid, relative to start of sequence)
s32 scroll = 0;

// count of 8x8 quads in the conneceted grid device
static u8 nquads = 0;
// width and height of connected grid device
static u8 gw = 0;
static u8 gh = 0;

// scroll the position of the grid editor relative to the sequence
void grid_inc_scroll(s8 inc) {
  scroll += inc;
  if(scroll < 0) { scroll = 0; }
  if(scroll > SEQ_NSTAGES_1) { scroll = SEQ_NSTAGES_1; }
  grid_show_seq();
  grid_show_pos();
  grid_show_loop();
}

// handle a key press
void grid_handle_key_event(s32 data) {
  u8 x, y, val, pos;
  monome_grid_key_parse_event_data(data, &x, &y, &val);
  if(val == 0) { return; }

  pos = scroll + x;
  // == top 4 rows : sequence
  if(y < 4) { 
    seq_tog_stage(y, pos);
    grid_show_seq();
  } else if(y==7) {
    // ==  bottom row: jump to position
    seq_set_next(pos);
    grid_show_pos();
  } else if (y == 4) {
    // == start point
  }
  else if (y == 5) {
    // == end point ? length ? 
  } 
  else {
    // == ???
  }
}

// set size of grid
void grid_set_size(u8 w, u8 h) {
  gw = w;
  gh = h;
  if(w > 8) {
    if(h > 8) {
      nquads = 4;
    } else {
      nquads = 2;
    }
  } else {
    // no 8x16 devices... are there?
    nquads = 1;
  }
}

// display sequence
void grid_show_seq(void) {
  const u8 * psrc;
  u8* pdst;
  //  u8 i;

  // somewhat hackish
  psrc = seq_get_voice_data(0) + scroll;
  pdst = monomeLedBuffer;
  memcpy(pdst, psrc, gw);

  psrc = seq_get_voice_data(1) + scroll;
  pdst += MONOME_LED_ROW_BYTES;
  memcpy(pdst, psrc, gw);

  psrc = seq_get_voice_data(2) + scroll;
  pdst += MONOME_LED_ROW_BYTES;
  memcpy(pdst, psrc, gw);

  psrc = seq_get_voice_data(3) + scroll;
  pdst += MONOME_LED_ROW_BYTES;
  memcpy(pdst, psrc, gw);

  // dirty flags
  //for(i=0; i<nquads; i++) {
  //    monome_set_quadrant_flag(i);
  //  }
  /// at least this
  monome_set_quadrant_flag(0);
  if(nquads > 1) {
    // and maybe this
    monome_set_quadrant_flag(1);
  }
}

// display position ( / transport / cursor??)
void grid_show_pos(void) {
  //...  
}

// display loop points
void grid_show_loop(void) {
  
}

