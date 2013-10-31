/*
  page_play.c

  the play page is a little different from the others,
  in that it doesn't initiate rendering directly.
  it simply defines handlers connected to UI operator inputs.

  screen update happens from the more general play.c module,
  which is potentially accessed any time an input node is activated.
  
 */

#include "pages.h"
#include "play.h"
#include "render.h"

//-------------------------
//---- static

// handler declarations
static void handle_enc_0(s32 val);
static void handle_enc_1(s32 val);
static void handle_enc_2(s32 val);
static void handle_enc_3(s32 val);
static void handle_key_0(s32 val);
static void handle_key_1(s32 val);
static void handle_key_2(s32 val);
static void handle_key_3(s32 val);

void handle_key_0(s32 val) {
}

void handle_key_1(s32 val) {
}

void handle_key_2(s32 val) {
}

void handle_key_3(s32 val) {
}

void handle_enc_0(s32 val) {
}

void handle_enc_1(s32 val) {
}

void handle_enc_2(s32 val) {
}

void handle_enc_3(s32 val) {
}


//----------------------
// ---- extern 
// refresh 
void refresh_play(void) {
  play_enable_render();
}

const page_handler_t handler_play[eNumPageHandlers] = {
  &handle_enc_0,
  &handle_enc_1,
  &handle_enc_2,
  &handle_enc_3,
  &handle_key_0,
  &handle_key_1,
  &handle_key_2,
  &handle_key_3,
};
