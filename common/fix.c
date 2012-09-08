#include "fix.h"

static inline char bsign (const int x) {
  return ( (x & 0x80000000) > 0 ) ;
}

static inline int binv (const int x) {
  return x ^ 0xffffffff;
}

static inline int babs (const int x) {
  return bsign(x) ? binv(x) - 1 : x;
}

void print_fix16(char* buf , fix16_t x) {
  // fixme: shouldn't really need these
  char bufHi[FIX_DIG_HI];
  char bufLo[FIX_DIG_LO];
  char * p;
  char sign;
  int y, i;

  sign = bsign(x);
  //  bufHi = p + 1;
  //bufLo  = bufHi + FIX_DIG_HI + 1;

  p = buf;

  if(sign) {
    // whole
    y = ( (x >> 16) ^ 0xffff ) & 0xffff ;
    itoa_whole(y, bufHi, FIX_DIG_HI);
    // fract
    y = (x ^ 0xffff) & 0xffff;
    itoa_fract(y, bufLo);
    *p = '-'; p++;
  } else {
    // whole
    y = x >> 16;
    itoa_whole(y, bufHi, FIX_DIG_HI);
    // fract
    y = x & 0xffff;
    itoa_fract(y, bufLo);
    *p = ' '; p++;
  }
  
  // fixme: shouldn't need to copy if pointers are set up correctly
  i = 0;
  while (i < FIX_DIG_HI) {
    *p = bufHi[i] ? bufHi[i] : '0';
    i++; p++;
  } 
  *p = '.'; p++;
  i = 0;
  while (i < FIX_DIG_LO) {
    *p = bufLo[i] ? bufLo[i] : ' '; 
    i++; p++;
  }
  
}

// format whole part
void itoa_whole(int val, char* buf, int len)
{
  const unsigned int radix = 10;

  char * p;       // pointer
  unsigned int a; // digit (remainder)
  unsigned int u; // unsigned value 
  char neg = bsign(val);

  p = buf + len - 1; // right justify; start at end

  if ( neg ) {
    len--;
    val = binv(val) + 1; // FIXME: this will wrap at 0xffffffff
  }


  u = (unsigned int)val;

  while(p >= buf) {
    if (u > 0) {
      a = u % radix;
      u /= radix;
      *p = '0' + a;
    } else {
      *p = ' '; 
    }
    p--;

  }
  if(neg) { *buf = '-'; }
}


void itoa_fract(int val, char* buf)
{
  const unsigned int places[FIX_DIG_LO] = {
    6553, 655, 65, 7
  };
  
  char* p;
  unsigned int a;        //every digit
  int i;
  char* b;            //start of the digit char
  //  char temp;
  unsigned int u;
  unsigned int mul;

  p = buf;
  b = p;

  u = (unsigned int)val;
  
  for(i=0; i<FIX_DIG_LO; i++) {
    mul = places[i];
    a = (u / mul);
    if (a > 9) { a = 9; }
    u -= (mul * a);
    //    u = u % mul;
    //     printf("\n     mul:%d , a: %d , u: %d ", mul, a, u);    
    *p++ = a + '0';
  } 
   //  *p = 0;
}
