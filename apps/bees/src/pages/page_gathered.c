/*
  page_gathered.c
 */

#include "net_protected.h"
#include "pages.h"
#include "render.h"

//-------------------------
//---- static

// array of onode pointers for gathering
u32(*gathered)[NET_OUTS_MAX];
// how many gathered
u32 numGathered;


// handler declarations
static void handle_enc_3(s32 val);
static void handle_enc_2(s32 val);
static void handle_enc_1(s32 val);
static void handle_enc_0(s32 val);
static void handle_key_0(s32 val);
static void handle_key_1(s32 val);
static void handle_key_2(s32 val);
static void handle_key_3(s32 val);

// function keys
void handle_key_0(s32 val) {
}

void handle_key_1(s32 val) {
}

void handle_key_2(s32 val) {
}

void handle_key_3(s32 val) {
}

// enc 0 : scroll page
void handle_enc_3(s32 val) {
}

// enc 1 : scroll selection
void handle_enc_2(s32 val) {
}

void handle_enc_1(s32 val) {
}

void handle_enc_0(s32 val) {
}


//----------------------
// ---- extern 
// select 
void select_gathered(void) {
}

const page_handler_t handler_gathered[eNumPageHandlers] = {
  &handle_enc_0,
  &handle_enc_1,
  &handle_enc_2,
  &handle_enc_3,
  &handle_key_0,
  &handle_key_1,
  &handle_key_2,
  &handle_key_3,
};
