/* net_monome.c
   bees
   aleph

   monome driver <-> operator tranlation layer

   keeps pointers to handler functions and framebuffer
   manages operator focus
 */

#include "net_monome.h"



// dummy handler
static void dum(event_t* ev) { return; }
// loopback  handler
static void monome_grid_loopback(event_t* ev) {
  u8 x, y, z;
  monome_grid_key_read_event(ev, &x, &y, &z);
  (*monome_grid_led)(x, y, z);
}


//---------------------------------
// extern variables, initialized here.

monome_handler_t monome_grid_key_handler = &monome_grid_loopback;
monome_handler_t monome_ring_enc_handler = &dum;

/// TODO: tilt and key press should be decoupled from grid/ring??? yeah probly. 
//// have system create additional adc operators?? hm
//monome_handler_t monome_grid_tilt_handler = NULL;
//monome_handler_t monome_ring_key_handler = NULL;

//--------------------------
//---- extern functions
// init
void net_monome_init(void) {
  u32 i;
  for(i=0; i<MONOME_MAX_LED_BYTES; i++) {
    monomeLedBuffer[i] = 0;
  }
}

// set focus
void net_monome_set_focus(op_monome_t* grid) {
  monome_grid_key_handler = grid->handler;
  grid->focus = 1;
}

// set operator attributes from connected grid device
void net_monome_set_attributes() {
  //... TODO
}

