/* pages.h
   bees
   aleph-avr32
   
*/


#ifndef _ALEPH_AVR32_APP_BEES_PAGE_H_
#define _ALEPH_AVR32_APP_BEES_PAGE_H_

#include "types.h"

//------------
//-- define

// number of pages (including modal pages)
#define NUM_PAGES 8

// enum of key handlers per menu page
typedef enum {
  ePageHandleEnc0,
  ePageHandleEnc1,
  ePageHandleEnc2,
  ePageHandleEnc3,
  ePageHandleKey0,
  ePageHandleKey1,
  ePageHandleKey2,
  ePageHandleKey3,
  eNumPageHandlers
} eKeyHandler;

// top-level state
typedef enum {
  ePageIns,
  ePageOuts,
  ePagePresets,
  ePageOps,
  ePageScenes,
  ePageDsp,
  ePageGathered,
  ePagePlay,
} ePage;

// function pointers for input handling on a given page
typedef void(*page_handler_t)(s32 val);
// function pointer for refresh (selection) on a given page
typedef void(*page_select_t)(void);

// class representing a page in a menu
// includes a title and a key input handler (function pointer)
typedef struct page_struct {
  // page title
  const char* name;
  // refresh function (call on selection)
  const page_select_t select_fn;
  // key handler functions
  const page_handler_t * handler;
  // current row selection
  s16 select;
  // cursor position
  s8 cursor;
  // knob sensitivities for this page
  s8 encSens[4];
} page_t;


//-----------------------
//--- extern variables


// page structures
extern page_t pages[NUM_PAGES];
// pointer to current page
extern page_t* curPage;
// idx of current page
extern s8 pageIdx;
// last pressed key
extern u8 keyPressed;
// alt-mode flag
extern u8 altMode;


//-----------------------
//--- extern functions

// handles
/* extern const page_handler_t handler_ins[eNumPageHandlers]; */
/* extern const page_handler_t handler_outs[eNumPageHandlers]; */
/* extern const page_handler_t handler_presets[eNumPageHandlers]; */
/* extern const page_handler_t handler_ops[eNumPageHandlers]; */
/* extern const page_handler_t handler_scenes[eNumPageHandlers]; */
/* extern const page_handler_t handler_dsp[eNumPageHandlers]; */
/* extern const page_handler_t handler_gathered[eNumPageHandlers]; */
/* extern const page_handler_t handler_play[eNumPageHandlers]; */

// init functions
extern void init_page_ins(void);
extern void init_page_outs(void);
extern void init_page_presets(void);
extern void init_page_ops(void);
extern void init_page_scenes(void);
extern void init_page_dsp(void);
extern void init_page_gathered(void);
extern void init_page_play(void);

// select functions
extern void select_ins(void);
extern void select_outs(void);
extern void select_presets(void);
extern void select_ops(void);
extern void select_scenes(void);
extern void select_dsp(void);
extern void select_gathered(void);
extern void select_play(void);


// redraw functions
extern void redraw_ins(void);
extern void redraw_outs(void);
extern void redraw_presets(void);
extern void redraw_ops(void);
extern void redraw_scenes(void);
extern void redraw_dsp(void);
extern void redraw_gathered(void);
extern void redraw_play(void);

//----- public functions
// init all pages
extern void pages_init(void);
// de-init
extern void pages_deinit(void);
// refresh
extern void pages_reselect(void);
// set page
extern void set_page(ePage n);
// check key against last pressed
extern u8 check_key(u8 key);
// toggle play mode
extern u8 pages_toggle_play(void);

 // toggle alt mode
extern u8 pages_set_alt(bool val);

// reset key pressed to cancel any ongoing actions
extern void pages_reset_keypressed(void);

// scroll cursor position in editable string
extern void pages_edit_cursor(s32 val, char* buf, s8* cursor, u16 len);

// scroll character at cursor position in editable string
extern void pages_edit_char_inc(char* str, u8 pos);
extern void pages_edit_char_dec(char* str, u8 pos);

#endif  // h guard
