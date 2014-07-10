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
static region bootScrollRegion = {.w = 128, .h = 64, .x = 0, .y = 0};

static scroll bootScroll;

static const u8 numRegions = 4;

static region prgm[4] = {
    {.w=64, .h=32, .x = 0, .y = 0},
    {.w=64, .h=32, .x = 64, .y = 0},
    {.w=64, .h=32, .x = 0, .y = 32},
    {.w=64, .h=32, .x = 64, .y = 32}
};

static region *prgmRegions[] = {
    &(prgm[0]),
    &(prgm[1]),
    &(prgm[2]),
    &(prgm[3]),
};

static region tracker[4] = {
    {.w=64, .h=32, .x = 0, .y = 0},
    {.w=64, .h=32, .x = 64, .y = 0},
    {.w=64, .h=32, .x = 0, .y = 32},
    {.w=64, .h=32, .x = 64, .y = 32}
};

static region *trackerRegions[] = {
    &(tracker[0]),
    &(tracker[1]),
    &(tracker[2]),
    &(tracker[3]),
};


//initialization, called by app_init()
void render_init(void) {
    region_alloc((region*)(&bootScrollRegion)); //BOOT: declared in region.h, calls mem_alloc() declared in memory.h
    scroll_init(&bootScroll, &bootScrollRegion); //BOOT

    u32 i;    
    for(i = 0; i<numRegions; i++) {
        region_alloc((region*)(prgmRegions[i]));
    }    

    for(i = 0; i<numRegions; i++) {
        region_alloc((region*)(trackerRegions[i]));
    }    
    
    tracker_init();
}

void render_boot(const char* str) {
    scroll_string_front(&bootScroll, (char*)str);
}

void render_startup (void) {
    screen_clear();
    set_page(ePageTracker);
    Freq0 = Freq1 = Freq2 = Freq3 = 220 * 0x00010000;
    print_fix16(renderFreq0, Freq0);
    print_fix16(renderFreq1, Freq1);
    print_fix16(renderFreq2, Freq2);
    print_fix16(renderFreq3, Freq3);
    render_tracker();
    
    Phase0 = Phase1 = Phase2 = Phase3 = 0;
    print_fix16(renderPhase0, Phase0);
    print_fix16(renderPhase1, Phase1);
    print_fix16(renderPhase2, Phase2);
    print_fix16(renderPhase3, Phase3);
    
    Blend0 = Blend1 = Blend2 = Blend3 = 0;
    print_fix16(renderBlend0, Blend0);
    print_fix16(renderBlend1, Blend1);
    print_fix16(renderBlend2, Blend2);
    print_fix16(renderBlend3, Blend3);
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
    
    for(i = 0; i<numRegions; i++) {
        r = trackerRegions[i]; 
        if(r->dirty) {
            screen_draw_region(r->x, r->y, r->w, r->h, r->data);
            r->dirty = 0;
        }
    }

    app_resume();
}

//draw prgm page
void render_prgm(void) {
    u8 i;
    for(i = 0; i<numRegions; i++) {
        region_fill(&prgm[i], 0x0);
    }
    /* region, string, offset x, offset y, color a, color b, size */
    region_string(&prgm[0], renderPhase0, 0, 8, 0xf, 0, 0);
    region_string(&prgm[1], renderPhase1, 0, 8, 0xf, 0, 0);
    region_string(&prgm[2], renderPhase2, 0, 8, 0xf, 0, 0);
    region_string(&prgm[3], renderPhase3, 0, 8, 0xf, 0, 0);
    
    region_string(&prgm[0], renderBlend0, 0, 16, 0xf, 0, 0);
    region_string(&prgm[1], renderBlend1, 0, 16, 0xf, 0, 0);
    region_string(&prgm[2], renderBlend2, 0, 16, 0xf, 0, 0);
    region_string(&prgm[3], renderBlend3, 0, 16, 0xf, 0, 0);    
}

void render_phase(void) {
    region_string(&prgm[0], renderPhase0, 0, 8, 0xf, 0, 0);
    region_string(&prgm[1], renderPhase1, 0, 8, 0xf, 0, 0);
    region_string(&prgm[2], renderPhase2, 0, 8, 0xf, 0, 0);
    region_string(&prgm[3], renderPhase3, 0, 8, 0xf, 0, 0);
}

void render_blend(void) {
    region_string(&prgm[0], renderBlend0, 0, 16, 0xf, 0, 0);
    region_string(&prgm[1], renderBlend1, 0, 16, 0xf, 0, 0);
    region_string(&prgm[2], renderBlend2, 0, 16, 0xf, 0, 0);
    region_string(&prgm[3], renderBlend3, 0, 16, 0xf, 0, 0);
}

//draw tracker page
void render_tracker(void) {
    u8 i;
    for(i = 0; i<numRegions; i++) {
        region_fill(&tracker[i], 0x0);
    }

    region_string(&tracker[0], renderFreq0, 0, 0, 0xf, 0, 0);
    region_string(&tracker[1], renderFreq1, 0, 0, 0xf, 0, 0);
    region_string(&tracker[2], renderFreq2, 0, 0, 0xf, 0, 0);
    region_string(&tracker[3], renderFreq3, 0, 0, 0xf, 0, 0);
}

void render_freq(void) {
    region_string(&tracker[0], renderFreq0, 0, 0, 0xf, 0, 0);
    region_string(&tracker[1], renderFreq1, 0, 0, 0xf, 0, 0);
    region_string(&tracker[2], renderFreq2, 0, 0, 0xf, 0, 0);
    region_string(&tracker[3], renderFreq3, 0, 0, 0xf, 0, 0);    
}
