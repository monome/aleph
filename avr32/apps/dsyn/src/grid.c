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
  u8 x, y, val;
  monome_grid_key_parse_event_data(data, &x, &y, &val);
  if(val == 0) { return; }

  // == top 4 rows : sequence
  if(y < 4) { 
    seq_tog_stage(y, x);
    grid_show_seq();
  } else if(y==7) {
    // ==  bottom row: jump to position
    seq_set_next(x);
  } else if (y == 5) {
    // == start point
    seq_set_start(x)
    grid_show_start();
  }
  else if (y == 6) {
    seq_set_len(x);
    grid_show_len();
  } 
  else if (y == 4) {
    seq_set_page(x);
    grid_show_page();
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
    // (bc: rotation of a 128 is a host service, not in the monome firmware)
    nquads = 1;
  }
  maxscroll = SEQ_NSTAGES - w;
}

// display sequence --- ROWS 0,1,2,3
void grid_show_seq(void) {
  const u8 * psrc;
  u8* pdst;
  //  u8 i;

  // somewhat hackish:
  // get pointer to led buf, increment by row offset
  psrc = seq_get_voice_data(0);
  pdst = monomeLedBuffer;
  memcpy(pdst, psrc, gw);

  psrc = seq_get_voice_data(1);
  pdst += MONOME_LED_ROW_BYTES;
  memcpy(pdst, psrc, gw);

  psrc = seq_get_voice_data(2);
  pdst += MONOME_LED_ROW_BYTES;
  memcpy(pdst, psrc, gw);

  psrc = seq_get_voice_data(3);
  pdst += MONOME_LED_ROW_BYTES;
  memcpy(pdst, psrc, gw);

  /// at least this
  monome_set_quadrant_flag(0);
  if(nquads > 1) {
    // and maybe this
    monome_set_quadrant_flag(1);
  }
}

// display position ( / transport / cursor??) in ROW 7
void grid_show_pos(void) {
  // hack: MONOME_LED_ROW_BYTES * 7
  static const u32 posRowOffset = 112;
  u8 i;
  u8* p = monomeLedBuffer + posRowOffset;
  u32 pos = seq_get_pos();
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

// display loop start -- ROW 5
void grid_show_loop(void) {
  static const u32 loopRowOffset = 80;
  u8 i;
  u8* p = monomeLedBuffer + posRowOffset;
  for(i=0; i<gw; i++) {
    if(start > end) {     // for rollover width of grid
      if(i <= end || i >= start) {
        *p++ = 0xff;
      } else {
        *p++ = 0x0;
      }
    } else {
      if(i <= end && i >= start) {
        *p++ = 0xff;
      } else {
        *p++ = 0x0;
      }
    }
}

// display loop length -- ROW 6: solid bar, start from left
void grid_show_len(void) {
  static const u32 lenRowOffset = 96;
  u8 i;
  u8* p = monomeLedBuffer + posRowOffset;
  for(i=0; i<gw; i++) {
    if(i <= len) {
      *p++ = 0xff;
    } else {
      *p++ = 0x0;
    }
}

// display data page -- ROW 5: single point
void grid_show_page(void) {
  static const u32 lenRowOffset = 64;
  u8 i;
  u8* p = monomeLedBuffer + posRowOffset;
  for(i=0; i<gw; i++) {
    if(i <= page) {
      *p++ = 0xff;
    } else {
      *p++ = 0x0;
    }
}

