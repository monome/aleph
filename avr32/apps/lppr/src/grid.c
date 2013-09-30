// std
#include "string.h"
// aleph-avr32
#include "monome.h"
// lppr
#include "ctl.h"
#include "grid.h"
#include "render.h"

// row assigments
// /define the row number of each parameter,
// and appropriate offset into monome led buffer

// set delaytime0 to multiple of tap time
// (moving read head only)
#define GRID_ROW_MUL_0		0
//#define GRID_ROW_MUL_0_OFFSET  0
// set delaytime0 to multiple of tap time
// (moving read and write heads! what will happen?)
#define GRID_ROW_POS_0		1
//#define GRID_ROW_POS_0_OFFSET GRID_ROW_POS_0 * MONOME_LED_ROW_BYTES;
// set loop time
#define GRID_LOOP_0		2
//#define GRID_ROW_POS_0_OFFSET GRID_ROW_LOOP_0 * MONOME_LED_ROW_BYTES;
// set pre-level
#define GRID_ROW_PRE_0 	3
//#define GRID_ROW_POS_0_OFFSET GRID_ROW_PRE_0 * MONOME_LED_ROW_BYTES;

// set delaytime0 to multiple of tap time
// (moving read head only)
#define GRID_ROW_MUL_1		4
//#define GRID_ROW_MUL_OFFSET GRID_ROW_MUL_0 * MONOME_LED_ROW_BYTES;
// set delaytime0 to multiple of tap time
// (moving read and write heads! what will happen?)
#define GRID_ROW_POS_1		5
//#define GRID_ROW_POS_1_OFFSET GRID_ROW_POS_1 * MONOME_LED_ROW_BYTES;
// set loop time
#define GRID_LOOP_1		6
//#define GRID_ROW_LOOP_1_OFFSET GRID_ROW_LOOP_1 * MONOME_LED_ROW_BYTES;
// set pre-level
#define GRID_ROW_PRE_1 	7
//#define GRID_ROW_PRE_1_OFFSET GRID_ROW_PRE_1 * MONOME_LED_ROW_BYTES;


// count of 8x8 quads in the conneceted grid device
static u8 nquads = 0;
// width and height of connected grid device
static u8 gw = 0;
static u8 gh = 0;


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

// show a single point on a row
static void grid_show_point(u8 row, u8 val);

// show a bar-graph value in a row
static void grid_show_bar(u8 row, u8 val);


//-----------------------
//------ external funcs


// handle a key press
void grid_handle_key_event(s32 data) {
  u8 x, y, val;
  monome_grid_key_parse_event_data(data, &x, &y, &val);
  // don't process lifts
  if(val == 0) { return; }

  switch(y) {
  case GRID_ROW_MUL_0 :
    render_touched_delaytime(0);
    ctl_set_delay_mul(0, x + 1);
    grid_show_point(y, x);
    break;
  case  GRID_ROW_POS_0 :
    ctl_set_delay_pos(0, x + 1);
    grid_show_point(y, x);
    break;
  case  GRID_LOOP_0 :
    ctl_set_loop(0, x + 1);
    grid_show_bar(y, x);
    break;
  case  GRID_ROW_PRE_0 :
    // TODO  FIXME
    grid_show_bar(y, x);
    break;
  case  GRID_ROW_MUL_1 :
    render_touched_delaytime(1);
    ctl_set_delay_mul(1, x + 1);
    grid_show_point(y, x);
    break;
  case  GRID_ROW_POS_1 :
    ctl_set_delay_pos(1, x + 1);
    grid_show_point(y, x);
    break;
  case  GRID_LOOP_1 : 	
    ctl_set_loop(1, x + 1);
    grid_show_bar(y, x);
    break;
  case GRID_ROW_PRE_1 :
    /// TODO / FIXME
    //    grid_show_bar(y, x);
    break;
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

// show a single point in a row
void grid_show_point(u8 row, u8 val) {
  u8* p = monomeLedBuffer + (row << MONOME_LED_ROW_LS);
  memset(p, 0x00, MONOME_LED_ROW_BYTES);
  p[val] = 0xff;
  // fixme, whatevr
  dirtquad();
}

// show a bar-graph value in a row
void grid_show_bar(u8 row, u8 val) {
  u8* p = monomeLedBuffer + (row << MONOME_LED_ROW_LS);
  u8 i;
  if(val > MONOME_LED_ROW_BYTES) {
    val = MONOME_LED_ROW_BYTES;
  }
  for(i=0; i<val; i++) {
    *p++ = 0xff;
  }
  for(i=val; i<MONOME_LED_ROW_BYTES; i++) {
    *p++ = 0x00;
  }
  dirtquad();
}
