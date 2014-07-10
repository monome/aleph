//prgm
//aleph-avr32

//asf
#include "print_funcs.h"

// avr32
#include "bfin.h"
#include "control.h"

#include "app.h"
#include "types.h"

//prgm
#include "handler.h"
#include "render.h"
#include "ctl.h"

static void handle_Adc0(s32 data) { 
//    op_adc_sys_input(opSysAdc, 0, data);
}

static void handle_Adc1(s32 data) { 
//    op_adc_sys_input(opSysAdc, 1, data);
}

static void handle_Adc2(s32 data) { 
//    op_adc_sys_input(opSysAdc, 2, data);
}

static void handle_Adc3(s32 data) { 
//    op_adc_sys_input(opSysAdc, 3, data);
}

static void handle_Switch6(s32 data) {
    // footswitch 1
    //op_sw_sys_input(opSysSw[4], data > 0);
}

static void handle_Switch7(s32 data) { 
    // footswitch 2
    //op_sw_sys_input(opSysSw[5], data > 0);
} 

//static functions
/*
static void handle_encoder_0(s32 val);

static void ctrl_parameter(u32 pid, fract32 val);

void handle_encoder_0(s32 val) {
    print_dbg("\r\n encoder moving...");
    print_dbg_ulong(val);
    static s32 Freq;
    Freq += val * 0x00010000;
    ctrl_parameter(eParamFreq0, (u32)Freq);
}

void ctrl_parameter(u32 pid, fract32 val) {
    print_dbg("\r\n parameter id...");
    print_dbg_ulong(pid);
    print_dbg("\r\n parameter value...");
    print_dbg_ulong(val);
    ctl_param_change(pid, val); //defined in control.h
    
}
*/

//external functions
void assign_prgm_event_handlers(void) {
    
//    app_event_handlers[ kEventAppCustom ]	= &net_poll_handler ;
    app_event_handlers[ kEventAdc0 ] = &handle_Adc0 ;
    app_event_handlers[ kEventAdc1 ] = &handle_Adc1 ;
    app_event_handlers[ kEventAdc2 ] = &handle_Adc2 ;
    app_event_handlers[ kEventAdc3 ] = &handle_Adc3 ;

//    app_event_handlers[ kEventEncoder0 ] = &handle_encoder_0 ;
//    app_event_handlers[ kEventEncoder1 ] = &handler_Encoder1 ;
//    app_event_handlers[ kEventEncoder2 ] = &handler_Encoder2 ;
//    app_event_handlers[ kEventEncoder3 ] = &handle_encoder_0 ;
//    app_event_handlers[ kEventSwitch0 ]	= &handler_Switch0 ;
//    app_event_handlers[ kEventSwitch1 ]	= &handler_Switch1 ;
//    app_event_handlers[ kEventSwitch2 ]	= &handler_Switch2 ;
//    app_event_handlers[ kEventSwitch3 ]	= &handler_Switch3 ;
//    app_event_handlers[ kEventSwitch4 ]	= &handler_Switch4 ; //mode switch
//    app_event_handlers[ kEventSwitch5 ]	= &handler_Switch5 ; //power switch
    app_event_handlers[ kEventSwitch6 ]	= &handle_Switch6 ; //sequencer step
    app_event_handlers[ kEventSwitch7 ]	= &handle_Switch7 ; //sequencer restart at 1
}
