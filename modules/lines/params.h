#ifndef _ALEPH_LINES_PARAMS_H_
#define _ALEPH_LINES_PARAMS_H_

#define PARAM_HZ_MIN OSC_FREQ_MIN
#define PARAM_HZ_MAX OSC_FREQ_MIN
#define PARAM_HZ_DEFAULT (OSC_FREQ_MIN * 16)

#define PARAM_DAC_MIN 0
//#define PARAM_DAC_MAX (10 << 16)
// bah?
#define PARAM_DAC_MAX 0x7fffffff
#define PARAM_DAC_RADIX 16

#define PARAM_RATE_MIN 0x2000 // 1/8
#define PARAM_RATE_MAX 0x80000 // 8
#define PARAM_RATE_RADIX 3

/* #define RATIO_MIN 0x4000     // 1/4 */
/* #define RATIO_MAX 0x40000    // 4 */
/* #define RATIO_RADIX 3 */

#define SMOOTH_FREQ_MIN 0x2000 // 1/8
#define SMOOTH_FREQ_MAX 0x400000 // 64
#define SMOOTH_FREQ_RADIX 7

// svf cutoff
#define PARAM_CUT_MAX     0x7fffffff
#define PARAM_CUT_DEFAULT 0x43D0A8EC

// rq
#define PARAM_RQ_MIN 0x00000000
#define PARAM_RQ_MAX 0x0000ffff
#define PARAM_RQ_DEFAULT 0x0000FFF0

#define PARAM_AMP_0 (FRACT32_MAX)
#define PARAM_AMP_6 (FRACT32_MAX >> 1)
#define PARAM_AMP_12 (FRACT32_MAX >> 2)

// max time in seconds, 16.16
#define PARAM_SECONDS_MAX 0x003c0000
#define PARAM_SECONDS_RADIX 7

// enumerate parameters
enum params {

  // delay input mix
  eParam_adc0_del0,		
  eParam_adc0_del1,		
  eParam_adc1_del0,		
  eParam_adc1_del1,		
  eParam_adc2_del0,		
  eParam_adc2_del1,		
  eParam_adc3_del0,		
  eParam_adc3_del1,		

  // delay output mix
  eParam_del0_dac0,		
  eParam_del0_dac1,		
  eParam_del0_dac2,		
  eParam_del0_dac3,		
  eParam_del1_dac0,		
  eParam_del1_dac1,		
  eParam_del1_dac2,		
  eParam_del1_dac3,		

  // feedback mix
  eParam_del0_del0,		
  eParam_del0_del1,		
  eParam_del1_del0,		
  eParam_del1_del1,		

  // dry
  eParam_adc0_dac0,		
  eParam_adc0_dac1,		
  eParam_adc0_dac2,		
  eParam_adc0_dac3,		
  eParam_adc1_dac0,		
  eParam_adc1_dac1,		
  eParam_adc1_dac2,		
  eParam_adc1_dac3,		
  eParam_adc2_dac0,		
  eParam_adc2_dac1,		
  eParam_adc2_dac2,		
  eParam_adc2_dac3,		
  eParam_adc3_dac0,		
  eParam_adc3_dac1,		
  eParam_adc3_dac2,		
  eParam_adc3_dac3,		

  // line 1
  eParam_freq1,		
  eParam_rq1,			
  eParam_low1,			
  eParam_high1,			
  eParam_band1,			
  eParam_notch1,		
  eParam_fwet1,			
  eParam_fdry1,		    
  eParam_pos_read1,	       
  eParam_pos_write1,	       
  eParam_run_read1,		
  eParam_run_write1,	    
  eParam_loop1,			
  eParam_rMul1,			
  eParam_rDiv1,		       
  eParam_pre1,		       
  eParam_write1,	       
  eParam_delay1,	   

  // line 0
  eParam_freq0,		
  eParam_rq0,			
  eParam_low0,			
  eParam_high0,			
  eParam_band0,			
  eParam_notch0,		
  eParam_fwet0,			
  eParam_fdry0,		    
  eParam_pos_read0,	       
  eParam_pos_write0,	       
  eParam_run_read0,		
  eParam_run_write0,	    
  eParam_loop0,			
  eParam_rMul0,			
  eParam_rDiv0,		       
  eParam_pre0,		       
  eParam_write0,	       
  eParam_delay0,	   

  eParamNumParams
};  


extern void fill_param_desc(void);

#endif // header guard 
// EOF
