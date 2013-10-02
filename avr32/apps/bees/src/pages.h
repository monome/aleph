#ifndef _ALEPH_AVR32_APP_BEES_PAGE_H_
#define _ALEPH_AVR32_APP_BEES_PAGE_H_

//------------
//-- define

#define NUM_PAGES 8
// top-level state
typedef enum ePage_t {
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
typedef void(*ui_handler_t)(s32 val);
//typedef void(*enc_handler_t)(uiKey_t key, s32 inc);

// function pointer for redraw on a given page
typedef void(*page_render_t)(void);

// messy page mode enum
typedef enum  { 
   eModeNone,
   eModeClear,
   eModeCopy,
   eModeWrite,
   eModeRead,
   eModeDefault,
} ePageMode;

// class representing a page in a menu
// includes a title and a key input handler (function pointer)
typedef struct page_struct {
  const char* pageString;
  _t keyHandler;
  redraw_t redraw;
  s16 selected;
  ePageMode mode;
  s8 cursor;
  // knob sensitivities for this page
  s8 encSens[4];
} page_t;


//----------------------
//--- static variables


/// arrays of handlers (function key and encoder)
// just storing pointers-to-function instead of [], 
/// avoid deep-copy
static ui_handler_t * page_ui_handlers;
//static enc_handler_t * page_enc_handlers;

#endif  // h guard
