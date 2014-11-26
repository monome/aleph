//pages.h
//aleph-prgm-avr32

#ifndef _ALEPH_APP_PRGM_PAGES_H_
#define _ALEPH_APP_PRGM_PAGES_H_

#include "types.h"
#include "event_types.h"
#include "encoders.h"

//------------
//-- define

// number of pages (including modal pages)
#define NUM_PAGES 2

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
    ePageHandleKey4,
    eNumPageHandlers
} eKeyHandler;

// top-level state
typedef enum {
    ePageLevel,
    ePageEnv
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

extern void render_level(void);
extern void render_env(void);
//extern void render_pattern(void);

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


//external functions
//page select
extern void select_level(void);
extern void select_env(void);
//extern void select_pattern(void);

//page redraw
extern void redraw_level(void);
extern void redraw_env(void);
//extern void redraw_pattern(void);

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

#endif  // h guard
