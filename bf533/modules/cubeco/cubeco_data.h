/* cube_data.h
 * cube module
 *
 * defines data struture for the "cube" processing module
 * this should include all the parameters and runtime data the module needs
 * cause it is going to just live at a specified location in the memory map (start of SDRAM) 
*/  

#include "../paramTables/gainTable24db.h"
#include "../common/types.h"
#include "../common/param.h"

#define CUBECO_GAIN_TABLE_SIZE 1024
#define CUBECO_RATIO_TABLE_SIZE 161
#define CUBECO_ECHO_FRAMES 2880000 // 1 minute at 48k 

typedef struct { // __attribute__((__packed_b init_m_)) {
  // parameters
  Param preGain;
  Param distortion;
  Param postGain;
  Param echoTimeRatio;
  // Param echoTimeMultiplier;
  Param echoMix;
  Param feedback;
  // tables
  float gainTable24[CUBECO_GAIN_TABLE_SIZE];
  float gainTable6[CUBECO_GAIN_TABLE_SIZE];
  float gainTable0[CUBECO_GAIN_TABLE_SIZE];
  float ratioTable[CUBECO_RATIO_TABLE_SIZE];
  // other data
  long unsigned int wcount;
  float echoBuf[CUBECO_ECHO_FRAMES][4];
} cubecoData;
