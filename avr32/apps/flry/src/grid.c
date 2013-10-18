#include "print_funcs.h"


#include "string.h"

#include "grid.h"
#include "monome.h"
#include "ctl.h"
#include "life.h"


// count of 8x8 quads in the conneceted grid device
static u8 nquads = 0;
// width and height of connected grid device
// static u8 gw = 0;
// static u8 gh = 0;
// width as a power of 2
//...
// static u8 gw_lshift 0;


/* adding the option of two loop-editing modes.
 0) default mode: loop length is [1, 8],
	len row edits as bargraph

1) alt mode: loop length is [1, num stages],
	len row sets endpoint,
*/

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

//-----------------------
//------ external funcs

// handle a key press
void grid_handle_key_event(s32 data) {
  u8 x, y, val;
  // u8 *p;
  monome_grid_key_parse_event_data(data, &x, &y, &val);

  // p = monomeLedBuffer;

  if(val) life_change(x,y);


  // print_dbg("\r\nbits:");
  // print_dbg_ulong(x);
  // print_dbg(" ");
  // print_dbg_ulong(y);
  // print_dbg(" ");
  // print_dbg_ulong(val);

}
