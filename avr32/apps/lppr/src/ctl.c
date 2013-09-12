// std
#include <string.h>
// asf
#include "delay.h"
#include "print_funcs.h"
// aleph-avr32
#include "app.h"
#include "bfin.h"
#include "control.h"
#include "interrupts.h"
// common
#include "fix.h"
#include "param_common.h"
// lppr
#include "ctl.h"
#include "render.h"
#include "util.h"

//----------------------------------------
//---- static variables

// milliseconds uptime of last event
static u32 ms_loop1 = 0;
// is a loop playing
static u8 loopPlay1 = 0;
// is a loop recording
static u8 loopRec1 = 0;

//-- parameter values
// inputs. use s32 type but unsigned range (accumulators)
s32 in_fb0 = 0;
s32 in_fb1 = 0;
s32 in_cutoff0 = 0;
s32 in_cutoff1 = 0;
s32 in_res0 = 0;
s32 in_res1 = 0;

/// help!
static inline s32 fr32_from_float(float x) {
  if(x > 0.f) {
    return (s32)((double)x * (double)0x7fffffff);
  } else {
    return (s32)((double)(x * -1.f) * (double)0x80000000);
  }
}


//-------------------------------------
//------ extern functions

// get parameter report from DSP
u8 ctl_report_params(void) {
  volatile char buf[64];
  volatile ParamDesc pdesc;
  u32 numParams;
  u8 i;
 
  bfin_get_num_params(&numParams);
  print_dbg("\r\nnumparams: ");
  print_dbg_ulong(numParams);

  if(numParams == 255) {
    print_dbg("\r\n report_params fail (too many)");
    return 0;
  }

  if(numParams > 0) {
    for(i=0; i<numParams; i++) {
      bfin_get_param_desc(i, &pdesc);
      
      print_dbg("\r\n got pdesc : ");
      print_dbg((const char* )pdesc.label);
    }
  } else {
    print_dbg("\r\n report_params fail (none)");
    return 0;
  }
  
  delay_ms(100);

  print_dbg("\r\n checking module label ");
  // check module label
  bfin_get_module_name(buf);

  delay_ms(10);

  print_dbg("\r\n bfin module name: ");
  print_dbg((const char*)buf);
  if(strcmp((const char*)buf, "aleph-lines")) {
    print_dbg( "\r\n report_params fail (module name mismatch)" );
    return 0;
  } else {
    return 1;
  }
}

// set initial parameters
void ctl_init_params(void) {
  // no filters
  ctl_param_change(eParam_mix0, 0);
  ctl_param_change(eParam_mix1, 0);
  // half dry
  ctl_param_change(eParam_adc0_dac0, fr32_from_float(0.5) );
  // half wet
  ctl_param_change(eParam_del0_dac0, fr32_from_float(0.5) );
  ctl_param_change(eParam_del1_dac0, fr32_from_float(0.5) );
  // adc0 -> del0
  ctl_param_change(eParam_adc0_del0, fr32_from_float(0.99));
  // adc0 -> del1
  ctl_param_change(eParam_adc0_del1, fr32_from_float(0.99));
  // del0 -> del1
  ctl_param_change(eParam_del0_del1, fr32_from_float(0.99));				    
  // slight feedback on del0 
  ctl_param_change(eParam_del0_del0, fix16_one >> 2);
  // set write flags
  ctl_param_change(eParam_write0, 1);
  ctl_param_change(eParam_write1, 1);		   
  // set run flags
  ctl_param_change(eParam_run_write0, 1);
  ctl_param_change(eParam_run_write1, 1);		   
  ctl_param_change(eParam_run_read0, 1);
  ctl_param_change(eParam_run_read1, 1);		   
  // set delay time
  ctl_param_change(eParam_delay0, 250 * 48 );
  ctl_param_change(eParam_delay1, 500 * 48 );
}


