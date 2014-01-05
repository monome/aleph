// set param values.
// this is a separate file for convenience only.

void module_set_param(u32 idx, ParamValue v) {
  switch(idx) {
  case eParamFreq1:
    set_freq1(v);
    break;
  case eParamFreq2:
    set_freq2(v);
    break;
  /* case eParamRatio2: */
  /*   set_ratio2(v); */
  /*   break; */
  /* case eParamTrack: */
  /*   track = (v > 0); */
  /*   break; */
  case eParamWave1:
        filter_1p_lo_in(wave1Lp, BIT_ABS_32(FIX16_FRACT_TRUNC(v)));
    //    filter_1p_lo_in(wave1Lp, BIT_ABS_32((v)));
    break;
  case eParamWave2:
        filter_1p_lo_in(wave2Lp, BIT_ABS_32(FIX16_FRACT_TRUNC(v)));
	//    filter_1p_lo_in(wave2Lp, BIT_ABS_32((v)));
    break;
  case eParamPm:
       filter_1p_lo_in(pmLp, BIT_ABS_32(FIX16_FRACT_TRUNC(v)));
       //    filter_1p_lo_in(pmLp, BIT_ABS_32((v)));
    break;
  case eParamAmp1:
    filter_1p_lo_in(amp1Lp, v);
    break;
  case eParamAmp2:
    filter_1p_lo_in(amp2Lp, v);
    break;
  /* case eParamGate: */
  /*    env_asr_set_gate(env, v > 0); */
  /*   break; */
  /* case eParamAtkDur: */
  /*   env_asr_set_atk_dur(env, sec_to_frames_trunc(v)); */
  /*   break; */
  /* case eParamRelDur: */
  /*   env_asr_set_rel_dur(env, sec_to_frames_trunc(v)); */
  /*   break; */
  /* case eParamAtkCurve: */
  /*   env_asr_set_atk_shape(env, BIT_ABS(FIX16_FRACT_TRUNC(v))); */
  /*   break; */
  /* case eParamRelCurve: */
  /*   env_asr_set_atk_shape(env, BIT_ABS(FIX16_FRACT_TRUNC(v))); */
  /*   break; */
  case eParamFreq1Smooth:
    filter_1p_lo_set_slew(freq1Lp, v);
    break;
  case eParamFreq2Smooth:
    filter_1p_lo_set_slew(freq2Lp, v);
    break;
  case eParamPmSmooth:
    filter_1p_lo_set_slew(pmLp, v);
    break;
  case eParamWave1Smooth:
    filter_1p_lo_set_slew(wave1Lp, v);
    break;
  case eParamWave2Smooth:
    filter_1p_lo_set_slew(wave2Lp, v);
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
