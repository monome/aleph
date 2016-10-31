/* net_monome.c
   bees
   aleph

   monome d\river <-> operator glue layer

   keeps pointers to handler functions,
   manages operator focus
*/

/// assf
#include "print_funcs.h"
/// bees
#include "app_timers.h"
#include "net_monome.h"

// device-connected flag
//// FIXME: in future, multiple devices.
bool monomeConnect = 0;


// dummy handler
static void dummyHandler(void* op, u32 edata) { return; }

// use led state buffer and dirty flags
/* static void monome_grid_key_loopback(void* op, u32 edata) { */
/*   u8 x, y, z; */
/*   /// FIXME: this stuff should really be abstracted */
/*   monome_grid_key_parse_event_data(edata, &x, &y, &z); */
/*   monomeLedBuffer[x | (y << 4)] = z; */
/*   monome_calc_quadrant_flag(x, y); */
/* } */

/* // use led state buffer and dirty flags */
/* static void monome_ring_enc_loopback(void* op, u32 edata) { */
/*   u8 n; */
/*   s8 val; */
/*   monome_ring_enc_parse_event_data(edata, &n, &val); */
/*   if(val > 0) { */
/*     // FIXME: not sure if this is useful? */
/*     monomeLedBuffer[val + (n<<6)] = 15; */
/*     monomeFrameDirty |= (1<<n); */
/*   } */
/* } */

//---------------------------------
// extern variables, initialized here.


// FIXME: a similar problem to avr32_lib/monome.c
// we are assuming there is only one connected device,
// but of course we would like to implement hub support
// and allow for multiple devices in the future.

// this would means that multiple operators would be mapped
// arbitrarily to different sources! oy...

monome_handler_t monome_grid_key_handler = &dummyHandler; //&monome_grid_key_loopback;
monome_handler_t monome_ring_enc_handler = &dummyHandler; //&monome_ring_enc_loopback;
op_monome_t* monomeOpFocus = NULL;

/// TODO: tilt and key press should be decoupled from grid/ring??? yeah probly. 
/// specialized operators are not really appropriate,
//// have system create additional instances of adc/sw ?? hm
//monome_handler_t monome_grid_tilt_handler = NULL;
//monome_handler_t monome_ring_key_handler = NULL;


// set focus
void net_monome_set_focus(op_monome_t* op_monome, u8 focus) {
  eMonomeDevice dev = monome_device();

  print_dbg("\r\n setting monome grid focus, op pointer: 0x");
  print_dbg_hex((u32)op_monome);
  print_dbg(" , value: ");
  print_dbg_ulong(focus);

  //// FIXME: differentiate on device type (grid/arc)

  if(focus > 0) {
    // aha... set_focus is getting called twice in a row on scene load
    /// (once on _init, once in _unpickle.)
      // we don't want the second call to *unset* focus!
    //    if(monomeOpFocus != NULL ) {
    if((monomeOpFocus != NULL) && (monomeOpFocus != op_monome)) {
      /// stealing focus, inform the previous holder
      monomeOpFocus->focus = 0;
    }
    if(dev == eDeviceGrid) {
      print_dbg("\r\n setting grid_key handler");
      monome_grid_key_handler = op_monome->handler;
    } else if(dev == eDeviceArc) {
      print_dbg("\r\n setting ring_enc handler");
      monome_ring_enc_handler = op_monome->handler;
    } else {
      print_dbg("\r\n warning! requested focus, but no handler was set. "
		" bad device type maybe?");
      // aha... this is ending up here on default scene load. 
      // maybe just a really gruesome delay in the monome comms, needs work 
      // this is kind of bad, but just set both grid and arc handlers by default.
      monome_grid_key_handler = op_monome->handler;
      monome_ring_enc_handler = op_monome->handler;
    }
    monomeOpFocus = op_monome;
    op_monome->focus = 1;
  } else {
    if(dev == eDeviceGrid) {
      monome_grid_key_handler = (monome_handler_t)&dummyHandler;
    } else if(dev == eDeviceArc) {
      monome_ring_enc_handler = (monome_handler_t)&dummyHandler;
    } else {
      ;;
    }
    monomeOpFocus = NULL;
    op_monome->focus = 0;
  }
}

// clear LEDs on grid
void net_monome_grid_clear(void) {
  int i;
  if(monomeConnect) {
    for(i=0; i<MONOME_MAX_LED_BYTES; ++i) {
      monomeLedBuffer[i] = 0;
    }
    monome_set_quadrant_flag(0);
    monome_set_quadrant_flag(1);
    monome_set_quadrant_flag(2);
    monome_set_quadrant_flag(3);
    monome_grid_refresh();
  }
}

// set operator attributes from connected grid device .. ??
void net_monome_set_attributes() {
  //... TODO
}

void net_monome_connect(void) {
  if(monomeConnect != 1) {
    monomeConnect = 1;
    timers_set_monome();
  } else {
    // already connected... oops?
    print_dbg("\r\n net_monome_connect, already connected? oops");
  }
}

// disconnect
void net_monome_disconnect(void) {
  print_dbg("\r\n net_monome_disconnect");
  monomeConnect = 0;
  //  monomeOpFocus = NULL;
  //  monome_grid_key_handler = (monome_handler_t)&dummyHandler;
  timers_unset_monome();
}
