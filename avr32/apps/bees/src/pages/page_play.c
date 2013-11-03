/*
  page_play.c

  the play page is a little different from the others,
  in that it doesn't initiate rendering directly.
  it simply defines handlers connected to UI operator inputs.

  screen update happens from the more general play.c module,
  which is potentially accessed any time an input node is activated.
  
 */

// aleph-avr32
#include "app.h"
#include "event_types.h"
// bees
#include "net.h"
#include "pages.h"
#include "play.h"
#include "render.h"


//-------------------------
//---- static


// indices of system operator inputs
// set them in page_init
static s16 sw0_idx;
static s16 sw1_idx;
static s16 sw2_idx;
static s16 sw3_idx;
static s16 enc0_idx;
static s16 enc1_idx;
static s16 enc2_idx;
static s16 enc3_idx;

// handler declarations
static void handle_enc_0(s32 val);
static void handle_enc_1(s32 val);
static void handle_enc_2(s32 val);
static void handle_enc_3(s32 val);
static void handle_key_0(s32 val);
static void handle_key_1(s32 val);
static void handle_key_2(s32 val);
static void handle_key_3(s32 val);

// handlers activate system UI operator nodes.

void handle_key_0(s32 val) {
  net_activate(sw0_idx, val, NULL);
}

void handle_key_1(s32 val) {
  net_activate(sw1_idx, val, NULL);
}

void handle_key_2(s32 val) {
  net_activate(sw2_idx, val, NULL);
}

void handle_key_3(s32 val) {
  net_activate(sw3_idx, val, NULL);
}

void handle_enc_0(s32 val) {
  net_activate(enc0_idx, val, NULL);
}

void handle_enc_1(s32 val) {
  net_activate(enc1_idx, val, NULL);
}

void handle_enc_2(s32 val) {
  net_activate(enc2_idx, val, NULL);
}

void handle_enc_3(s32 val) {
  net_activate(enc3_idx, val, NULL);
}

//----------------------
// ---- extern 

// init
void init_page_play(void) {
  // assign system input indices
  sw0_idx = net_op_in_idx(4, 0);
  sw1_idx = net_op_in_idx(5, 0);
  sw2_idx = net_op_in_idx(6, 0);
  sw3_idx = net_op_in_idx(7, 0);
  enc0_idx = net_op_in_idx(0, 0);
  enc1_idx = net_op_in_idx(1, 0);
  enc2_idx = net_op_in_idx(2, 0);
  enc3_idx  = net_op_in_idx(3, 0);
}

// select 
void select_play(void) {
  play_enable_render();
  // other regions are static in top-level render, with global handles
  region_fill(headRegion, 0x0);
  font_string_region_clip(headRegion, "PLAY", 0, 0, 0xf, 0x1);
  // assign handlers
  app_event_handlers[ kEventEncoder0 ]	= &handle_enc_0 ;
  app_event_handlers[ kEventEncoder1 ]	= &handle_enc_1 ;
  app_event_handlers[ kEventEncoder2 ]	= &handle_enc_2 ;
  app_event_handlers[ kEventEncoder3 ]	= &handle_enc_3 ;
  app_event_handlers[ kEventSwitch0 ]	= &handle_key_0 ;
  app_event_handlers[ kEventSwitch1 ]	= &handle_key_1 ;
  app_event_handlers[ kEventSwitch2 ]	= &handle_key_2 ;
  app_event_handlers[ kEventSwitch3 ]	= &handle_key_3 ;
}
