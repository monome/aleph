#include <math.h>
#include <stdio.h>

#include "fix.h"
#include "fract32_emu.h"
#include "types.h"


// input range
// 24 bits
#define IN_MAX 0xffff
// max [1)
#define FR32_MAX 0x7fffffff
// min [-1]
#define FR32_MIN 0x80000000

// table size
#define LEN 1024
// right shift input range -> index range from u32 -> index
#define RSH 6
#define STRIDE 64 // how many input values between indices
#define MASK 63 // stride - 1

// values
double fdata[LEN];
fract32 data[LEN];
// deltas. 
// should be table[n+1] - table[n] / STRIDE
double fdiff[LEN];
fract32 diff[LEN];

// test: lookup function:
fract32 look(u32 in) {
  u32 idx = in >> RSH;
  u32 mul = in & MASK;
  u32 val = data[idx] + (mul * diff[idx]);
  return (fract32)val;
}

//  convert double in [0, 1) to fract32
fract32 ffr(double f) {
  return (fract32)(f * (double)FR32_MAX);
}


//  convert fract32 to double in [0, 1)
double frf(fract32 fr) {
  return (double)fr / (double)FR32_MAX;
}

// test lookup with some arbitrarily spanned inputs 
void test_inputs(u32 n) {
  u32 inc = IN_MAX / n;
  u32 i = 0;
  fract32 val;
  while(i < IN_MAX) {
    val = look(i);
    printf("\r\n in: %d, out: 0x%8x (%f)", i, val, frf(val) );
    i += inc;
  }
  val = look(IN_MAX);
  printf("\r\n in: %d, out: 0x%8x (%f)", IN_MAX, val, frf(val) );
  printf("\r\n \r\n");
}

// main function
int main(void) {
  u32 i;
  double f, finc, fmax;

  finc = 1.0 / (double)LEN; 
  // fill values table
  for(i=0; i<LEN; i++) {
    f = (double)i * finc * M_PI * 0.5;
    fdata[i] = sin(f);
  }
  // max value (not actually stored)
  fmax = sin(M_PI * 0.5);
  // fill deltas table with floats
  for(i=0; i<(LEN - 1); i++) {
    fdiff[i] = (fdata[i+1] - fdata[i]) / (double)(STRIDE - 1);
  }
  fdiff[LEN-1] = (fmax - fdata[LEN-1]) / (double)(STRIDE - 1);
  // convert both to fract32
  for(i=0; i<LEN; i++) {
    data[i] = ffr(fdata[i]);
    diff[i] = ffr(fdiff[i]);
  }

  //// test: print
  for(i=0; i<LEN; i++) {
    printf("\r\n data %d : 0x%8x (%f)", i, data[i], fdata[i]);
    printf("\t diff %d : 0x%8x (%f)", i, diff[i], fdiff[i]);
  }
  printf("\r\n \r\n ");


  //// test with some inputs;
  test_inputs(50);
}
