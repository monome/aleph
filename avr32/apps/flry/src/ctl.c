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

// blah
#define DSP_STRING "aleph-dacs"

//----------------------------------------
//---- static variables

//-- parameter values
// inputs. use s32 type but unsigned range (accumulators)
s32 dac[4];

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
  if(strcmp((const char*)buf, DSP_STRING)) {
    print_dbg( "\r\n report_params fail (module name mismatch)" );
    return 0;
  } else {
    return 1;
  }
}

// set initial parameters
void ctl_init_params(void) {
  // dacs at 0
  ctl_param_change(eParam_dac0, 0);
  ctl_param_change(eParam_dac1, 0);
  ctl_param_change(eParam_dac2, 0);
  ctl_param_change(eParam_dac3, 0);

}

// set dac value
void  ctl_set_dac(u8 ch, u16 val) {
  // param enum hack...
  dac[ch] = val;
  ctl_param_change(eParam_dac0 + ch, val);
  render_dac(ch, val);
}

// increment dac value
void ctl_inc_dac(u8 ch, s32 delta) {
  s32 tmp = dac[ch] + delta;

  if(tmp > PARAM_DAC_MAX) {    
    tmp = PARAM_DAC_MAX;
  }

  if(tmp < PARAM_DAC_MIN) {
    if (delta>0) { // overflow
      tmp = PARAM_DAC_MAX;
    } else {
      tmp = PARAM_DAC_MIN;
    }
  }
  ctl_set_dac(ch, tmp);
}
