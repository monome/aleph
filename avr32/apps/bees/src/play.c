
/* play.c
   bees
   aleph

   implementation for play mode UI
 */

// std
#include "string.h"
// common
#include "fix.h"
// avr32
#include "print_funcs.h"
#include "simple_string.h"
// bees
#include "menu_protected.h"
#include "play.h"

// big text buffer
static char scrollBuf[PLAY_SCROLL_BUF_LEN];

// offsets into each text buffer
static const u8 kIdxOff  = 0;
static const u8 kNameOff = 4;
static const u8 kValOff  = 14;

// a scroll of paramter name/value pars
// static touched_t touchedParams[PLAY_SCROLL_NUM];
// current idx into scroll
static u16 scrollIdx = 0;

//-------------------------------
//--- external functions

// intiailize
void play_init(void) {
  u8 i, j;
  for(i=0; i<PLAY_SCROLL_NUM; i++) {
    for(j=0; j<PLAY_STR_LEN; j++) {
      //      print_dbg("\r\n buf fill index: ");
      //      print_dbg_hex(i * PLAY_STR_LEN + j);
      scrollBuf[i * PLAY_STR_LEN + j] = 0x20;
      scrollBuf[i * PLAY_STR_LEN + PLAY_STR_LEN - 1] = 0;
    }
  }
}

// add input node data to scroll
void play_input(u16 idx) {
  char* scr = scrollBuf + (scrollIdx * PLAY_STR_LEN);
  itoa_whole(idx, scr + kIdxOff, 3 );
  str_copy(scr + kNameOff, net_in_name(idx), PARAM_LABEL_LEN );
  print_fix16( scr + kValOff, net_get_in_value(idx) );
  //  print_dbg("\r\n play entry:");
  //  print_dbg(scr);
  scrollIdx++;
  if(scrollIdx > PLAY_SCROLL_NUM) {
    scrollIdx = 0;
  }
}

// return text buffer for given entry
const char * play_get_entry(u16 idx) {
  u16 getIdx;
  // +1 for oldest entry, offset by idx, wrap
  getIdx = scrollIdx + 1 + idx;
  while(getIdx >= PLAY_SCROLL_NUM) {
    getIdx -= PLAY_SCROLL_NUM;
  }  
  return (const char*)( scrollBuf + (getIdx * PLAY_STR_LEN) );
}

// add parameter data to the end of the scroll  
void play_param(u16 paramIdx) {
 
}
