/*

  test some fast table-lookup ideas.

  proposing a modified value-delta method.

//////////////////////////////////

  standard value-delta algorithm: 
  store values y[] and deltas d[], where d[n] = y[n+1] = y[n]
  then output vlaue z(x) = y[i(x)] + f(x)d[x]
  i(x) = integer part,
  f(x) = fractional part.

  PROBLEM:
  16 fractional bits is not enough to represent a typical delta * fract product.
  implementing this naively, the fractional term is usually zero.

/////////////////////

alternative implementation:
  consider an unsigned N-bit input (x), to vaue/delta tables of 2^M size.
  then index into table i = x >> (N-M)
  these leaves fractional part f as the lower (N-M) bits of x, 
  normalized to the appropriate range for arithmetic operations.

  furthermore, i'd like to try storing, instead of d[n] defined above,
  d[n] = (y[n+1] - y[n]) / 2 ** (N-M),
  thus the linear interpolation can be accomplished with integer,
  instead of fractional, multiplication.

  the problem with this would arise, i think,
  from limitations of numerical representation at small values.
  hence this experiment.

  PROBLEM: 
  rounding of the divisor causes overshoot/undershoot and nasty jagged output.

  unforunately, it doesn't seem that 16 fractional bits is enough for adequate linear interpolation on e.g. amplitude tables. bah.

///////////////////////

case-specific:

ok, the real goal here is to make an efficient crossfade / grain envelope on blackfin.
bf instruction set has fast, packed 16-bit multiplies.
16 bits is enough resolution for amplitudes.
the problem as we have seen comes from the representation of the delta... 

a basic case would have just enough data points 
so that each delta is near or just below the threshold of perceptual discernment.
then we can simply divide the rate... 
wish there was a way to compute the the per-sample delta as each data point is reached.
seems too expensive though.

/////////////////////
 
*/

#include <math.h>
#include <stdio.h>

#include "fix.h"
//#include "fract32_emu.h"
#include "types.h"

// test with fractional multiplication
// #define FRACT_MUL_TEST
// test with integer multiplication
#define INT_MUL_TEST

// bits of input
#define IN_BITS 16
// index bits from input
#define INDEX_BITS 10
// fractional bits from input
#define FRACT_BITS (IN_BITS - INDEX_BITS)
// mask to get fractional value from input
#define FRACT_MASK 0x3f
// size of delta and value tables
#define TABLE_SIZE (1 << INDEX_BITS)


// float values
double fy[TABLE_SIZE];
// float deltas
double fd[TABLE_SIZE];
// fixed-point values
fix16 y[TABLE_SIZE];
// fixed point deltas
fix16 d[TABLE_SIZE];

//--- output files
// the tables themselves
FILE* floatValueOut;
FILE* floatDeltaOut;
FILE* fixValueOut;
FILE* fixDeltaOut;
// interpolation test curves
FILE* floatInterpOut;
FILE* fixInterpOut;



// lookup function
#ifdef FRACT_MUL_TEST
static fix16 look_fix(u16 in) {
  fix16 val;
  u16 idx = in >> FRACT_BITS;
  fix16 fract = in & FRACT_MASK;
  fix16 delta = d[idx];
  val = fix16_add(y[idx], fix16_mul(delta, fract));
  printf("\r\n look_fix, idx: 0x%08x, fract: 0x%08x, delta: 0x%08x, val: 0x%08x",
	 idx, fract, delta, val);
  return val;
}
#endif

#ifdef INT_MUL_TEST
static fix16 look_fix(u16 in) {
  u16 idx = in >> FRACT_BITS;
  fix16 fract = in & FRACT_MASK;
  return y[idx] + d[idx] * fract;
}
#endif

static double look_float(double in) {
  u32 idx = (u32)(in);
  float fract = in - idx;
  return fy[idx] + fd[idx] * fract;
}

int main(void) {
  u32 i; 
  double f = 0.0;
  double inc = M_PI / (double)TABLE_SIZE;
  u16 fix_in = 0;

  //--- open output files
  // the tables themselves
  floatValueOut = fopen("floatValueOut.txt", "w");
  floatDeltaOut = fopen("floatDeltaOut.txt", "w");
  fixValueOut = fopen("fixValueOut.txt", "w");
  fixDeltaOut = fopen("fixDeltaOut.txt", "w");
  // interpolation test
  floatInterpOut = fopen("floatInterpOut.txt", "w");
  fixInterpOut = fopen("fixInterpOut.txt", "w");

  //---- fill tables with a half-sine function

  // fill value tables
  for (i=0; i<TABLE_SIZE; ++i) {
    fy[i] = sin(f);
    y[i] = fix16_from_float(fy[i]);
    printf("\r\n angle: %f, value: %f", f, fy[i]);
    // out to file
    fprintf(floatValueOut, "%i\t%f\r\n", i, fy[i]);
    fprintf(fixValueOut, "%i\t%f\r\n", i, fix16_to_dbl(y[i]));
    
    f += inc;
  }

  // fill delta tables
  for (i=0; i<(TABLE_SIZE - 1); ++i) {
    fd[i] = fy[i+1] - fy[i];
#ifdef FRACT_MUL_TEST
    d[i] = fix16_from_dbl(fd[i]);
#endif
#ifdef INT_MUL_TEST
    d[i] = fix16_from_dbl(fd[i] / (1 << FRACT_BITS));
#endif
    // out to file
    fprintf(floatDeltaOut, "%i\t%f\r\n", i, fd[i]);
    fprintf(fixDeltaOut, "%i\t%f\r\n", i, fix16_to_dbl(d[i]));
  }

  // last delta should be unused
  fd[TABLE_SIZE - 1] = 0.0;
  d[TABLE_SIZE - 1] = 0;


  ///---- test interpolation

  //-- float
  f = 0;
  inc = (double)(1 << INDEX_BITS) / (double)(1 << IN_BITS);
  for(i=0; i<(1 << IN_BITS); ++i) {
    double val = look_float(f);
    fprintf(floatInterpOut, "%i\t%f\r\n", i, val);
    f += inc;
  }

  //-- fix
  for(i=0; i<(1 << IN_BITS); ++i) {
    fix16 val = look_fix(fix_in);
    fprintf(fixInterpOut, "%i\t%f\r\n", i, fix16_to_dbl(val));
    ++fix_in;
  }


  //---- close output files
  fclose( floatValueOut );
  fclose( floatDeltaOut );
  fclose( fixValueOut );
  fclose( fixDeltaOut );
  fclose( floatInterpOut );
  fclose( fixInterpOut );

 
}
