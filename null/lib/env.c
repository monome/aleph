#include "env.h"

// 1 style: a counter into a buffer for each envelope shape

// precalculate at module init and store shape bufs in slow memory

// load a shape from slow to fast memory on param change (interpolate?)
