//prgm
//aleph-avr32

//render.c
//render prgm screen

//--- std headers
#include <string.h>

//--- asf headers
#include "print_funcs.h"

//avr32-lib
#include "app.h"
#include "memory.h"
#include "screen.h"

//prgm
#include "render.h"
#include "util.h"


//BOOT
static scroll bootScroll;

static region bootScrollRegion = {.w = 128, .h = 64, .x = 0, .y = 0};

void render_boot(char *str) {
    scroll_string_front(&bootScroll, str);  
}


//PRGM
//external variables
region *headRegion = NULL;


//static variables
static region headRegion_prgm = {.w=128, .h=8, .x = 0, .y = 0};


//external functions
//initialization, called by app_init()
void render_init(void) {
    //BOOT
    region_alloc((region*)(&bootScrollRegion)); //declared in memory.h
    scroll_init(&bootScroll, &bootScrollRegion);
    
    //PRGM
    region_alloc((region*)(&headRegion_prgm));
}


//flashscreen, called by app_launch()
void render_startup(void) {
    screen_clear();
    region_fill(&headRegion_prgm, 0);
    region_string(&headRegion_prgm, "PRGM beta by Test2", 0, 0, 0xf, 0, 0); //region, string, offset x, offset y, color a, color b, size
}


void render_update(void) {
  app_pause();

  region_draw(&headRegion_prgm);

  app_resume();
}
