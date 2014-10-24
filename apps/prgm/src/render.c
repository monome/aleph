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
#include "page_level.h"
#include "page_env.h"

#include "render.h" //includes region.h
//#include "handler.h"
#include "util.h"
#include "scale.h"

//regions
static region bootScrollRegion = {.w = 128, .h = 64, .x = 0, .y = 0};

static scroll bootScroll;

static const u8 numRegions_lvl = 6;
static const u8 numRegions_env = 4;
//static const u8 numRegions_pat = 4;

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
/*
static region pattern[4] = {
    {.w=64, .h=32, .x = 0, .y = 0},
    {.w=64, .h=32, .x = 64, .y = 0},
    {.w=64, .h=32, .x = 0, .y = 32},
    {.w=64, .h=32, .x = 64, .y = 32}
};

static region *patternRegions[] = {
    &(pattern[0]),
    &(pattern[1]),
    &(pattern[2]),
    &(pattern[3]),
};
*/
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
/*
    for(i = 0; i<numRegions_pat; i++) {
        region_alloc((region*)(patternRegions[i]));
    }
*/
    tracker_init();
}

void render_boot(const char* str) {
    scroll_string_front(&bootScroll, (char*)str);
}

void render_startup (void) {
    screen_clear();
    set_page(ePageLevel);
    
    Transposed0tmp = Transposed1tmp = Transposed2tmp = Transposed3tmp = TRANSPOSED_VAL_INIT;
    Transposed0 = transpose_lookup(Transposed0tmp) * 0x00010000;
    Transposed1 = transpose_lookup(Transposed1tmp) * 0x00010000;
    Transposed2 = transpose_lookup(Transposed2tmp) * 0x00010000;
    Transposed3 = transpose_lookup(Transposed3tmp) * 0x00010000;

    Free0 = Free1 = Free2 = Free3 = INIT_F;
        
    print_fix16(renderFree0, fix16_mul(Free0, Transposed0));
    print_fix16(renderFree1, fix16_mul(Free1, Transposed1));
    print_fix16(renderFree2, fix16_mul(Free2, Transposed2));
    print_fix16(renderFree3, fix16_mul(Free3, Transposed3));

    print_fix16(renderTransposed0, Transposed0);
    print_fix16(renderTransposed1, Transposed1);
    print_fix16(renderTransposed2, Transposed2);
    print_fix16(renderTransposed3, Transposed3);

    print_fix16(renderCounter, 1 * 0x00010000);
    
    render_level();
    
    Time0 = Time1 = Time2 = Time3 = 0;
    print_fix16(renderTime0, Time0);
    print_fix16(renderTime1, Time1);
    print_fix16(renderTime2, Time2);
    print_fix16(renderTime3, Time3);

    Curve0 = Curve1 = Curve2 = Curve3 = 0;
    print_fix16(renderCurve0, Curve0);
    print_fix16(renderCurve1, Curve1);
    print_fix16(renderCurve2, Curve2);
    print_fix16(renderCurve3, Curve3);
    
    Destination0 = Destination1 = Destination2 = Destination3 = 0;
    print_fix16(renderDest0, Destination0);
    print_fix16(renderDest1, Destination1);
    print_fix16(renderDest2, Destination2);
    print_fix16(renderDest3, Destination3);
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
/*
    for(i = 0; i<numRegions_pat; i++) {
        r = patternRegions[i];
        if(r->dirty) {
            screen_draw_region(r->x, r->y, r->w, r->h, r->data);
            r->dirty = 0;
        }
    }
*/
    app_resume();
}

//draw level page
void render_level(void) {
    u8 i;
    for(i = 0; i<numRegions_lvl; i++) {
        region_fill(&level[i], 0x0);
    }
    /* region, string, offset x, offset y, color a, color b, size */
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
    
    region_string(&env[0], renderFree0, 0, 24, 0x7, 0, 0);
    region_string(&env[1], renderFree1, 0, 24, 0x7, 0, 0);
    region_string(&env[2], renderFree2, 0, 24, 0x7, 0, 0);
    region_string(&env[3], renderFree3, 0, 24, 0x7, 0, 0);
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
