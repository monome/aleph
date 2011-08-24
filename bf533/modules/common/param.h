/* param.h
 * aleph-blackfin modules
 *
 * public interface for parameter access
 */

/*
TODO: encapsulation is all messed up now that params no longer live in an array/take an index for access. we need private accessors (whicht take a pointer) and puvlic accessors (which simply take an index, look up the appropriate pointer in the patch module, and call the private accessor.)
 */

#ifndef _SBX_PARAM_H_
#define _SBX_PARAM_H_

#include "types.h"

#define PARAM_NAME_LEN 32
// TODO: this should go somewhere else, of course
#define MODULE_SAMPLERATE 48000

//============ data types
// parameter data structure
typedef struct __attribute__((__packed__)) ParamStruct {
  char name[PARAM_NAME_LEN];
  u16 iVal;           // unsigned integer value, scaled to param's tablesize  
  f32 iScale;          // ratio of (message bit-depth / table size) -- 0 if no table
  f32 fVal;           // float value, expressed in the param's real units
  f32* table;         // pointer to first element of lookup table (can be NULL?)
  u16 iMax;           // maximum index (table size - 1) -- 0 if no table
  f32 fMax;           // maximum float value -- or last entry in table
  f32 fMin;           // minimum float value -- or first entry in table
  f32 fTarget;        // target float value for interpolation
  f32 rampTime;       // transition time
  f32 rampStep;       // per-sample increment to get to the target 
  f32 rampVal;        // current interpolated index in table
  u8 rampFlag;        // is param currently interpolating?
} Param;

//============= function declarations
// ---- getters
// for now, just return an exact match, negative if none
s16 getParamIndex(const char* name);
// get name from index
// const char* getParamName(Param* param);
// get the value of a parameter, scaled to [0, ((2^SBX_PARAM_BITS)-1)]
u16 getParamI(Param* param);
// get the actual used value (e.g. hz) of a parameter, as a float
f32 getParamF(Param* param);
// get maximum step value (table size)
u16 getParamImax(Param* param);
// get maximum float value;
u16 getParamFmax(Param* param);
// get the ramp time
f32 getParamRamp(Param* param);

//----- setters
// move the param value by a specified number of steps
void stepParam(Param* param, s16 inc);
// move the param value to a specified float
void setParamF(Param* param, f32 val, u8 sync, u8 ramp);
// move the param value to a specified int, convert to internal scaling
void setParamI(Param* param, u32 val);
// set the ramp time
void setParamRamp(Param* param, f32 val);
// set the scene storage flag
// void setParamSceneFlag(Param* param, u8 val);

// intialize a given param with values
void initParamWithTable(Param* param, char name[PARAM_NAME_LEN], f32* table, u16 tableSize, f32 val);

// same, no table
// TODO
// void initParamNoTable(Param* param, char name[PARAM_NAME_LEN], f32 min, f32 max, f32 val);

// perform parameter ramping on an audio frame, 
// return the processed value
f32 processParamFrame(Param* param);

#endif
