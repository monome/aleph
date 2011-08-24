/* cube_data.h
 * cube module
 *
 * data structures for fast and slow memory
*/  

#include "../paramTables/gainTable24db.h"
#include "../common/types.h"
#include "../common/param.h"

#define CUBECO_GAIN_TABLE_SIZE 1024
#define CUBECO_RATIO_TABLE_SIZE 161

// TEST: make this a power of two always!
// 2**20 = 1048576, ~= 21 seconds
#define CUBECO_ECHO_FRAMES 1048576
#define CUBECO_ECHO_FRAMES_1 1048575

// data structure for cached variables
typedef struct { // __attribute__((__packed_b init_m_)) {
  // parameters
  Param preGain;
  Param distortion;
  Param postGain;
  Param echoTimeRatio;
  // Param echoTimeBase;
  Param echoMix;
  Param feedback;
  // other data
  unsigned long long wcount;
} cubecoData;


// data structure for variables that must live in external memory
typedef struct {
  // tables
  float gainTable24[CUBECO_GAIN_TABLE_SIZE];
  float gainTable6[CUBECO_GAIN_TABLE_SIZE];
  float gainTable0[CUBECO_GAIN_TABLE_SIZE];
  float ratioTable[CUBECO_RATIO_TABLE_SIZE];
  //  float echoBuf[CUBECO_ECHO_FRAMES][4];
  // fixme: interleave channels
  int echoBuf[CUBECO_ECHO_FRAMES];

} cubecoExtData;
