/* module_custom.h
   
   some consants should be both customized for each module,
   and preprocessed for efficiency.
   here is where such things live.

   most likely, this is just a count of parameters.
   
   a copy of this header should exist in your custom module directory 
   (e.g. aleph/bfin/modules/mymodule/
 */

#ifndef _ALEPH_MODULE_CUSTOM_H_
#define _ALEPH_MODULE_CUSTOM_H_

///// inputs
enum params {

  eParamFreq1,
  eParamFreq2,

  eParamTune1,
  eParamTune2,

  eParamWave1,
  eParamWave2,

  eParamAmp1,
  eParamAmp2,

  eParamPm12,
  eParamPm21,

  eParamWm12,
  eParamWm21,

  eParamBl1,
  eParamBl2,

  // filter 1
  eParam_cut1,		
  eParam_rq1,		
  eParam_low1,		
  eParam_high1,		
  eParam_band1,		
  eParam_notch1,	
  eParam_fwet1,		
  eParam_fdry1,		

  // filter 2
  eParam_cut2,
  eParam_rq2,
  eParam_low2,		
  eParam_high2,		
  eParam_band2,		
  eParam_notch2,	
  eParam_fwet2,		
  eParam_fdry2,		

  eParamFreq1Smooth,
  eParamFreq2Smooth,

  eParamPm12Smooth,
  eParamPm21Smooth,

  eParamWm12Smooth,
  eParamWm21Smooth,

  eParamWave1Smooth,
  eParamWave2Smooth,
  eParamAmp1Smooth,
  eParamAmp2Smooth,

  eParamIoAmp0,
  eParamIoAmp1, 
  eParamIoAmp2,
  eParamIoAmp3,

  eParamNumParams
};

#define NUM_PARAMS eParamNumParams

#endif // h guard
