//#include <stdlib.h>
#include <limits.h>
#include "bf533_audio_core.h"
#include "../../bf533.h"
#include "../common/param.h"
#include "../paramTables/gainTable.h"
#include "../paramTables/ratioTable.h"
#include "cubeco_data.h"

//---------- static function prototypes
static void processChannel(const int* in, int* out );
static void processDistortion(float* in, float* out );
static void processEcho(float* in, float* out );
static float itof_scale(signed int i);
static signed int ftoi_clip(float f);

//---------- static variables
// module data lives in SDRAM... is this stupid?
cubecoData* data;

///////////////
// there is something fucked up with SDRAM access time.
// attempting to stick the data on the heap to test
// static cubecoData theData;
// cubecoData * data = &theData;

static int chan;
static unsigned int iIndex;
static float fIndex, frac, echoMix;

/*
// test...
int* shit; // = SDRAM_ADDRESS;
static const int  kNumShits = 4096;
*/

//------------ external function defs
void init_module(void) {
  unsigned int i;
 
  // point the data at start of SDRAM
  data = (cubecoData*)BF533_SDRAM_START;
  
  // populate parameter tables
  populateGainTable(data->gainTable24, CUBECO_GAIN_TABLE_SIZE, 12.f, -24.f, 100);
  populateGainTable(data->gainTable6, CUBECO_GAIN_TABLE_SIZE, 6.f, -24.f, 100);
  populateGainTable(data->gainTable0, CUBECO_GAIN_TABLE_SIZE, 0.f, -24.f, 100);
  populateRatioTable(data->ratioTable, CUBECO_RATIO_TABLE_SIZE, 5, 0.125);
  // point parameters at tables, assign names and values
  initParamWithTable(&(data->preGain), "preGain", data->gainTable24, CUBECO_GAIN_TABLE_SIZE, 1.0);
  initParamWithTable(&(data->distortion), "distortion", data->gainTable6, CUBECO_GAIN_TABLE_SIZE, 0.8);
  initParamWithTable(&(data->postGain), "postGain", data->gainTable6, CUBECO_GAIN_TABLE_SIZE, 1.0);
  initParamWithTable(&(data->echoTimeRatio), "echoTimeRatio", data->ratioTable, CUBECO_RATIO_TABLE_SIZE, 1.0);
  initParamWithTable(&(data->echoMix), "echoMix", data->gainTable0, CUBECO_GAIN_TABLE_SIZE, 0.5);
  initParamWithTable(&(data->feedback), "feedback", data->gainTable0, CUBECO_GAIN_TABLE_SIZE, 0.7);
  // clear the echo buffer, or be destroyed
  for(i=0; i< CUBECO_ECHO_FRAMES; i++) {
    data->echoBuf[i][0] = 0.f;
    data->echoBuf[i][1] = 0.f;
    data->echoBuf[i][2] = 0.f;
    data->echoBuf[i][3] = 0.f;
  }

  /*
  // test...
  // shit = malloc(sizeof(int) * kNumShits);
  //  shit = SDRAM_ADDRESS;  
  shit = 0x00000000;  
shit[0] = 0;
  shit[1] = 0;
  shit[2] = 0;
  shit[3] = 0;
  */
  
}

void process_frame(void) {
  // process parameters (interpolation)
  /*
  processParamFrame(&(data->preGain));
  processParamFrame(&(data->distortion));
  processParamFrame(&(data->postGain));
  processParamFrame(&(data->echoTimeRatio));
  processParamFrame(&(data->echoMix));
  processParamFrame(&(data->feedback));
  */

  // calculate echo parameters
  fIndex = (float)(data->wcount) - (getParamFloat(&(data->echoTimeRatio)) * (float)MODULE_SAMPLERATE);
  while (fIndex < 0.f) { fIndex += CUBECO_ECHO_FRAMES; }
  iIndex = (unsigned int)fIndex;
  frac = fIndex - iIndex;
  echoMix = getParamFloat(&(data->echoMix));

  // Process audio
  chan = 0;
  processChannel(&iIn00, &iOut00); 
  chan = 1;
  processChannel(&iIn01, &iOut01); 
  chan = 2;
  processChannel(&iIn10, &iOut10); 
  chan = 3;
  processChannel(&iIn11, &iOut11); 
  //increment counters
  data->wcount = ((data->wcount + 1) % CUBECO_ECHO_FRAMES);
}

