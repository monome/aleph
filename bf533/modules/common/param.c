#include <math.h>
#include "../../../common/protocol.h"
#include "param.h"

// TODO: implmement linear parameters without lookup tables!!!!


// =========== static function declarations
// synchronize param's step value with current float value (nearest match)
static void syncParamStepValue(Param* param);
// synchronize param's ramp value with current float value (nearest match)
static void syncParamRampValue(Param* param);
// synchronize all param values to current float value
static void syncParamValues(Param* param);
// re-compute ramp increment and reset interpolation value
static void computeParamRamp(Param* param);

// =========== function definitions

const char* getParamName(Param* param) {
  return (param->name);
}

u16 getParamStep(Param* param) {
  return param->iVal;
}

f32 getParamFloat(Param* param) {
   return param->fVal;
}

u16 getParamImax(Param* param) {
  return param->iMax;
}

u16 getParamFmax(Param* param) {
  return param->fMax;
}

f32 getParamRampTime(Param* param) {
  return param->rampTime;
}

//----- setters
// change the value of the param in stepped increments

void stepParam(Param* param, s16 inc) {
  s16 ival;
  ival = param->iVal + inc;
  if (ival > param->iMax) ival = param->iMax;
  if (ival < 0) ival = 0;
  param->iVal = ival;
  param->fVal = (param->table)[ival];
}

void setParamFloat(Param* param, f32 val, u8 sync, u8 ramp) {
  if (ramp) {    
    param->fTarget = val;  
    param->rampFlag = 1;
    computeParamRamp(param);
  } else {
    param->fTarget = val;  
    param->fVal = val;  
    param->rampFlag = 0;
  }
  if (sync) { syncParamStepValue(param); }
}

void setParamInt(Param* param, u32 val) {
  param->iVal = (u32)((float)val / param->iScale);
  param->fVal = param->table[param->iVal];
}

void setParamRampTime(Param* param, f32 val) {
  param->rampTime = val;
  if(param->rampFlag) {
    computeParamRamp(param);
  }
}

//-------- other external functions

// intialize one parameter
void initParamWithTable(Param* param, 
	       char name[PARAM_NAME_LEN],
	       f32* table,
	       u16 tableSize,
	       f32 val) {
  /// fixme: fuckin strings  
//  param->name = name;
  param->table = table;
  param->iMax = tableSize-1;
  param->fMin = table[0];
  param->fMax = table[param->iMax];
  param->fVal = val;
  param->rampFlag = 0;
  param->rampTime = 0.125;
  param->iScale = (float)(param->iMax) / (powf(2.f, P_PARAM_MSG_INT_BIT_DEPTH) - 1);
  syncParamValues(param);
}


// FIXME
/*
void initParamNoTable(Param* param, 
		      char name[PARAM_NAME_LEN]
,
		      f32 min,
		      f32 max,
		      f32 val) {
  // fixme: god damn strings
  //  param->name = name;
  param->table = NULL;
  param->iMax = 0; // no table
  param->fMin = min;
  param->fMax = max;
  param->fVal = val;
  param->rampFlag = 0;
  param->rampTime = 0.125;
  syncParamValues(param);
}
*/

f32 processParamFrame(Param* param) {
  if (param->rampFlag) {
    param->rampVal += (param->rampStep);
    // TODO: want interpolation between table entries here?
    // hm.. probably yes. so maybe a compile time option 
    param->fVal = param->table[(u16)(param->rampVal)];
    // check if we overshot/undershot the target    
    if (param->rampStep > 0) {
      if(param->fVal >= param->fTarget) {
	param->fVal = param->fTarget;
	param->rampFlag = 0;
	syncParamRampValue(param);
	syncParamStepValue(param);
	//refreshSingleParamDisplay(idx);
      }
    } else { // step is negative, check for undershoot
      if(param->fVal < param->fTarget) {
	param->fVal = param->fTarget;
	param->rampFlag = 0;
	syncParamRampValue(param);
	syncParamStepValue(param);
	//refreshSingleParamDisplay(idx);
      }
    }
  }
  return param->fVal;
}

//-------- static functions

int findClosestIndex(f32* tab, const u16 tabSize, f32 target) {
  s16 v = -1;
  f32 diff = 1000000.f;
  f32 testdiff;
  u16 i;

  for(i=0; i<tabSize; i++) {
    testdiff = tab[i] - target;
    if (testdiff < 0.f) testdiff *= -1.f;
    if (testdiff < diff) {
      diff = testdiff;
      v = i;
    }
  }
  return v;
}

void syncParamStepValue(Param* param) {
  param->iVal = findClosestIndex(param->table, param->iMax,param->fVal);
}

void syncParamRampValue(Param* param) {
  param->rampVal = findClosestIndex(param->table, param->iMax,param->fVal);
}

void syncParamValues(Param* param) {
  const u16 step = findClosestIndex(param->table, param->iMax,param->fVal);
  param->rampVal = step;
  param->iVal = step;
}

void computeParamRamp(Param* param) {
  const u32 rampFrames = MODULE_SAMPLERATE * param->rampTime;
  // bail if the requested ramp time is less that 1 sample
  if (rampFrames < 1) { return; }
  param->rampStep = (findClosestIndex(param->table, param->iMax, param->fTarget) - param->rampVal) / rampFrames;
}
