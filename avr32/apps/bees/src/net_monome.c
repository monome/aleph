/* net_monome.c
   bees
   aleph

   monome driver <-> operator glue layer

   keeps pointers to handler functions,
   manages operator focus
 */

#include "print_funcs.h"
#include "net_monome.h"


// dummy handler
static void dummyHandler(u32 edata) { return; }

// loopback handler for test or default
/* static void monome_grid_key_loopback(u32 ev) { */
/*   u8 x, y, z; */
/*   monome_grid_key_parse_event_data(ev, &x, &y, &z); */
/*   (*monome_grid_led)(x, y, z); */
/* } */

// use led state buffer and dirty flags
static void monome_grid_key_loopback(u32 edata) {
  u8 x, y, z;
  monome_grid_key_parse_event_data(edata, &x, &y, &z);
  monomeLedBuffer[x | (y << 4)] = z;
  monome_calc_quadrant_flag(x, y);
  /// test: all frames
  //  monomeFrameDirty = 0xf;
}


//---------------------------------
// extern variables, initialized here.

monome_handler_t monome_grid_key_handler = &monome_grid_key_loopback;
monome_handler_t monome_ring_enc_handler = &dummyHandler;
op_monome_t* monomeOpFocus = NULL;

/// TODO: tilt and key press should be decoupled from grid/ring??? yeah probly. 
/// specialized operators are not really appropriate,
//// have system create additional instances of adc/sw ?? hm
//monome_handler_t monome_grid_tilt_handler = NULL;
//monome_handler_t monome_ring_key_handler = NULL;

//--------------------------
//---- extern functions
// init
extern void net_monome_init(void) {
  //...
}

// set focus
extern void net_monome_set_focus(op_monome_t* op, u8 focus) {
  if(focus > 0) {
    monome_grid_key_handler = op->handler;
    monomeOpFocus = op;
    //// FIXME: focus input wont work quite right i dont think
    op->focus = 1;
  } else {
    monome_grid_key_handler = &dummyHandler;
    monomeOpFocus = NULL;
    op->focus = 0;
  }
}

// set operator attributes from connected grid device
extern void net_monome_set_attributes() {
  //... TODO
}
