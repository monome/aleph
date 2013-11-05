#include "string.h"

#include "grid.h"
#include "monome.h"
#include "sequence.h"

// row assigments
#define GRID_ROW_SEQ0	0
#define GRID_ROW_SEQ1	1
#define GRID_ROW_SEQ2	2
#define GRID_ROW_SEQ3	3

#define GRID_ROW_START	4
#define GRID_ROW_LEN	5

#define GRID_ROW_PAGE 	6
#define GRID_ROW_POS 	7

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
// width as a power of 2
//...
// static u8 gw_lshift 0;


/* adding the option of two loop-editing modes.
 0) default mode: loop length is [1, 8],
	len row edits as bargraph

1) alt mode: loop length is [1, num stages],
	len row sets endpoint,
*/

static u8 lenEditMode = 1;

//------------------------
//---- -static funcs

static inline void dirtquad(void) {
  /// at least this
  monome_set_quadrant_flag(0);
  if(nquads > 1) {
    // and maybe this
    monome_set_quadrant_flag(1);
  }
}

static void grid_set_scroll (s32 v) {
  if(v > maxscroll) { v = maxscroll; }
  if(v < 0) { v = 0; }
  scroll = v;
  grid_show_seq();
  grid_show_pos();
  grid_show_start();
  grid_show_len();
  grid_show_page();
}

// setting page as a multiple of scroll
static void grid_set_page(u8 n) {
  /// FIXME: magic hack 
  grid_set_scroll(n << 3);
}

//-----------------------
//------ external funcs

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
  /// FIXME: ignoring the #defines above
  if(y < 4) { 
    seq_tog_stage(y, pos);
    grid_show_seq();
  } else if(y==GRID_ROW_POS) {
    /// jump to position
    seq_set_next(pos);
  } else if (y == GRID_ROW_START) {
    // set loop start
    seq_set_start(pos);
    grid_show_start();
    grid_show_len();
  }
  else if (y == GRID_ROW_LEN) {
    if(lenEditMode) {
      // set length / endpoint
      seq_set_end(pos);
    } else {
      seq_set_len(x + 1);
    }
    grid_show_start();
    grid_show_len();
  } 
  else if (y==GRID_ROW_PAGE) {
    // set pagegg
    if(x < 8) {
      grid_set_page(x);
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
  dirtquad();
}

// display position ( / transport / cursor??)
void grid_show_pos(void) {
  static const u32 posRowOffset = GRID_ROW_POS * MONOME_LED_ROW_BYTES;
  const u32 ulim = gw + scroll;
  u8* p = monomeLedBuffer + posRowOffset;
  u32 pos = seq_get_pos();
  u8 i;
  for(i=scroll; i<ulim; i++) {
    if(i == pos) {
      *p++ = 0xff;
    } else {
      *p++ = 0x0;
    }
  }
  dirtquad();
}

// display loop points
void grid_show_start(void) {
  static const u32 startRowOffset = GRID_ROW_START * MONOME_LED_ROW_BYTES;
  u8* p = monomeLedBuffer + startRowOffset;
  const u32 ulim = gw + scroll;
  u32 i;
  const u8 s = seq_get_start();
  const u8 e = seq_get_end();
  for(i=scroll; i<ulim; i++ ) {
    /// color cells included in loop
      if( (i >= s) && (i <= e)) { 
      *p++ = 0xff;
    } else {
      *p++ = 0x00;
    }
  }
  dirtquad();
}

// display loop length / endpoint
void grid_show_len(void) {
  static const u32 lenRowOffset = GRID_ROW_LEN * MONOME_LED_ROW_BYTES;
  u8* p = monomeLedBuffer + lenRowOffset;
  const u32 ulim = gw + scroll;
  u32 i;
  //  const u8 s = seq_get_start();
  const u8 e = seq_get_end();
  const u8 l = seq_get_len();

  if(lenEditMode) { // alt: color endpoint
    for(i=scroll; i<ulim; i++ ) {
      if( (i == e)) { 
	*p++ = 0xff;
      } else {
	*p++ = 0x00;
      } 
    }   
  } else { // default: show length in [1, 8] as bargraph
    
    for(i=scroll; i<ulim; i++ ) {
      if( i < l) { 
	*p++ = 0xff;
	} else {
	*p++ = 0x00;
      }
    }
    dirtquad();
  }
}


// show page depending on scroll
void grid_show_page(void) {
  static const u32 pageRowOffset = GRID_ROW_PAGE * MONOME_LED_ROW_BYTES;
  /// FIXME: magic hack... 
  const u8 pg = scroll >> 3;
  const u8 ps = seq_get_pos() >> 3;
  u8* p = monomeLedBuffer + pageRowOffset;
  u8 i;
  for(i=0; i<gw; i++) {
    if(i == pg || i == ps) {
      *p++ = 0xff;
    } else {
      *p++ = 0;
    }
  }
  dirtquad();
}


// toggle edit mode
extern void grid_toggle_edit_mode(void) {
  if (lenEditMode) {
    lenEditMode = 0;
  } else {
    lenEditMode = 1;
  }
  grid_show_start();
  grid_show_len();
}
