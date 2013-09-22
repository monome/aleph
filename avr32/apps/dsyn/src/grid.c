#include "string.h"

#include "grid.h"
#include "monome.h"
#include "sequence.h"

// scroll position 
// (location of x_0 on grid, relative to start of sequence)
static s32 scroll = 0;
// sequence length - grid length
static s32 maxscroll = 4;

// count of 8x8 quads in the conneceted grid device
static u8 nquads = 0;
// width and height of connected grid device
static u8 gw = 0;
static u8 gh = 0;

static void grid_set_scroll (u8 v) {
  if(v > maxscroll) { v = maxscroll; }
  if(scroll < 0) { scroll = 0; }
  scroll = v;
  grid_show_seq();
  grid_show_pos();
  grid_show_loop();
  grid_show_len();
}

// scroll the position of the grid editor relative to the sequence
void grid_inc_scroll(s8 inc) {
  grid_set_scroll(scroll + inc);
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
    /// TODO: or something
    //    grid_show_next();
  } else if (y == 4) {
    // == start point
  }
  else if (y == 5) {
    // == end point ? length ? 
  } 
  else if (y==6) {
    // page
    switch(x) {
      
    }
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
  maxscroll = SEQ_NSTAGES - w;
}

// display sequence
void grid_show_seq(void) {
  const u8 * psrc;
  u8* pdst;
  //  u8 i;

  // somewhat hackish:
  // get pointer to led buf, increment by row offset
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

  /// at least this
  monome_set_quadrant_flag(0);
  if(nquads > 1) {
    // and maybe this
    monome_set_quadrant_flag(1);
  }
}

// display position ( / transport / cursor??)
void grid_show_pos(void) {
  // hack: MONOME_LED_ROW_BYTES * 7
  static const u32 posRowOffset = 112;
  u8 i;
  u8* p = monomeLedBuffer + posRowOffset;
  u32 pos = seq_get_pos() + scroll;
  for(i=0; i<gw; i++) {
    if(i == pos) {
      *p++ = 0xff;
    } else {
      *p++ = 0x0;
    }
  }
  /// at least this
  monome_set_quadrant_flag(0);
  if(nquads > 1) {
    // and maybe this
    monome_set_quadrant_flag(1);
  }
}

// display loop points
void grid_show_loop(void) {
  //  static const u32 loopRowOffset = 
  // TODO...
}

// display loop length
void grid_show_len(void) {
  //  static const u32 lenRowOffset = 
  // TODO
}

