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
#define DSP_STRING "aleph-drumsyn"


// testing values.
/* the env_int class takes durations as integrator coefficients.
   (FIXME: should rename the params in aleph-drumsyn,
   and use dsyn/inputs.c for conversion).
*/

//	2640 hz	~= 0.707813107994074	~= 0x5a999eb2
#define ENV_DUR_ATK_SHORT 0x5a999eb2

//	`55 hz 	~= 0.9928263710316179	~= 0x7f14ef3c
#define ENV_DUR_ATK_LONG 0x7f14ef3c

//	2 hz	~= 0.9997382348786702	~= 0x7ff76c26
#define ENV_DUR_REL_SHORT 0x7ff76c26

//	1/2 hz 	~= 0.9999345522948447	~= 0x7ffddafb
#define ENV_DUR_REL_LONG 0x7ffddafb



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
  /// TODO: make a text parser for intial parameters and read from sdcard?
  ctl_param_change(  eParamGate0, 		0 );
  ctl_param_change(  eParamTrig0, 		0 );
  ctl_param_change(  eParamAmp0, 		FR32_MAX - 1 );
  /// FIXME: durations are actually integrator coefficients :S
  /// so for now, these are hardcoded constants above.
  ctl_param_change(  eParamAmpAtkDur0, 		ENV_DUR_ATK_SHORT );
  ctl_param_change(  eParamAmpRelDur0, 		ENV_DUR_REL_LONG );
  ctl_param_change(  eParamAmpAtkCurve0, 	0x0 );
  ctl_param_change(  eParamAmpRelCurve0, 	0x0 );
  ctl_param_change(  eParamFreqAtkDur0, 	ENV_DUR_ATK_LONG );
  ctl_param_change(  eParamFreqRelDur0, 	ENV_DUR_REL_SHORT );
  ctl_param_change(  eParamFreqAtkCurve0, 	0x0 );
  ctl_param_change(  eParamFreqRelCurve0, 	0x0 );
  ctl_param_change(  eParamFreqMul0, 		fr32_from_float(0.5) );
  ctl_param_change(  eParamFreqAdd0, 		fr32_from_float(0.01) );
  ctl_param_change(  eParamResAtkDur0, 		ENV_DUR_ATK_LONG );
  ctl_param_change(  eParamResRelDur0, 		ENV_DUR_REL_SHORT );
  ctl_param_change(  eParamResAtkCurve0, 	0x0 );
  ctl_param_change(  eParamResRelCurve0, 	0x0 );
  ctl_param_change(  eParamResMul0, 		fr32_from_float(-0.2) );
  ctl_param_change(  eParamResAdd0, 		fr32_from_float(0.4) );
  ctl_param_change(  eParamLow0, 		FR32_MAX - 1 );
  ctl_param_change(  eParamHigh0, 		0 );
  ctl_param_change(  eParamBand0, 		0 );
  ctl_param_change(  eParamNotch0, 		0 );
}

// set gate
void  ctl_set_gate(u8 ch, u8 val) {
  static const eParam gateParams[4] = { eParamGate0, eParamGate1, eParamGate2, eParamGate3,  };
  eParam p = gateParams[ch];
  ctl_param_change(p, (s32)val);
}
