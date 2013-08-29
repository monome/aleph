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
static void dummyHandler(void* op, u32 edata) { return; }

// use led state buffer and dirty flags
static void monome_grid_key_loopback(void* op, u32 edata) {
  u8 x, y, z;
  /// FIXME: this stuff should really be abstracted
  monome_grid_key_parse_event_data(edata, &x, &y, &z);
  monomeLedBuffer[x | (y << 4)] = z;
  monome_calc_quadrant_flag(x, y);
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
extern void net_monome_set_focus(op_monome_t* op_monome, u8 focus) {
  print_dbg("\r\n setting monome device focu, op pointer: 0x");
  print_dbg_hex((u32)op_monome);
  print_dbg(" , value: ");
  print_dbg_ulong(focus);

  if(focus > 0) {
    if(monomeOpFocus != NULL ){
      /// stealing focus, inform the previous holder
      monomeOpFocus->focus = 0;
    }
    monome_grid_key_handler = op_monome->handler;
    monomeOpFocus = op_monome;
    op_monome->focus = 1;
  } else {
    // release focus if we had it, otherwise do nothing
    if( monomeOpFocus == op_monome) {
      monome_grid_key_handler = (monome_handler_t)&dummyHandler;
      monomeOpFocus = NULL;
      op_monome->focus = 0;
    }
  }
}

// set operator attributes from connected grid device
extern void net_monome_set_attributes() {
  //... TODO
}
