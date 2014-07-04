//
//  pages.c
//  main
//
//  Created by Staffan Jansson on 7/1/14.
//  Copyright (c) 2014 __MyCompanyName__. All rights reserved.
//

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
    { .name = "PRGM",
        .select_fn = &select_prgm, // select function
        .encSens = { 0, 0, ENC_THRESH_PAGESCROLL, ENC_THRESH_LISTSCROLL, }, // encoder sens 
    },
    { .name = "TRACKER",
        .select_fn = &select_tracker, // select function
        .encSens = { 0, 0, ENC_THRESH_PAGESCROLL, ENC_THRESH_LISTSCROLL, }, // encoder sens 
    }
};

// pointer to current page
page_t* curPage;
// idx of current page
s8 pageIdx;
// last pressed key
u8 keyPressed = 255;

//static variables

// saved idx of last non-play page
//static s8 lastPageIdx = 0;

//-----------------------------------
//----- external function definitions

// init
void pages_init(void) {
    screen_clear();
    
    print_dbg("\r\n pages init");

    print_dbg("\r\n set page prgm");
    pageIdx = ePagePrgm;
    set_page(pageIdx);

//    print_dbg("\r\n redram page prgm");
//    redraw_prgm();
//    redraw_tracker();
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
//    u8 i;
    pageIdx = n;
    
    curPage = &(pages[pageIdx]);
    curPage->select_fn();
/*
    for(i=0; i<4; i++) {
        set_enc_thresh(i, curPage->encSens[i]);
    }
*/
    print_dbg("\r\n ...done setting page.");
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
