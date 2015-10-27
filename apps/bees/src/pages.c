/*
  pages.c
  bees
  aleph
*/

#ifdef ARCH_AVR32
#include "print_funcs.h"
#include "encoders.h"
#endif

#include "pages.h"
//#include "pages_protected.h"
#include "net.h"
#include "op.h"
#include "pages.h"
#include "param.h"
#include "preset.h"
#include "render.h"
#include "screen.h"
#include "ui.h"

// encoder sensitivities
#define ENC_THRESH_PAGESCROLL 32
#define ENC_THRESH_LISTSCROLL 4
#define ENC_THRESH_FINE 0
#define ENC_THRESH_COARSE 4

// min/max legal characters for editable strings
#define MAX_EDIT_CHAR 122
#define MIN_EDIT_CHAR 45


//--------------------------
//--------- variables

// page structures - synchronize with ePage enum in pages.h
page_t pages[NUM_PAGES] = {
  { .name = "INS",
    .select_fn = &select_ins, // select function
    //    .encSens = { 0, 0, ENC_THRESH_PAGESCROLL, ENC_THRESH_LISTSCROLL, }, // encoder sens 
    .encSens = { ENC_THRESH_LISTSCROLL, ENC_THRESH_PAGESCROLL,  0, 0,}, // encoder sens 
  },
  { .name = "OUTS",
    .select_fn = &select_outs, // select function
    .encSens = { ENC_THRESH_LISTSCROLL, ENC_THRESH_PAGESCROLL,  0, 8, }, // encoder sens 
  },
  { .name = "PRESETS",
    .select_fn = &select_presets, // select function
    .encSens = { ENC_THRESH_LISTSCROLL, ENC_THRESH_PAGESCROLL,  8, 8, }, // encoder sens 
  },
  { .name = "OPS",
    .select_fn = &select_ops, // select function
    .encSens = { ENC_THRESH_LISTSCROLL, ENC_THRESH_PAGESCROLL,  0, 12, }, // encoder sens 
  },
  { .name = "SCENES",
    .select_fn = &select_scenes, // select function
    .encSens = { ENC_THRESH_LISTSCROLL, ENC_THRESH_PAGESCROLL,  8, 8, }, // encoder sens 
  },
  { .name = "DSP",
    .select_fn = &select_dsp, // select function
    .encSens = { ENC_THRESH_LISTSCROLL, ENC_THRESH_PAGESCROLL,  0, 0, }, // encoder sens 
  },
  // modal:
  { .name = "GATHERED", 
    .select_fn = &select_gathered , // select function
    .encSens = { ENC_THRESH_LISTSCROLL, ENC_THRESH_PAGESCROLL,  0, 0, }, // encoder sens 
  },
  { .name = "PLAY",
    .select_fn = &select_play, // select function
    .encSens = { 0, 0, 0, 0, }, // encoder sens 
  }
};

// pointer to current page
page_t* curPage;
// idx of current page
s8 pageIdx;
// last pressed key
u8 keyPressed = 255;
// alt-mode flag
u8 altMode = 0;

//--------------
//--- static vars
// saved idx of last non-play page
static s8 lastPageIdx = 0;



//---------------
//--- static funcs

/// utility to restrict characters in legal filenames
// return 0 if ok
static u8 check_edit_char(char c) {
  /*
    45 		dash
    48-57 	nums
    65-90	upper
    95		underscore
    97-122	lower
  */
  if(c == 45) { return 0; }
  if(c == 95) { return 0; }
  if( (c >= 48) && (c <= 57) ) { return 0; }
  if( (c >= 65) && (c <= 90) ) { return 0; }
  if( (c >= 97) && (c <= 122) ) { return 0; }
  return 1;
}

//-----------------------------------
//----- external function definitions

// init
void pages_init(void) {
  init_page_ins();
  init_page_dsp();
  init_page_ops();  
  init_page_outs();
  init_page_play();
  init_page_presets();
  init_page_scenes();
  /*
  // TODO
  init_page_gathered();
  */


  pageIdx = ePageScenes;
  set_page(pageIdx);

  redraw_dsp();
  redraw_outs();
  redraw_ops();
  redraw_presets();
  redraw_ins();
  //... last
  redraw_scenes();

}

// de-init
void pages_deinit(void) {
}

// re-select
void pages_reselect(void) {
  curPage->select_fn();
}

// set current page
void set_page(ePage n) {
  u8 i;
  pageIdx = n;
  
  curPage = &(pages[pageIdx]);
  curPage->select_fn();

  //  print_dbg("\r\n set enc sense");
  // set encoder sensitivity
  for(i=0; i<4; i++) {
    //// FIXME: use encoder map?
    //set_enc_thresh(encMap[i], curPage->encSens[i]);
    set_enc_thresh(i, curPage->encSens[i]);
  }
  //  print_dbg("\r\n ...done setting page.");
}

// check key against last pressed
u8 check_key(u8 key) {
  u8 ret;
  if(keyPressed == key) {
    keyPressed = 255;
    ret = 1;
  } else {
    keyPressed = key;
    ret = 0;
  }
  return ret;
}

// toggle play mode
u8 pages_toggle_play(void) {
  u8 ret;
  if(pageIdx == ePagePlay) {
    ret = 0;
    set_page(lastPageIdx);
  } else {
    ret = 1;
    lastPageIdx = pageIdx;
    set_page(ePagePlay);
  }
  return ret;
}

// toggle alt mode
u8 pages_set_alt(bool v) {
  if(v > 0) {
    altMode = 1;
  } else {
    altMode = 0;
  }
  return altMode;
}


// reset keypressed field externally to cancel any ongoing actions
void pages_reset_keypressed(void) { 
  keyPressed = 255;
}

// utility for editing name
void pages_edit_cursor(s32 val, char* buf, s8* cursor, u16 len) { 
  s8 _cursor;
  u8 i;
  if(val > 0) { 
    _cursor = *cursor + 1;
  } else {
    _cursor = *cursor - 1;
  }
  if(_cursor < 0) { _cursor = 0; }
  if(_cursor >= len) { _cursor = len - 1; }
  *cursor = _cursor;

  if(altMode) { 
    if(_cursor == 0) {
      buf[_cursor] = '_';
    } else {
      i = _cursor;
      do{
	  buf[i] = '\0';
	  ++i;
	}
      while(i < len);
    }
  } else {
    if(buf[_cursor] == '\0') { 
      if(_cursor > 0) { 
	buf[_cursor] = buf[_cursor-1] ;
      } else {
	buf[_cursor] = '_';
      }
    }  
  }
}

// scroll character up
void pages_edit_char_inc (char* str, u8 pos) {
  s32 tmp = (s32)str[pos];
  if(tmp == '\0') { tmp = '_'; } else { ++tmp; }
  while(check_edit_char((char)tmp)) {
    ++tmp;
    if(tmp > MAX_EDIT_CHAR) {
      tmp = MAX_EDIT_CHAR;
    }
  }
  str[pos] = tmp;
}

// scroll character down
void pages_edit_char_dec (char* str, u8 pos) {
  u8 tmp = str[pos];
  if(tmp == '\0') { tmp = '_'; } else { --tmp; }
  while(check_edit_char((char)tmp)) {
    --tmp;
    if(tmp < MIN_EDIT_CHAR) {
      tmp = MIN_EDIT_CHAR;
    }
  }
  str[pos] = tmp;
}