// set parameter, linear scaling from unsigned int
void request_param_set_int(unsigned int idx, unsigned int val) {
  switch(idx) {
  case 3:
    setParamInt(&(data->preGain), val);
    break;
  case 5:
    setParamInt(&(data->distortion), val);
    break;
  case 2:
    setParamInt(&(data->postGain), val);
    break;
  case 0:
    setParamInt(&(data->echoTimeRatio), val);
    break;
  case 4:
    setParamInt(&(data->echoMix), val);
    break;
  case 1:
    setParamInt(&(data->feedback), val);
    break;
  default:
    break;
  }
}

// set parameter, direct value with float
void request_param_set_float(unsigned int idx, float val, char ramp, char sync) {
 switch(idx) {
  case 3:
    setParamFloat(&(data->preGain), val, ramp, sync);
    break;
  case 5:
    setParamFloat(&(data->distortion), val, ramp, sync);
    break;
  case 2:
    setParamFloat(&(data->postGain), val, ramp, sync);
    break;
  case 0:
    setParamFloat(&(data->echoTimeRatio), val, ramp, sync);
    break;
  case 4:
    setParamFloat(&(data->echoMix), val, ramp, sync);
    break;
  case 1:
    setParamFloat(&(data->feedback), val, ramp, sync);
    break;
  default:
    break;
  }
}

//-------------- static function defs
// process audio data for one channel
void processChannel(const int* in, int* out) {
  float fIn = itof_scale(*in);
  float fOut;
  processDistortion(&fIn, &fOut);
  processEcho(&fOut, &fOut);
  // fOut = fIn;
  // processEcho(&fIn, &fOut);
  //fOut = fIn;
  *out = ftoi_clip(fOut);
}

void processDistortion(float* in, float* out) {
  const float dist = getParamFloat(&(data->distortion));
  float x = *in * getParamFloat(&(data->preGain));
  float y;
  const float sign = (x > 0.f ? 1.f : -1.f);
  //if (x < 0.f) x = x * -1.f; 
  x *= sign; // abs
  if (x > dist) {
    const float diff = x - dist;
    const float b = diff / (1.f-dist);
    y = dist + diff / (1.f + (b*b));
  }
  else {
    y = x;
  }
  if (x > 1.f) {
    y = (dist + 1.f) / 2.f;
  }
  y = y * sign;	
  *(out) = y * getParamFloat(&(data->postGain));
}

void processEcho(float* in, float* out) {
  
  const float x0 = (data->echoBuf)[(iIndex) % CUBECO_ECHO_FRAMES][chan];
  const float x1 = (data->echoBuf)[(iIndex+1) % CUBECO_ECHO_FRAMES][chan];
  const float y = x0 * (1.f- frac) + x1 * frac;
  (data->echoBuf)[data->wcount][chan] = *in + (y * getParamFloat(&(data->feedback)));
  *out = ((*in) * (1.f - echoMix)) + (y * echoMix);
  
  /*
  float y;
  signed int idx = (data->wcount) - 24000;
  while (idx < 0) { idx += CUBECO_ECHO_FRAMES; }
  y = (data->echoBuf)[idx][chan];
  data->echoBuf[data->wcount][chan] = *in + (y * 0.5);
  *out = *out + y;
  */
}

// scale signed integer to float in [-1, 1]
float itof_scale(signed int a) {
  if (a & 0x80000000) {
    return (float)((~a & 0x7fffffff) + 1) / (float)INT_MIN;
  } else {
    return (float)(a & 0x7fffffff) / INT_MAX;
  }
}

// clip float to [-1, 1] and convert to signed int
signed int ftoi_clip(float f) {
  float x = f;
  int y;
  if (x <= 0.f) {
    if (x < -1.f) {
      return INT_MIN;
    }
    y = (int)((float)(INT_MAX - 1) * x);
    
  } else {
    if (x >= 1.f) {
      return INT_MAX;
    }
    y = (int)(((float)(INT_MAX - 1)) * x);      
  }
  return y;
}
