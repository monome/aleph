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

  eParamAmp1,
  eParamAmp2,

  eParamIoAmp0,
  eParamIoAmp1, 

  eParamIoAmp2,
  eParamIoAmp3,

  eParamPm,
  eParamWave1,
  eParamWave2,

  eParamFreq1Smooth,
  eParamFreq2Smooth,
  eParamPmSmooth,
  eParamWave1Smooth,
  eParamWave2Smooth,
  eParamAmp1Smooth,
  eParamAmp2Smooth,
  eParamNumParams
};

#define NUM_PARAMS eParamNumParams

#endif // h guard
