#include "bf533_audio_core.h"

// #include "../common/Types.h"
//#include "../paramTables/gainTable6db.h"
//#include "../paramTables/gainTable0db.h"
//#include "../paramTables/ratioTable_5_32.h"

//#include "../common/Utils.h"
//#include "../common/Module.h"
//#include "../common/Param.h"

//---------- defines
//#define MAX_ECHO_FRAMES 2646000 // 1 minute at 44.1k
// TODO: figure out how to address SDRAM!!
#define MAX_ECHO_FRAMES 96000

//---------- static function prototypes
void process_channel(const int* in, int* out, const unsigned int index, const float frac, const int chan );

//---------- static variables
// echo frame counter
static long unsigned int wcount = 0;
// stereo buffer
static float buf[MAX_ECHO_FRAMES][4];
// LPF'd echo samples
static unsigned int echoSampsFiltered;

// TODO: params
static float mix = 0.5;
static float echosamps = 48000;
static float feedback = 0.5;

void process_frame(void) {

  unsigned int iIndex;
  float fIndex, frac;

  fIndex = wcount - echosamps;
  while (fIndex < 0.f) { fIndex += MAX_ECHO_FRAMES; }
  iIndex = (unsigned int)fIndex;
  frac = fIndex - iIndex;
  
  process_channel(&iIn00, &iOut00, iIndex, frac, 0); 
  process_channel(&iIn01, &iOut01, iIndex, frac, 1); 
  process_channel(&iIn10, &iOut10, iIndex, frac, 2); 
  process_channel(&iIn11, &iOut11, iIndex, frac, 3); 

  wcount = (wcount + 1) % MAX_ECHO_FRAMES;
}

void process_channel(const int* in, int* out, const unsigned int iIndex, const float frac, const int chan) {
  const float x = (float)(*in);
  const int x0 = buf[(iIndex) % MAX_ECHO_FRAMES][chan];
  const float x1 = buf[(iIndex+1) % MAX_ECHO_FRAMES][chan];
  const float y = x0 * (1.f- frac) + x1 * frac;
  buf[wcount][chan] = x + (y * feedback);
  *out = (int)( ((float)*out)*(1.f - mix) + (y * mix));
}
