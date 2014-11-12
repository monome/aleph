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
#include "tracker.h"
#include "pages.h"
#include "page_level.h"
#include "page_env.h"

#include "render.h" //includes region.h
#include "util.h"
#include "scale.h"

//regions
static region bootScrollRegion = {.w = 128, .h = 64, .x = 0, .y = 0};

static scroll bootScroll;

static const u8 numRegions_lvl = 6;
static const u8 numRegions_env = 4;

static region level[6] = {
    {.w=64, .h=24, .x = 0, .y = 0},
    {.w=64, .h=24, .x = 64, .y = 0},
    {.w=64, .h=24, .x = 0, .y = 24},
    {.w=64, .h=24, .x = 64, .y = 24},
    {.w=64, .h=16, .x = 0, .y = 48},
    {.w=64, .h=16, .x = 64, .y = 48}
};

static region *levelRegions[] = {
    &(level[0]),
    &(level[1]),
    &(level[2]),
    &(level[3]),
    &(level[4]),
    &(level[5]),
};

static region env[4] = {
    {.w=64, .h=32, .x = 0, .y = 0},
    {.w=64, .h=32, .x = 64, .y = 0},
    {.w=64, .h=32, .x = 0, .y = 32},
    {.w=64, .h=32, .x = 64, .y = 32}
};

static region *envRegions[] = {
    &(env[0]),
    &(env[1]),
    &(env[2]),
    &(env[3]),
};

//initialization, called by app_init()
void render_init(void) {
    region_alloc((region*)(&bootScrollRegion)); //BOOT: declared in region.h, calls mem_alloc() declared in memory.h
    scroll_init(&bootScroll, &bootScrollRegion); //BOOT

    u32 i;
    for(i = 0; i<numRegions_lvl; i++) {
        region_alloc((region*)(levelRegions[i]));
    }
    for(i = 0; i<numRegions_env; i++) {
        region_alloc((region*)(envRegions[i]));
    }
}

void render_boot(const char* str) {
    scroll_string_front(&bootScroll, (char*)str);
}

void render_startup (void) {
    screen_clear();
    set_page(ePageLevel);
    
    print_fix16(renderFree0, 0);
    print_fix16(renderFree1, 0);
    print_fix16(renderFree2, 0);
    print_fix16(renderFree3, 0);

    print_fix16(renderTransposed0, 0);
    print_fix16(renderTransposed1, 0);
    print_fix16(renderTransposed2, 0);
    print_fix16(renderTransposed3, 0);

    print_fix16(renderCounter, 1 * 0x00010000);
    
    render_level();
    
    print_fix16(renderTime0, 0);
    print_fix16(renderTime1, 0);
    print_fix16(renderTime2, 0);
    print_fix16(renderTime3, 0);

    print_fix16(renderCurve0, 0);
    print_fix16(renderCurve1, 0);
    print_fix16(renderCurve2, 0);
    print_fix16(renderCurve3, 0);
    
    print_fix16(renderDest0, 0);
    print_fix16(renderDest1, 0);
    print_fix16(renderDest2, 0);
    print_fix16(renderDest3, 0);

    print_fix16(renderTrig0, 0);
    print_fix16(renderTrig1, 0);
    print_fix16(renderTrig2, 0);
    print_fix16(renderTrig3, 0);
}

void render_update(void) {
    region *r;
    u8 i;
    
    app_pause();
    
    if(bootScroll.reg->dirty) {
        scroll_draw(&bootScroll);
    }
    
    for(i = 0; i<numRegions_lvl; i++) {
        r = levelRegions[i];
        if(r->dirty) {
            screen_draw_region(r->x, r->y, r->w, r->h, r->data);
            r->dirty = 0;
        }
    }
    
    for(i = 0; i<numRegions_env; i++) {
        r = envRegions[i];
        if(r->dirty) {
            screen_draw_region(r->x, r->y, r->w, r->h, r->data);
            r->dirty = 0;
        }
    }

    app_resume();
}

//draw level page
void render_level(void) {
    u8 i;
    for(i = 0; i<numRegions_lvl; i++) {
        region_fill(&level[i], 0x0);
    }
    /* region, string, offset x, offset y, color text, color background, size */
    region_string(&level[0], renderFree0, 0, 0, 0xf, 0, 0);
    region_string(&level[1], renderFree1, 0, 0, 0xf, 0, 0);
    region_string(&level[2], renderFree2, 0, 0, 0xf, 0, 0);
    region_string(&level[3], renderFree3, 0, 0, 0xf, 0, 0);
    
    region_string(&level[0], renderTransposed0, 0, 8, 0xf, 0, 0);
    region_string(&level[1], renderTransposed1, 0, 8, 0xf, 0, 0);
    region_string(&level[2], renderTransposed2, 0, 8, 0xf, 0, 0);
    region_string(&level[3], renderTransposed3, 0, 8, 0xf, 0, 0);

    region_string(&level[4], renderCounter, 0, 0, 0xf, 0, 0);
}

