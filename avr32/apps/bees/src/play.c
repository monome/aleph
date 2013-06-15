
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
  char* scr;

  /* print_dbg("\r\n play_input, idx: "); */
  /* print_dbg_hex(idx); */
  /* print_dbg(" , inputname: "); */
  /* print_dbg(net_in_name(idx)); */
  /* print_dbg(" , value: "); */
  /* print_dbg_hex(  net_get_in_value(idx) ); */
    scr = scrollBuf + (scrollIdx * PLAY_STR_LEN);

  /* print_dbg("\r\n target pointer: 0x");  */
  /* print_dbg_hex((u32)scr);  */
  /* print_dbg(" / 0x");  */
  /* print_dbg_hex((u32)scrollBuf);  */

  /* print_dbg("\r\n scroll contents: \r\n"); */
  /* print_dbg(scrollBuf); */
    itoa_whole(idx, scr + kIdxOff, 3 );
    str_copy(scr + kNameOff, net_in_name(idx), PARAM_LABEL_LEN );
    print_fix16( scr + kValOff, net_get_in_value(idx) );
    print_dbg("\r\n play_input printed value: ");
    print_dbg_hex( (unsigned long int)net_get_in_value(idx));

  scrollIdx++;
  if(scrollIdx > PLAY_SCROLL_NUM) {
    scrollIdx = 0;
  }
}

// return text buffer for given entry
const char * play_get_entry(u16 idx) {
  u16 getIdx;
  getIdx = scrollIdx + 1 + idx;
  while(getIdx >= PLAY_SCROLL_NUM) {
    getIdx -= PLAY_SCROLL_NUM;
  }  
  //  print_dbg("\r\n play entry idx (after wrap) : ");
  //  print_dbg_hex((unsigned long int)getIdx);
  return (const char*)( scrollBuf + (getIdx * PLAY_STR_LEN) );
}

// add parameter data to the end of the scroll  
void play_param(u16 paramIdx) {
  /* /// this part works... */
  /* itoa_whole( paramIdx, scrollBuf[scrollIdx] + kIdxOff, 3 ); */
  /* // this doesn't work?? */
  /* str_copy( scrollBuf[scrollIdx] + kNameOff, get_param_name(paramIdx), PARAM_LABEL_LEN ); */
  /* print_dbg("\r\n playparam: "); */
  /* print_dbg(get_param_name(paramIdx)); */

  /* // this doesn't work??? */
  /* print_fix16( scrollBuf[scrollIdx] + kValOff, get_param_value(paramIdx) ); */
  /* scrollIdx++; */
  /* if(scrollIdx > PLAY_SCROLL_NUM) { */
  /*   scrollIdx = 0; */
  /* } */
}
