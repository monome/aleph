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

static const u8 numRegions_lvl = 5;
static const u8 numRegions_env = 5;

static region level[5] = {
    {.w=64, .h=24, .x = 0, .y = 0},
    {.w=64, .h=24, .x = 64, .y = 0},
    {.w=64, .h=24, .x = 0, .y = 24},
    {.w=64, .h=24, .x = 64, .y = 24},
    {.w=128, .h=16, .x = 0, .y = 48},
};

static region *levelRegions[] = {
    &(level[0]),
    &(level[1]),
    &(level[2]),
    &(level[3]),
    &(level[4]),
};

static region env[5] = {
    {.w=64, .h=24, .x = 0, .y = 0},
    {.w=64, .h=24, .x = 64, .y = 0},
    {.w=64, .h=24, .x = 0, .y = 24},
    {.w=64, .h=24, .x = 64, .y = 24},
    {.w=128, .h=16, .x = 0, .y = 48},
};

static region *envRegions[] = {
    &(env[0]),
    &(env[1]),
    &(env[2]),
    &(env[3]),
    &(env[4]),
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

void render_startup (void) {
    screen_clear();
    set_page(ePageLevel);
    
    print_fix16(renderL0, 0);
    print_fix16(renderL1, 0);
    print_fix16(renderL2, 0);
    print_fix16(renderL3, 0);
    
    print_fix16(renderP0, 0);
    print_fix16(renderP1, 0);
    print_fix16(renderP2, 0);
    print_fix16(renderP3, 0);
    
    print_fix16(renderF0, 0);
    print_fix16(renderF1, 0);
    print_fix16(renderF2, 0);
    print_fix16(renderF3, 0);

    print_fix16(renderCounter, 1 * 0x00010000);
    
    render_level();
    
    print_fix16(renderTrig0, 0);
    print_fix16(renderTrig1, 0);
    print_fix16(renderTrig2, 0);
    print_fix16(renderTrig3, 0);
    
    print_fix16(renderTime0, 0);
    print_fix16(renderTime1, 0);
    print_fix16(renderTime2, 0);
    print_fix16(renderTime3, 0);
}

void render_boot(const char* str) {
    scroll_string_front(&bootScroll, (char*)str);
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
    region_string(&level[0], renderL0, 0, 0, 0xf, 0, 0);
    region_string(&level[1], renderL1, 0, 0, 0xf, 0, 0);
    region_string(&level[2], renderL2, 0, 0, 0xf, 0, 0);
    region_string(&level[3], renderL3, 0, 0, 0xf, 0, 0);
    
    region_string(&level[0], renderP0, 0, 8, 0xf, 0, 0);
    region_string(&level[1], renderP1, 0, 8, 0xf, 0, 0);
    region_string(&level[2], renderP2, 0, 8, 0xf, 0, 0);
    region_string(&level[3], renderP3, 0, 8, 0xf, 0, 0);
    
    region_string(&level[0], renderF0, 0, 16, 0xf, 0, 0);
    region_string(&level[1], renderF1, 0, 16, 0xf, 0, 0);
    region_string(&level[2], renderF2, 0, 16, 0xf, 0, 0);
    region_string(&level[3], renderF3, 0, 16, 0xf, 0, 0);

    region_string(&level[4], renderCounter, 0, 0, 0xf, 0, 0);
}

void render_row1(void) {
    region_string(&level[0], renderL0, 0, 0, 0xf, 0, 0);
    region_string(&level[1], renderL1, 0, 0, 0xf, 0, 0);
    region_string(&level[2], renderL2, 0, 0, 0xf, 0, 0);
    region_string(&level[3], renderL3, 0, 0, 0xf, 0, 0);
}

void render_row2(void) {
    region_string(&level[0], renderP0, 0, 8, 0xf, 0, 0);
    region_string(&level[1], renderP1, 0, 8, 0xf, 0, 0);
    region_string(&level[2], renderP2, 0, 8, 0xf, 0, 0);
    region_string(&level[3], renderP3, 0, 8, 0xf, 0, 0);
}

void render_row3(void) {
    region_string(&level[0], renderF0, 0, 16, 0xf, 0, 0);
    region_string(&level[1], renderF1, 0, 16, 0xf, 0, 0);
    region_string(&level[2], renderF2, 0, 16, 0xf, 0, 0);
    region_string(&level[3], renderF3, 0, 16, 0xf, 0, 0);
}

void render_countlev(void) {
    region_string(&level[4], renderCounter, 0, 0, 0xf, 0, 0);
}

//draw env page
void render_env(void) {
    u8 i;
    for(i = 0; i<numRegions_env; i++) {
        region_fill(&env[i], 0x0);
    }
    render_modes(counter);
    
    region_string(&env[0], renderTrig0, 0, 0, 0xf, 0, 0);
    region_string(&env[1], renderTrig1, 0, 0, 0xf, 0, 0);
    region_string(&env[2], renderTrig2, 0, 0, 0xf, 0, 0);
    region_string(&env[3], renderTrig3, 0, 0, 0xf, 0, 0);
    
    region_string(&env[0], renderTime0, 0, 16, 0xf, 0, 0);
    region_string(&env[1], renderTime1, 0, 16, 0xf, 0, 0);
    region_string(&env[2], renderTime2, 0, 16, 0xf, 0, 0);
    region_string(&env[3], renderTime3, 0, 16, 0xf, 0, 0);
    
    region_string(&env[4], renderCounter, 0, 0, 0xf, 0, 0);
}

void render_trig(void) {
    region_string(&env[0], renderTrig0, 0, 0, 0xf, 0, 0);
    region_string(&env[1], renderTrig1, 0, 0, 0xf, 0, 0);
    region_string(&env[2], renderTrig2, 0, 0, 0xf, 0, 0);
    region_string(&env[3], renderTrig3, 0, 0, 0xf, 0, 0);
}

void render_modes(u8 i) {
    render_mode(0, track[0]->modename[i]);
    render_mode(1, track[1]->modename[i]);
    render_mode(2, track[2]->modename[i]);
    render_mode(3, track[3]->modename[i]);
}

//  MAX 8 char in a name for now..
void render_mode(u8 chan, s32 name) {
    static char *nameptr[] = {
        "OFF",      //0
        "HOLD",     //1
        "TRIG",     //2
        "GATE",     //3
        "NOISE",    //4
        "one",      //5
        "loop",     //6
        "scrub",    //7
        "noise",    //8
        "recin1",   //9
        "TGrecin2"  //10
    };
    
    region_string(&env[chan], "████████", 0, 8, 0, 0, 0);
    region_string(&env[chan], nameptr[name], 0, 8, 0xf, 0, 0);
}

void render_time(void) {
    region_string(&env[0], renderTime0, 0, 16, 0xf, 0, 0);
    region_string(&env[1], renderTime1, 0, 16, 0xf, 0, 0);
    region_string(&env[2], renderTime2, 0, 16, 0xf, 0, 0);
    region_string(&env[3], renderTime3, 0, 16, 0xf, 0, 0);
}

void render_countenv(void) {
    region_string(&env[4], renderCounter, 0, 0, 0xf, 0, 0);
}
