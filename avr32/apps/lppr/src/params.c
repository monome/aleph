// std
#include <string.h>
// asf
#include "delay.h"
#include "print_funcs.h"
// aleph-avr32

#include "app.h"
#include "bfin.h"
#include "control.h"
#include "param_common.h"

// lppr
#include "params.h"

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
void set_params(void) {
  
  // no filters
  ctl_param_change(eParam_mix0, 0);
  ctl_param_change(eParam_mix1, 0);
  
  // half dry
  ctl_param_change(eParam_adc0_dac0, fix16_from_float(0.5) );
  ctl_param_change(eParam_adc1_dac1, fix16_from_float(0.5) );
  
  // half wet
  ctl_param_change(eParam_del0_dac0, fix16_from_float(0.5) );
  ctl_param_change(eParam_del1_dac1, fix16_from_float(0.5) );
				    
  // full input->del 
  ctl_param_change(eParam_adc0_del0, fix16_from_float(0.99));
  ctl_param_change(eParam_adc1_del1, fix16_from_float(0.99));
				    
  // slight feedback 
  ctl_param_change(eParam_del0_del0, fix16_one >> 2);
  ctl_param_change(eParam_del1_del0, fix16_one >> 2);

  // set write flags
  ctl_param_change(eParam_write0, fix16_from_float(0.99));
  ctl_param_change(eParam_write1, fix16_from_float(0.99));		   

  // set run flags
  ctl_param_change(eParam_run_write0, fix16_from_float(0.99));
  ctl_param_change(eParam_run_write1, fix16_from_float(0.99));		   
  ctl_param_change(eParam_run_read0, fix16_from_float(0.99));
  ctl_param_change(eParam_run_read1, fix16_from_float(0.99));		   

  // set delay time
  ctl_param_change(eParam_delay0, fix16_from_float(1.0));
  ctl_param_change(eParam_delay1, fix16_from_float(1.5));

}
