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
#include "ops/op_monome.h"


//---------------------
//----- variables

extern monome_handler_t monome_grid_key_handler;
//extern monome_handler_t monome_ring_enc_handler;

// a buffer big enough to hold all l data for 256 or arc4
// each led gets a full byte
extern u8 monomeLedBuffer[MONOME_MAX_LED_BYTES];

//------------------------
//----- functions

// initialize
void net_monome_init(void);

// set focus
extern void net_monome_set_focus(op_monome_t* grid);

// set operator attributes from connected grid device
extern void net_monome_set_attributes( /* ...???... */void);

#endif // h guard
