#include "print_funcs.h"


#include "string.h"

#include "grid.h"
#include "monome.h"
#include "ctl.h"
#include "life.h"


// count of 8x8 quads in the conneceted grid device
static u8 nquads = 0;

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
  monome_grid_key_parse_event_data(data, &x, &y, &val);

  if(val) life_change(x,y);

}
