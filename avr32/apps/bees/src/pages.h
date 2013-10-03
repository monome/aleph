#ifndef _ALEPH_AVR32_APP_BEES_PAGE_H_
#define _ALEPH_AVR32_APP_BEES_PAGE_H_

//------------
//-- define

// number of pages (including modal pages)
#define NUM_PAGES 8

// enum of key handlers per page
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
typedef void(*page_refresh_t)(void);

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
  // page title
  const char* name;
  // refresh function (call on selection)
  const page_refresh_t refresh;
  // key handler functions
  const page_handler_t * handler;
  // current mode
  ePageMode mode;
  // current row selection
  s16 select;
  // cursor position
  s8 cursor;
  // knob sensitivities for this page
  s8 encSens[4];
} page_t;


// handles
extern const page_handler_t handler_ins[eNumPageHandlers];
extern const page_handler_t handler_outs[eNumPageHandlers];
extern const page_handler_t handler_presets[eNumPageHandlers];
extern const page_handler_t handler_ops[eNumPageHandlers];
extern const page_handler_t handler_scenes[eNumPageHandlers];
extern const page_handler_t handler_dsp[eNumPageHandlers];
extern const page_handler_t handler_gathered[eNumPageHandlers];
extern const page_handler_t handler_play[eNumPageHandlers];

// init functions
extern void init_page_ins(void);
extern void init_page_outs(void);
extern void init_page_presets(void);
extern void init_page_ops(void);
extern void init_page_scenes(void);
extern void init_page_dsp(void);
extern void init_page_gathered(void);
extern void init_page_play(void);

// refresh functions
extern void refresh_ins(void);
extern void refresh_outs(void);
extern void refresh_presets(void);
extern void refresh_ops(void);
extern void refresh_scenes(void);
extern void refresh_dsp(void);
extern void refresh_gathered(void);
extern void refresh_play(void);

#endif  // h guard
