#ifndef _ENV_H_
#define _ENV_H_

// aleph/common
#include "fix.h"

extern void* sdram;



//---- 1 style: a counter into a buffer for each envelope shape

typedef struct _env { 
  
} env;

// precalculate at module init and store shape bufs in slow memory


// load a shape from slow to fast memory on param change




//---- OR: exponent synthesis, recalc at param change or segment

#endif // header guard
