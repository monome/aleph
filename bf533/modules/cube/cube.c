#include "bf533_audio_core.h"

// #include "../common/Types.h"
//#include "../paramTables/gainTable6db.h"
//#include "../paramTables/gainTable0db.h"
//#include "../paramTables/ratioTable_5_32.h"

//#include "../common/Utils.h"
//#include "../common/Module.h"
//#include "../common/Param.h"

//---------- defines

//---------- static function prototypes
void process_channel(const int* in, int* out );

//---------- static variables

// TODO: params
static float preGain = 2.0;
static float dist = 0.5;
static float postGain = 0.75;

void process_frame(void) {
  process_channel(&iIn00, &iOut00); 
  process_channel(&iIn01, &iOut01); 
  process_channel(&iIn10, &iOut10); 
  process_channel(&iIn11, &iOut11); 
}

void process_channel(const int* in, int* out) {
  const float x = (float)(*in);
  float y = x * preGain;

  if (x > dist) {
    const float b = (x-dist) / (1.f-dist);
    y = dist + (x-dist) / (1.f + (b*b));
  }
  if (x > 1.f) {
    y = (dist + 1.f) / 2.f;
  }	
  *(out) = y * postGain;
}

void set_param(unsigned int idx, unsigned int val, unsigned int bitDepth) {
  float fVal;
  const float bitScale = (float)((1 << bitDepth) - 1);
  fVal = (float)val / bitScale;
  // fVal is now scaled to [0, 1]
  switch(idx) {
  case 0:
    preGain = fVal * 2.0;
    break;
  case 1:
    dist = fVal * 2.0;
    break;
  case 2:
    postGain = fVal * 2.0;
    break;
  default:
    return;
  }
}

