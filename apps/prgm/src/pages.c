//pages.c
//aleph-prgm-avr32

#ifdef ARCH_AVR32
#include "print_funcs.h"
#include "encoders.h"
#endif

#include "pages.h"
#include "render.h"
#include "screen.h"

// encoder sensitivities
#define ENC_THRESH_PAGESCROLL 32
#define ENC_THRESH_LISTSCROLL 4
#define ENC_THRESH_FINE 0
#define ENC_THRESH_COARSE 4

//--------------------------
//--------- variables

// page structures - synchronize with ePage enum in pages.h
page_t pages[NUM_PAGES] = {
    
    { .name = "LEVEL",
        .select_fn = &select_level, // select function
        .encSens = { 0, 0, ENC_THRESH_PAGESCROLL, ENC_THRESH_LISTSCROLL, }, // encoder sens 
    },
    
    { .name = "ENV",
        .select_fn = &select_env, // select function
        .encSens = { 0, 0, ENC_THRESH_PAGESCROLL, ENC_THRESH_LISTSCROLL, }, // encoder sens
    },
};

// pointer to current page
page_t* curPage;
// idx of current page
s8 pageIdx;
// last pressed key
u8 keyPressed = 255;

// init
void pages_init(void) {
    screen_clear();
    
    print_dbg("\r\n pages init");

    print_dbg("\r\n set page PRGM");
    pageIdx = ePageLevel;
    set_page(pageIdx);
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
    pageIdx = n;
    
    curPage = &(pages[pageIdx]);
    curPage->select_fn();
}

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
