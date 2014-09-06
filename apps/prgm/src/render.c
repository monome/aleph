//render.c
//aleph-prgm-avr32

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
#include "control.h"

//prgm
#include "pages.h"
#include "render.h" //includes region.h
#include "util.h"
#include "prgm.h"
#include "tracker.h"
#include "scale.h"

//regions
static region bootScrollRegion = {.w = 128, .h = 64, .x = 0, .y = 0};

static scroll bootScroll;

static const u8 numRegions = 4;
static const u8 numRegions_t = 6;

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

static region tracker[6] = {
    {.w=64, .h=24, .x = 0, .y = 0},
    {.w=64, .h=24, .x = 64, .y = 0},
    {.w=64, .h=24, .x = 0, .y = 24},
    {.w=64, .h=24, .x = 64, .y = 24},
    {.w=64, .h=8, .x = 0, .y = 48},
    {.w=64, .h=8, .x = 64, .y = 48}
};

static region *trackerRegions[] = {
    &(tracker[0]),
    &(tracker[1]),
    &(tracker[2]),
    &(tracker[3]),
    &(tracker[4]),
    &(tracker[5]),
};

//initialization, called by app_init()
void render_init(void) {
    region_alloc((region*)(&bootScrollRegion)); //BOOT: declared in region.h, calls mem_alloc() declared in memory.h
    scroll_init(&bootScroll, &bootScrollRegion); //BOOT

    u32 i;
    for(i = 0; i<numRegions; i++) {
        region_alloc((region*)(prgmRegions[i]));
    }    

    for(i = 0; i<numRegions_t; i++) {
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
    
    Transpose0tmp = Transpose1tmp = Transpose2tmp = Transpose3tmp = RATIO_VAL_INIT;
    Transpose0 = transpose_lookup(Transpose0tmp) * 0x00010000;
    Transpose1 = transpose_lookup(Transpose1tmp) * 0x00010000;
    Transpose2 = transpose_lookup(Transpose2tmp) * 0x00010000;
    Transpose3 = transpose_lookup(Transpose3tmp) * 0x00010000;

    Freq0tmp = Freq1tmp = Freq2tmp = Freq3tmp = FREQ_VAL_INIT;
    Freq0 = note_lookup(Freq0tmp) * 0x00010000;
    Freq1 = note_lookup(Freq1tmp) * 0x00010000;
    Freq2 = note_lookup(Freq2tmp) * 0x00010000;
    Freq3 = note_lookup(Freq3tmp) * 0x00010000;
        
    print_fix16(renderFreq0, fix16_mul(Freq0, Transpose0));
    print_fix16(renderFreq1, fix16_mul(Freq1, Transpose1));
    print_fix16(renderFreq2, fix16_mul(Freq2, Transpose2));
    print_fix16(renderFreq3, fix16_mul(Freq3, Transpose3));

    print_fix16(renderTranspose0, Transpose0);
    print_fix16(renderTranspose1, Transpose1);
    print_fix16(renderTranspose2, Transpose2);
    print_fix16(renderTranspose3, Transpose3);

    print_fix16(renderCounter, 1 * 0x00010000);
    print_fix16(renderCounter_t, 1 * 0x00010000);
    
    render_tracker();
    
    Wave0 = Wave1 = Wave2 = Wave3 = 0;
    print_fix16(renderWave0, Wave0);
    print_fix16(renderWave1, Wave1);
    print_fix16(renderWave2, Wave2);
    print_fix16(renderWave3, Wave3);
    
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
    
    for(i = 0; i<numRegions_t; i++) {
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
    region_string(&prgm[0], renderWave0, 0, 0, 0xf, 0, 0);
    region_string(&prgm[1], renderWave1, 0, 0, 0xf, 0, 0);
    region_string(&prgm[2], renderWave2, 0, 0, 0xf, 0, 0);
    region_string(&prgm[3], renderWave3, 0, 0, 0xf, 0, 0);

    region_string(&prgm[0], renderPhase0, 0, 8, 0xf, 0, 0);
    region_string(&prgm[1], renderPhase1, 0, 8, 0xf, 0, 0);
    region_string(&prgm[2], renderPhase2, 0, 8, 0xf, 0, 0);
    region_string(&prgm[3], renderPhase3, 0, 8, 0xf, 0, 0);
    
    region_string(&prgm[0], renderBlend0, 0, 16, 0xf, 0, 0);
    region_string(&prgm[1], renderBlend1, 0, 16, 0xf, 0, 0);
    region_string(&prgm[2], renderBlend2, 0, 16, 0xf, 0, 0);
    region_string(&prgm[3], renderBlend3, 0, 16, 0xf, 0, 0);    
}

void render_wave(void) {
    region_string(&prgm[0], renderWave0, 0, 0, 0xf, 0, 0);
    region_string(&prgm[1], renderWave1, 0, 0, 0xf, 0, 0);
    region_string(&prgm[2], renderWave2, 0, 0, 0xf, 0, 0);
    region_string(&prgm[3], renderWave3, 0, 0, 0xf, 0, 0);
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
    for(i = 0; i<numRegions_t; i++) {
        region_fill(&tracker[i], 0x0);
    }

    region_string(&tracker[0], renderFreq0, 0, 0, 0xf, 0, 0);
    region_string(&tracker[1], renderFreq1, 0, 0, 0xf, 0, 0);
    region_string(&tracker[2], renderFreq2, 0, 0, 0xf, 0, 0);
    region_string(&tracker[3], renderFreq3, 0, 0, 0xf, 0, 0);
    
    region_string(&tracker[0], renderTranspose0, 0, 8, 0xf, 0, 0);
    region_string(&tracker[1], renderTranspose1, 0, 8, 0xf, 0, 0);
    region_string(&tracker[2], renderTranspose2, 0, 8, 0xf, 0, 0);
    region_string(&tracker[3], renderTranspose3, 0, 8, 0xf, 0, 0);

    region_string(&tracker[4], renderCounter, 0, 0, 0xf, 0, 0);
    region_string(&tracker[5], renderCounter_t, 0, 0, 0xf, 0, 0);
}

void render_freq(void) {
    region_string(&tracker[0], renderFreq0, 0, 0, 0xf, 0, 0);
    region_string(&tracker[1], renderFreq1, 0, 0, 0xf, 0, 0);
    region_string(&tracker[2], renderFreq2, 0, 0, 0xf, 0, 0);
    region_string(&tracker[3], renderFreq3, 0, 0, 0xf, 0, 0);    
}

void render_transpose(void) {
    region_string(&tracker[0], renderFreq0, 0, 0, 0xf, 0, 0);
    region_string(&tracker[1], renderFreq1, 0, 0, 0xf, 0, 0);
    region_string(&tracker[2], renderFreq2, 0, 0, 0xf, 0, 0);
    region_string(&tracker[3], renderFreq3, 0, 0, 0xf, 0, 0);
    
    region_string(&tracker[0], renderTranspose0, 0, 8, 0xf, 0, 0);
    region_string(&tracker[1], renderTranspose1, 0, 8, 0xf, 0, 0);
    region_string(&tracker[2], renderTranspose2, 0, 8, 0xf, 0, 0);
    region_string(&tracker[3], renderTranspose3, 0, 8, 0xf, 0, 0);
}

void render_counters(void) {
    region_string(&tracker[4], renderCounter, 0, 0, 0xf, 0, 0);
    region_string(&tracker[5], renderCounter_t, 0, 0, 0xf, 0, 0);
}
