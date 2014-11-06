/*
  page_play.c

  the play page is a little different from the others,
  in that it doesn't initiate rendering directly.
  it simply defines handlers connected to UI operator inputs.

  screen update happens from the more general play.c module,
  which is potentially accessed any time an input node is activated.
  
 */

// asf
#include "print_funcs.h"

// bees
#include "app.h"
#include "net_protected.h"
#include "pages.h"
#include "play.h"
#include "render.h"


//-------------------------
//---- static


// indices of system operator inputs
// set them in page_init
/* static s16 sw0_idx; */
/* static s16 sw1_idx; */
/* static s16 sw2_idx; */
/* static s16 sw3_idx; */
/* static s16 enc0_idx; */
/* static s16 enc1_idx; */
/* static s16 enc2_idx; */
/* static s16 enc3_idx; */

// handler declarations
static void handle_enc_3(s32 val);
static void handle_enc_2(s32 val);
static void handle_enc_1(s32 val);
static void handle_enc_0(s32 val);
static void handle_key_0(s32 val);
static void handle_key_1(s32 val);
static void handle_key_2(s32 val);
static void handle_key_3(s32 val);

// handlers activate system UI operator nodes.

void handle_key_0(s32 val) {
  //   print_dbg("\r\n play handler, key 0, op address: 0x");
  //   print_dbg_hex((u32)opSysSw[0]);
   op_sw_sys_input(opSysSw[0], (s8)val);
}

void handle_key_1(s32 val) {
  //   print_dbg("\r\n play handler, key 1, op address: 0x");
  //   print_dbg_hex((u32)opSysSw[1]);
   op_sw_sys_input(opSysSw[1], (s8)val);
}

void handle_key_2(s32 val) {
  //   print_dbg("\r\n play handler, key 2, op address: 0x");
  //   print_dbg_hex((u32)opSysSw[2]);
   op_sw_sys_input(opSysSw[2], (s8)val);
}

void handle_key_3(s32 val) {
  //   print_dbg("\r\n play handler, key 3, op address: 0x");
  //   print_dbg_hex((u32)opSysSw[3]);
   op_sw_sys_input(opSysSw[3], (s8)val);
}


void handle_enc_3(s32 val) {
  //  print_dbg("\r\n play handler, enc 0, op address: 0x");
  //  print_dbg_hex((u32)opSysEnc[0]);
// FIXME: this is a hack to make the operators work with the correct numbers
  op_enc_sys_input(opSysEnc[3], (s8)val);
}

void handle_enc_2(s32 val) {
  //  print_dbg("\r\n play handler, enc 1, op address: 0x");
  //  print_dbg_hex((u32)opSysEnc[1]);
   op_enc_sys_input(opSysEnc[1], (s8)val);
}

void handle_enc_1(s32 val) {
  //  print_dbg("\r\n play handler, enc 2, op address: 0x");
  //  print_dbg_hex((u32)opSysEnc[2]);
   op_enc_sys_input(opSysEnc[2], (s8)val);
}

void handle_enc_0(s32 val) {
  // FIXME: this is a hack to make the operators work with the correct numbers
   op_enc_sys_input(opSysEnc[0], (s8)val);
}

//----------------------
// ---- extern 

// init
void init_page_play(void) {
}

// select 
void select_play(void) {
  
  // other regions are static in top-level render, with global handles
  region_fill(footRegion[0], 0x0);
  region_fill(footRegion[1], 0x0);
  region_fill(footRegion[2], 0x0);
  region_fill(footRegion[3], 0x0);
  region_fill(headRegion, 0x0);

  play_enable_render();
  
  //  font_string_region_clip(headRegion, "PLAY", 0, 0, 0xf, 0x1);
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
