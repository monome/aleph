/* 
   net_monome.h

   monome management routines for the bees control network.
   handles operator focus, routes handler/refresh requests

*/

#ifndef _ALEPH_BEES_NET_MONOME_H_
#define _ALEPH_BEES_NET_MONOME_H_

// common
#include "types.h"
// avr32
#include "monome.h"
// bees
// #include "ops/op_monome.h"


// handler function type
typedef void(*monome_handler_t)(void* op, u32 event);

// abstract superclass for monome operators
// has event handler and focus flag
typedef struct _op_monome {
  // handler function, will connect to app event handler
  monome_handler_t handler;
  // focus flag
  u8 focus;
  // pointer to operator subclass
  void* op;
} op_monome_t;

//---------------------
//----- variables

extern monome_handler_t monome_grid_key_handler;
extern monome_handler_t monome_ring_enc_handler;
extern op_monome_t* monomeOpFocus;

// device-connected flag
extern bool monomeConnect;

//------------------------
//----- functions

// initialize
void net_monome_init(void);

// set/release focus
extern void net_monome_set_focus(op_monome_t* grid, u8 focus);

// set operator attributes from connected grid device
extern void net_monome_set_attributes( /* ...???... */void);

// clear LEDs on grid
extern void net_monome_grid_clear(void);

// connect
extern void net_monome_connect(void);
// disconnect
extern void net_monome_disconnect(void);

#endif // h guard
