
#ifndef _ALEPH_APP_FLRY_CTL_H_
#define _ALEPH_APP_FLRY_CTL_H_

#define PARAM_DAC_MIN 0x00000000
#define PARAM_DAC_MAX 0xffff

//---------------------------
//---- -external functions

// query the DSP for parameter descriptors. 
// return 0 if something seems wrong, 1 otherwise
u8 ctl_report_params(void);
// set parameters to initial values
void ctl_init_params(void);
// set gate
void  ctl_set_gate(u8 ch, u8 val);

// parameter enum
// FIXME: just copying from .ldr source for now
typedef enum Params {
  eParamGate0,
  eParamTrig0,
  eParamAmp0,
  eParamAmpAtkDur0,
  eParamAmpRelDur0,
  eParamAmpAtkCurve0,
  eParamAmpRelCurve0,
  eParamFreqAtkDur0,
  eParamFreqRelDur0,
  eParamFreqAtkCurve0,
  eParamFreqRelCurve0,
  eParamFreqMul0,
  eParamFreqAdd0,
  eParamResAtkDur0,
  eParamResRelDur0,
  eParamResAtkCurve0,
  eParamResRelCurve0,
  eParamResMul0,
  eParamResAdd0,
  eParamLow0,
  eParamHigh0,
  eParamBand0,
  eParamNotch0,

  eParamGate1,
  eParamTrig1,
  eParamAmp1,
  eParamAmpAtkDur1,
  eParamAmpRelDur1,
  eParamAmpAtkCurve1,
  eParamAmpRelCurve1,
  eParamFreqAtkDur1,
  eParamFreqRelDur1,
  eParamFreqAtkCurve1,
  eParamFreqRelCurve1,
  eParamFreqMul1,
  eParamFreqAdd1,
  eParamResAtkDur1,
  eParamResRelDur1,
  eParamResAtkCurve1,
  eParamResRelCurve1,
  eParamResMul1,
  eParamResAdd1,
  eParamLow1,
  eParamHigh1,
  eParamBand1,
  eParamNotch1,

  eParamGate2,
  eParamTrig2,
  eParamAmp2,
  eParamAmpAtkDur2,
  eParamAmpRelDur2,
  eParamAmpAtkCurve2,
  eParamAmpRelCurve2,
  eParamFreqAtkDur2,
  eParamFreqRelDur2,
  eParamFreqAtkCurve2,
  eParamFreqRelCurve2,
  eParamFreqMul2,
  eParamFreqAdd2,
  eParamResAtkDur2,
  eParamResRelDur2,
  eParamResAtkCurve2,
  eParamResRelCurve2,
  eParamResMul2,
  eParamResAdd2,
  eParamLow2,
  eParamHigh2,
  eParamBand2,
  eParamNotch2, 

  eParamGate3,
  eParamTrig3,
  eParamAmp3,
  eParamAmpAtkDur3,
  eParamAmpRelDur3,
  eParamAmpAtkCurve3,
  eParamAmpRelCurve3,
  eParamFreqAtkDur3,
  eParamFreqRelDur3,
  eParamFreqAtkCurve3,
  eParamFreqRelCurve3,
  eParamFreqMul3,
  eParamFreqAdd3,
  eParamResAtkDur3,
  eParamResRelDur3,
  eParamResAtkCurve3,
  eParamResRelCurve3,
  eParamResMul3,
  eParamResAdd3,
  eParamLow3,
  eParamHigh3,
  eParamBand3,
  eParamNotch3,

  eParamNumParams
} eParam;



#endif // h guard

