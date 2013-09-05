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
#include "param_common.h"

// lppr
#include "params.h"
#include "util.h"

//static u32 ms_loop0 = 0;
static u32 ms_loop1 = 0;

u8 report_params(void) {
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
void set_initial_params(void) {
  
  // no filters
  ctl_param_change(eParam_mix0, 0);
  ctl_param_change(eParam_mix1, 0);
  
  // half dry
  ctl_param_change(eParam_adc0_dac0, fix16_from_float(0.5) );
  //  ctl_param_change(eParam_adc1_dac1, fix16_from_float(0.5) );
  
  // half wet
  ctl_param_change(eParam_del0_dac0, fix16_from_float(0.5) );
  ctl_param_change(eParam_del1_dac0, fix16_from_float(0.5) );
  //  ctl_param_change(eParam_del1_dac1, fix16_from_float(0.5) );
				    
  // adc0 -> del0
  ctl_param_change(eParam_adc0_del0, fix16_from_float(0.99));
  // adc0 -> del1
  ctl_param_change(eParam_adc0_del1, fix16_from_float(0.99));
  // del0 -> del1
  ctl_param_change(eParam_del0_del1, fix16_from_float(0.99));
				    
  // slight feedback on del0 
  ctl_param_change(eParam_del0_del0, fix16_one >> 2);
  //  ctl_param_change(eParam_del1_del1, fix16_one >> 3);

  // set write flags
  ctl_param_change(eParam_write0, fix16_from_float(0.99));
  ctl_param_change(eParam_write1, fix16_from_float(0.99));		   

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
  while(samps > PARAM_BUFFER_MAX) {
    samps -= PARAM_BUFFER_MAX;
  }
  // bleh
  switch(idx) {
  case 0:
    ctl_param_change(eParam_delay0, samps);
    break;
  case 1:
    ctl_param_change(eParam_delay1, samps);
    break;
  default:
    break;
  }
}


// start recording loop on given delayline
void ctl_loop_record(u8 idx) {
  switch(idx) {
  case 0:
    ms_loop0 = tcTicks;
    ctl_param_change(eParam_run_read0, 0);
    ctl_param_change(eParam_pos_write0, 0);
    ctl_param_change(eParam_run_write0, 1);
    break;
  case 1:
    ms_loop1 = tcTicks;
    ctl_param_change(eParam_run_read1, 0);
    ctl_param_change(eParam_pos_write1, 0);
    ctl_param_change(eParam_run_write1, 1);
    break;
  }

}

// stop recording loop / start playback on given delayline
void ctl_loop_playback(u8 idx) {
  u32 samps;
  u32 ms;
  
  switch(idx) {
  case 0:
    if (ms_loop0 > tcTicks) { // overflow
      ms = tcTicks + (0xffffffff - ms_loop0);
    } else {
      ms = tcTicks - ms_loop0;
    }
    samps = MS_TO_SAMPS(ms);
    ctl_param_change(eParam_run_write0, 0);
    ctl_param_change(eParam_pos_read0, 0);
    ctl_param_change(eParam_loop0, samps);
    ctl_param_change(eParam_run_read0, 1);
  break;

  case 1:
    if (ms_loop1 > tcTicks) { // overflow
      ms = tcTicks + (0xffffffff - ms_loop1);
    } else {
      ms = tcTicks - ms_loop1;
    }
    samps = MS_TO_SAMPS(ms) - 1;
    ctl_param_change(eParam_run_write1, 0);
    ctl_param_change(eParam_pos_read1, 0);
    ctl_param_change(eParam_loop1, samps);
    ctl_param_change(eParam_run_read1, 1);
  }
}
