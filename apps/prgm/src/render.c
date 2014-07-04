//prgm
//aleph-avr32
//render.c

//--- std headers
#include <string.h>

//debug
#include "print_funcs.h"

//avr32-lib
#include "fix.h"
#include "app.h"
#include "font.h"
#include "memory.h"
#include "region.h"
#include "screen.h"

//prgm
#include "pages.h"
#include "render.h" //includes region.h
#include "util.h"
#include "prgm.h"
#include "tracker.h"
#include "ctl.h"

//regions
static const u8 numRegions = 4;

static region prgm[4] = {
    {.w=64, .h=32, .x = 0, .y = 0},
    {.w=64, .h=32, .x = 64, .y = 0},
    {.w=64, .h=32, .x = 0, .y = 32},
    {.w=64, .h=32, .x = 64, .y = 32}
};

static region tracker = {.w=128, .h=64, .x = 0, .y = 0};

//array of pointers to prgm regions
static region *prgmRegions[] = {
    &(prgm[0]),
    &(prgm[1]),
    &(prgm[2]),
    &(prgm[3]),
};

static region bootScrollRegion = {.w = 128, .h = 64, .x = 0, .y = 0};

static scroll bootScroll;


//initialization, called by app_init()
void render_init(void) {
    region_alloc((region*)(&bootScrollRegion)); //BOOT: declared in region.h, calls mem_alloc() declared in memory.h
    scroll_init(&bootScroll, &bootScrollRegion); //BOOT

    u32 i;    
    for(i = 0; i<numRegions; i++) {
        region_alloc((region*)(prgmRegions[i]));
    }    
    region_alloc((region*)(&tracker));
}

void render_boot(const char* str) {
    scroll_string_front(&bootScroll, (char*)str);
}

void render_startup (void) {
    screen_clear();
    set_page(ePagePrgm);
    Freq0 = Freq1 = Freq2 = Freq3 = 220 * 0x00010000;
    print_fix16(renderFreq0, Freq0);
    print_fix16(renderFreq1, Freq1);
    print_fix16(renderFreq2, Freq2);
    print_fix16(renderFreq3, Freq3);
    render_prgm();
}

void render_update(void) {
    region *r;
    u8 i;
    
    app_pause();
    
    if(bootScroll.reg->dirty) {
        scroll_draw(&bootScroll);
    }
    
    for(i = 0; i<numRegions; i++) {
        r = prgmRegions[i]; 
        if(r->dirty) {
            screen_draw_region(r->x, r->y, r->w, r->h, r->data);
            r->dirty = 0;
        }
    }
    
    r = &tracker;
    if(r->dirty) {
        screen_draw_region(r->x, r->y, r->w, r->h, r->data);
            r->dirty = 0;
        }
    
    app_resume();
}

//draw prgm page
void render_prgm(void) {
    u8 i;
    for(i = 0; i<numRegions; i++) {
        region_fill(&prgm[i], 0x0);
    }
    region_string(&prgm[0], renderFreq0, 0, 0, 0xf, 0, 0);
    region_string(&prgm[1], renderFreq1, 0, 0, 0xf, 0, 0);
    region_string(&prgm[2], renderFreq2, 0, 0, 0xf, 0, 0);
    region_string(&prgm[3], renderFreq3, 0, 0, 0xf, 0, 0);
}

void render_freq(void) {
    region_string(&prgm[0], renderFreq0, 0, 0, 0xf, 0, 0);
    region_string(&prgm[1], renderFreq1, 0, 0, 0xf, 0, 0);
    region_string(&prgm[2], renderFreq2, 0, 0, 0xf, 0, 0);
    region_string(&prgm[3], renderFreq3, 0, 0, 0xf, 0, 0);
}

//draw tracker page
void render_tracker(void) {
    region_fill(&tracker, 0x0);
    region_string(&tracker, "TRACKER", 0, 0, 0xf, 0, 0);
}

