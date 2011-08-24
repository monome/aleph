//#include <stdlib.h>
#include <limits.h>
#include "bf533_audio_core.h"
#include "../../bf533.h"
#include "../common/param.h"
#include "../paramTables/gainTable.h"
#include "../paramTables/ratioTable.h"
#include "cubeco_data.h"
// fixme
#include "init.h"

//---------- static function prototypes
static void processChannel(const long* in, long* out );
static void processDistortion(float* in, float* out );
static void processEcho(float* in, float* out );
static float itof_scale(long i);
static long ftoi_clip(float f);

//---------- static variables
// heap variables
cubecoData mData;
cubecoData* data = &mData;

// SDRAM variables (much slower access)
cubecoExtData* extData;

// local
static short chan;
static unsigned long iIndex, echoFrames;
static float fIndex, frac, echoMix;

//------------ external function defs
void init_module(void) {
  unsigned long i;
 
  // point the data at start of SDRAM
  extData = (cubecoExtData*)BF533_SDRAM_START;
  
  // populate parameter tables
  populateGainTable(extData->gainTable24, CUBECO_GAIN_TABLE_SIZE, 12.f, -24.f, 100);
  populateGainTable(extData->gainTable6, CUBECO_GAIN_TABLE_SIZE, 6.f, -24.f, 100);
  populateGainTable(extData->gainTable0, CUBECO_GAIN_TABLE_SIZE, 0.f, -24.f, 100);
  populateRatioTable(extData->ratioTable, CUBECO_RATIO_TABLE_SIZE, 5, 0.125);
  // point parameters at tables, assign names and values
  initParamWithTable(&(data->preGain), "preGain", extData->gainTable24, CUBECO_GAIN_TABLE_SIZE, 1.0);
  initParamWithTable(&(data->distortion), "distortion", extData->gainTable6, CUBECO_GAIN_TABLE_SIZE, 0.8);
  initParamWithTable(&(data->postGain), "postGain", extData->gainTable6, CUBECO_GAIN_TABLE_SIZE, 1.0);
  initParamWithTable(&(data->echoTimeRatio), "echoTimeRatio", extData->ratioTable, CUBECO_RATIO_TABLE_SIZE, 0.5);
  initParamWithTable(&(data->echoMix), "echoMix", extData->gainTable0, CUBECO_GAIN_TABLE_SIZE, 0.5);
  initParamWithTable(&(data->feedback), "feedback", extData->gainTable0, CUBECO_GAIN_TABLE_SIZE, 0.7);
  // clear the echo buffer, or be destroyed
  for(i=0; i< CUBECO_ECHO_FRAMES; i++) {
  
    extData->echoBuf[i] = 0;
    //    extData->echoBuf[i][0] = 0.f;
    //    extData->echoBuf[i][1] = 0.f;
    //    extData->echoBuf[i][2] = 0.f;
    //    extData->echoBuf[i][3] = 0.f;
  }

  data->wcount = 0;
}

// benchmark
static volatile unsigned long start, stop, cycles;
static unsigned long minCycles=0xffffff, maxCycles=0;  
static unsigned long cycleBuf[256];
static unsigned long cycleIdx = 0;