void render_free(void) {
    region_string(&level[0], renderFree0, 0, 0, 0xf, 0, 0);
    region_string(&level[1], renderFree1, 0, 0, 0xf, 0, 0);
    region_string(&level[2], renderFree2, 0, 0, 0xf, 0, 0);
    region_string(&level[3], renderFree3, 0, 0, 0xf, 0, 0);
}

void render_transposed(void) {
    region_string(&level[0], renderFree0, 0, 0, 0xf, 0, 0);
    region_string(&level[1], renderFree1, 0, 0, 0xf, 0, 0);
    region_string(&level[2], renderFree2, 0, 0, 0xf, 0, 0);
    region_string(&level[3], renderFree3, 0, 0, 0xf, 0, 0);
    
    region_string(&level[0], renderTransposed0, 0, 8, 0xf, 0, 0);
    region_string(&level[1], renderTransposed1, 0, 8, 0xf, 0, 0);
    region_string(&level[2], renderTransposed2, 0, 8, 0xf, 0, 0);
    region_string(&level[3], renderTransposed3, 0, 8, 0xf, 0, 0);
}

void render_counters(void) {
    region_string(&level[4], renderCounter, 0, 0, 0xf, 0, 0);
}

//draw env page
void render_env(void) {
    u8 i;
    for(i = 0; i<numRegions_env; i++) {
        region_fill(&env[i], 0x0);
    }
    region_string(&env[0], renderTime0, 0, 0, 0xf, 0, 0);
    region_string(&env[1], renderTime1, 0, 0, 0xf, 0, 0);
    region_string(&env[2], renderTime2, 0, 0, 0xf, 0, 0);
    region_string(&env[3], renderTime3, 0, 0, 0xf, 0, 0);
    
    region_string(&env[0], renderCurve0, 0, 8, 0xf, 0, 0);
    region_string(&env[1], renderCurve1, 0, 8, 0xf, 0, 0);
    region_string(&env[2], renderCurve2, 0, 8, 0xf, 0, 0);
    region_string(&env[3], renderCurve3, 0, 8, 0xf, 0, 0);
    
    region_string(&env[0], renderDest0, 0, 16, 0xf, 0, 0);
    region_string(&env[1], renderDest1, 0, 16, 0xf, 0, 0);
    region_string(&env[2], renderDest2, 0, 16, 0xf, 0, 0);
    region_string(&env[3], renderDest3, 0, 16, 0xf, 0, 0);
    
    region_string(&env[0], renderTrig0, 0, 24, 0xf, 0, 0);
    region_string(&env[1], renderTrig1, 0, 24, 0xf, 0, 0);
    region_string(&env[2], renderTrig2, 0, 24, 0xf, 0, 0);
    region_string(&env[3], renderTrig3, 0, 24, 0xf, 0, 0);
}

void render_time(void) {
    region_string(&env[0], renderTime0, 0, 0, 0xf, 0, 0);
    region_string(&env[1], renderTime1, 0, 0, 0xf, 0, 0);
    region_string(&env[2], renderTime2, 0, 0, 0xf, 0, 0);
    region_string(&env[3], renderTime3, 0, 0, 0xf, 0, 0);
}

void render_curve(void) {
    region_string(&env[0], renderCurve0, 0, 8, 0xf, 0, 0);
    region_string(&env[1], renderCurve1, 0, 8, 0xf, 0, 0);
    region_string(&env[2], renderCurve2, 0, 8, 0xf, 0, 0);
    region_string(&env[3], renderCurve3, 0, 8, 0xf, 0, 0);
}

void render_dest(void) {
    region_string(&env[0], renderDest0, 0, 16, 0xf, 0, 0);
    region_string(&env[1], renderDest1, 0, 16, 0xf, 0, 0);
    region_string(&env[2], renderDest2, 0, 16, 0xf, 0, 0);
    region_string(&env[3], renderDest3, 0, 16, 0xf, 0, 0);
}

void render_trig(void) {
    region_string(&env[0], renderTrig0, 0, 24, 0xf, 0, 0);
    region_string(&env[1], renderTrig1, 0, 24, 0xf, 0, 0);
    region_string(&env[2], renderTrig2, 0, 24, 0xf, 0, 0);
    region_string(&env[3], renderTrig3, 0, 24, 0xf, 0, 0);
}
