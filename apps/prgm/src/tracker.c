//
//  prgm.c
//  main
//
//  Created by Staffan Jansson on 7/1/14.
//  Copyright (c) 2014 __MyCompanyName__. All rights reserved.
//

// asf
#include "print_funcs.h"

// common
#include "fix.h"

// avr32
#include "region.h"
#include "app.h"

// bees
#include "handler.h"
#include "pages.h"
#include "render.h"
#include "tracker.h"

// handlers
static void handle_encoder_0(s32 val);

static void handle_key_4(s32 val);

static void handle_encoder_0(s32 val) {
    // nothing
}

static void handle_encoder_1(s32 val) {
    // nothing
}

static void handle_encoder_2(s32 val) {
    // nothing
}

static void handle_encoder_3(s32 val) {
    // nothing
}

static void handle_key_4(s32 val) {
    if(val == 0) { return; }
    
    else {
        set_page(ePagePrgm);
        render_prgm();
    }
}

void select_tracker(void) {
//assign tracker handlers
    app_event_handlers[ kEventEncoder0 ]	= &handle_encoder_3 ;
    app_event_handlers[ kEventEncoder1 ]	= &handle_encoder_2 ;
    app_event_handlers[ kEventEncoder2 ]	= &handle_encoder_1 ;
    app_event_handlers[ kEventEncoder3 ]	= &handle_encoder_0 ;
    //    app_event_handlers[ kEventSwitch0 ]	= &handle_key_0 ;
    //    app_event_handlers[ kEventSwitch1 ]	= &handle_key_1 ;
    //    app_event_handlers[ kEventSwitch2 ]	= &handle_key_2 ;
    //    app_event_handlers[ kEventSwitch3 ]	= &handle_key_3 ;
    app_event_handlers[ kEventSwitch4 ]	= &handle_key_4 ;
    
}