// set delay time in ms
void  ctl_set_delay_ms(u8 idx, u32 ms)  {
  u32 samps =  MS_TO_SAMPS(ms);
  print_dbg("\r\n\r\n ctl_set_delay_ms:");
  while(samps > PARAM_BUFFER_MAX) {
    samps -= PARAM_BUFFER_MAX;
  }
  // bleh
  switch(idx) {
  case 0:
    ctl_param_change(eParam_delay0, samps);
    render_delay_time(0, ms, samps);
    break;
  case 1:    
    if(loopPlay1) {
      print_dbg("\r\n (switching from loop to delay mode");
      print_dbg("\r\n set loop time to max");
      ctl_param_change(eParam_loop1, PARAM_BUFFER_MAX);
      print_dbg("\r\n write enable");
      ctl_param_change(eParam_write1, 1);
      print_dbg("\r\n start write head movement");
      ctl_param_change(eParam_run_write1, 1);
      print_dbg("\r\n no overdub");
      ctl_param_change(eParam_pre1, 0);
      loopPlay1 = 0;
    }
    print_dbg("\r\n sync write/read heads");
    ctl_param_change(eParam_delay1, samps);    
    render_delay_time(1, ms, samps);

    break;
  default:
    break;
  }
}


// start recording loop on given delayline
void ctl_loop_record(u8 idx) {
  print_dbg("\r\n\r\n ctl_loop_record:");
  if(!loopRec1) {
    if(loopPlay1) {
      print_dbg("\r\n (existing loop)");
      print_dbg("\r\n start writing ");
      ctl_param_change(eParam_write1, 1);
      print_dbg("\r\n full overdub");
      ctl_param_change(eParam_pre1, fix16_one);
      // can reset loop length
      // by recording, cancelling loop, playing
      ms_loop1 = tcTicks; 
    } else {
      print_dbg("\r\n (new loop)");
      ms_loop1 = tcTicks;
      print_dbg("\r\n stop read head movement");
      ctl_param_change(eParam_run_read1, 0);
      print_dbg("\r\n reset write head");
      ctl_param_change(eParam_pos_write1, 0);
      print_dbg("\r\n write enable");
      ctl_param_change(eParam_write1, 1);
      print_dbg("\r\n start write head movement");
      ctl_param_change(eParam_run_write1, 1);
    }
    loopRec1 = 1;
  }
}

// stop recording loop / start playback on given delayline
void ctl_loop_playback(u8 idx) {
  u32 samps;
  u32 ms;
  print_dbg("\r\n\r\n ctl_loop_playback:");

  if(loopRec1) {
    // recording
    if(loopPlay1) {

      // already playing
      print_dbg("\r\n (existing loop)");
      print_dbg("\r\n write disable");
      ctl_param_change(eParam_write1, 0);
    } else {

      // not playing
      print_dbg("\r\n (new loop)"); 
      if (ms_loop1 > tcTicks) { // overflow
	ms = tcTicks + (0xffffffff - ms_loop1);
      } else {
	ms = tcTicks - ms_loop1;
      }
      samps = MS_TO_SAMPS(ms) - 1;

      print_dbg("\r\n write disable");
      ctl_param_change(eParam_write1, 0);
      print_dbg("\r\n reset write head");
      ctl_param_change(eParam_pos_write1, 1);
      print_dbg("\r\n reset read head");
      ctl_param_change(eParam_pos_read1, 0);
      print_dbg("\r\n set loop time");
      ctl_param_change(eParam_loop1, samps);
      print_dbg("\r\n start read head");
      ctl_param_change(eParam_run_read1, 1);

      // set loop-playing flag
      loopPlay1 = 1;
    }
    // unset loop-recording
    loopRec1 = 0;  
  } else {
    // not recording
    if (loopPlay1) {
      // but a loop is playing
      // reset the loop 
	print_dbg("\r\n reset read head");
	ctl_param_change(eParam_pos_read1, 0);
    } else {
      ;; // no action
    }
  }
}