void process_frame(void) {
  unsigned long dumbass;

  __asm__ __volatile__("%0 = CYCLES;":"=r"(start));
  start++;
  dumbass = start;
  dumbass++;
  start = dumbass;

  // process parameters (interpolation)
  /*
  processParamFrame(&(data->preGain));
  processParamFrame(&(data->distortion));
  processParamFrame(&(data->postGain));
  processParamFrame(&(data->echoTimeRatio));
  processParamFrame(&(data->echoMix));
  processParamFrame(&(data->feedback));
  

  // calculate echo parameters

   fIndex = (float)(data->wcount) - (getParamF(&(data->echoTimeRatio)) * (float)MODULE_SAMPLERATE);
   echoMix = getParamF(&(data->echoMix));
  */

  // TEST: no parameter handling, benchmark baseline
  // full echo, 1 second

  echoFrames = 96000;

  if (data->wcount >= echoFrames) {
    iIndex = data->wcount - echoFrames;
  } else {
    iIndex = CUBECO_ECHO_FRAMES + data->wcount - echoFrames;
    dumbass = iIndex;
    dumbass++;
  }

  ///// TEST: a single channel, no float conversion
   iOut00 = (extData->echoBuf)[iIndex];
   (extData->echoBuf)[data->wcount] = iIn00 + ((iOut00 >> 3) * 7);

   // flash leds
   if (data->wcount == 0) {
     *pFlashA_PortB_Data = 0b111111;
   }
   if (data->wcount == 24000) {
     *pFlashA_PortB_Data = 0;
   }

  // Process audio
  // chan = 0;
  // processChannel(&iIn00, &iOut00); 
  // iOut00 = iIn00;

  // chan = 1;
  //processChannel(&iIn01, &iOut01); 
  //iOut01 = iOut01;
  iOut01 = iIn01;

  // chan = 2;
  //processChannel(&iIn10, &iOut10); 
  //  iOut10 = iOut10;
  iOut10 = iIn10;

  // chan = 3;
  // processChannel(&iIn11, &iOut11); 
  iOut11 = iIn11;
  

  // increment counters
  //  data->wcount = ((data->wcount + 1) & CUBECO_ECHO_FRAMES_1);
  data->wcount = data->wcount + 1;
  if (data->wcount > CUBECO_ECHO_FRAMES_1) {
    data->wcount -= CUBECO_ECHO_FRAMES;
  }

  // benchmark
  __asm__ __volatile__("%0 = CYCLES;":"=r"(stop));
  stop++;
  dumbass = stop; 
  dumbass++;
  stop = dumbass;
  cycles = stop - start;
  if(cycles > 1000) {
    dumbass = 0;
    dumbass++;
  }
  cycleBuf[cycleIdx] = cycles;
  cycleIdx = (cycleIdx + 1) % 256;
  if (cycles < minCycles) minCycles = cycles;
  if (cycles > maxCycles) maxCycles = cycles;
}

// set parameter, linear scaling from unsigned int
void request_param_set_int(unsigned short idx, unsigned long val) {
  switch(idx) {
  case 3:
    setParamI(&(data->preGain), val);
    break;
  case 5:
    setParamI(&(data->distortion), val);
    break;
  case 2:
    setParamI(&(data->postGain), val);
    break;
  case 0:
    setParamI(&(data->echoTimeRatio), val);
    break;
  case 4:
    setParamI(&(data->echoMix), val);
    break;
  case 1:
    setParamI(&(data->feedback), val);
    break;
  default:
    break;
  }

}

// set parameter, direct value with float
void request_param_set_float(unsigned short idx, float val, char ramp, char sync) {
 switch(idx) {
  case 3:
    setParamF(&(data->preGain), val, ramp, sync);
    break;
  case 5:
    setParamF(&(data->distortion), val, ramp, sync);
    break;
  case 2:
    setParamF(&(data->postGain), val, ramp, sync);
    break;
  case 0:
    setParamF(&(data->echoTimeRatio), val, ramp, sync);
    break;
  case 4:
    setParamF(&(data->echoMix), val, ramp, sync);
    break;
  case 1:
    setParamF(&(data->feedback), val, ramp, sync);
    break;
  default:
    break;
  }

}

//-------------- static function defs
// process audio data for one channel
void processChannel(const long* in, long* out) {
  float fIn = itof_scale(*in);
  float fOut;
  //  if(chan==0) {
    processDistortion(&fIn, &fOut);
    processEcho(&fOut, &fOut);
    //} else {
    //fOut = fIn;
    // }
  *out = ftoi_clip(fOut);
}

void processDistortion(float* in, float* out) {
  const float dist = getParamF(&(data->distortion));
  float x = *in * getParamF(&(data->preGain));
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
  *(out) = y * getParamF(&(data->postGain));
}

void processEcho(float* in, float* out) {

  /*  
  const float x0 = (extData->echoBuf)[(iIndex) % CUBECO_ECHO_FRAMES][chan];
  const float x1 = (extData->echoBuf)[(iIndex+1) % CUBECO_ECHO_FRAMES][chan];
  const float y = x0 * (1.f- frac) + x1 * frac;
  (extData->echoBuf)[data->wcount][chan] = *in + (y * getParamF(&(data->feedback)));
  *out = ((*in) * (1.f - echoMix)) + (y * echoMix);
  */

  /*
  float y;
  signed long idx = (data->wcount) - 24000;
  while (idx < 0) { idx += CUBECO_ECHO_FRAMES; }
  y = (data->echoBuf)[idx][chan];
  data->echoBuf[data->wcount][chan] = *in + (y * 0.5);
  *out = *out + y;
  */
}

// scale signed integer to float in [-1, 1]
float itof_scale(signed long a) {
  if (a & 0x80000000) {
    return (float)((~a & 0x7fffffff) + 1) / (float)INT_MIN;
  } else {
    return (float)(a & 0x7fffffff) / INT_MAX;
  }
}

// clip float to [-1, 1] and convert to signed int
signed long ftoi_clip(float f) {
  float x = f;
  long y;
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
