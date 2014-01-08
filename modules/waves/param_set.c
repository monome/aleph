// set param values.
// this is a separate file for convenience only.

void module_set_param(u32 idx, ParamValue v) {
  switch(idx) {
  case eParamFreq1:
    osc_set_hz( &osc1, v );
    break;
  case eParamFreq2:
    osc_set_hz( &osc2, v );
    break;

  case eParamWave1:
    osc_set_shape( &osc1, BIT_ABS_32(FIX16_FRACT_TRUNC(v)) );
    break;
  case eParamWave2:
    osc_set_shape( &osc2, BIT_ABS_32(FIX16_FRACT_TRUNC(v)) );
    break;

  case eParamPm12:
    osc_set_pm ( &osc2, BIT_ABS_32(FIX16_FRACT_TRUNC(v)) );
    break;
  case eParamPm21:
    osc_set_pm ( &osc1, BIT_ABS_32(FIX16_FRACT_TRUNC(v)) );
    break;

  case eParamWm12:
    osc_set_wm ( &osc2, BIT_ABS_32(FIX16_FRACT_TRUNC(v)) );
    break;
  case eParamWm21:
    osc_set_wm ( &osc1, BIT_ABS_32(FIX16_FRACT_TRUNC(v)) );
    break;

  case eParamAmp1:
     filter_1p_lo_in(amp1Lp, v);
    break;
  case eParamAmp2:
    filter_1p_lo_in(amp2Lp, v);
    break;

  case eParamFreq1Smooth:
    filter_1p_lo_set_slew( &(osc1.lpInc), v); 
    break;
  case eParamFreq2Smooth:
    filter_1p_lo_set_slew( &(osc2.lpInc), v); 
    break;

  case eParamPm21Smooth:
    filter_1p_lo_set_slew( &(osc1.lpPm), v); 
    break;
  case eParamPm12Smooth:
    filter_1p_lo_set_slew( &(osc2.lpPm), v); 
    break;

  case eParamWm21Smooth:
    filter_1p_lo_set_slew( &(osc1.lpWm), v); 
    break;
  case eParamWm12Smooth:
    filter_1p_lo_set_slew( &(osc2.lpWm), v); 
    break;

  case eParamWave1Smooth:
    filter_1p_lo_set_slew( &(osc1.lpShape), v); 
    break;
  case eParamWave2Smooth:
    filter_1p_lo_set_slew( &(osc2.lpShape), v); 
    break;
  case eParamAmp1Smooth:
    filter_1p_lo_set_slew(amp1Lp, v);
    break;
  case eParamAmp2Smooth:
    filter_1p_lo_set_slew(amp2Lp, v);
    break;

  case eParamIoAmp0:
    ioAmp0 = (v);
    break;
  case eParamIoAmp1:
    ioAmp1 = (v);
    break;
  case eParamIoAmp2:
    ioAmp2 = (v);
    break;
  case eParamIoAmp3:
    ioAmp3 = (v);
    break;
  default:
    break;
  }

}
