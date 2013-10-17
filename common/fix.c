/* fix.c
 * common
 * aleph
 *
 * additional routines for converting/printing fixed-point datatypes
 
 FIXME: the integer-conversion routines could use optimization.
 specifically, avoid modulus and refactor to use half as many divides.

here is a good blog post on the topic:

http://embeddedgurus.com/stack-overflow/2011/02/efficient-c-tip-13-use-the-modulus-operator-with-caution/
 
 */

#include "fix.h"

//// comon temp variables
static unsigned int a, u;
static unsigned long int sign;

// fixme: shouldn't really need these
static char bufHi[FIX_DIG_HI] = "     ";
static char bufLo[FIX_DIG_LO] = "    ";

// for fractional part of 16.16
static const unsigned int places[FIX_DIG_LO] = {
  6553, 655, 65, 7
};

// print 16.16
void print_fix16(char* buf , fix16_t x) {
  static char * p;
  // char sign;
  int y, i;
  sign = BIT_SIGN(x);
  p = buf;

  if(sign == 0)  {
    // whole
    y = x >> 16;
    itoa_whole(y, bufHi, FIX_DIG_HI);
    // fract
    y = x & 0xffff;
    itoa_fract(y, bufLo);
    *p = ' ';
    p++;
  } else {
    // whole
    y = ( (x >> 16) ^ 0xffff ) & 0xffff;
    itoa_whole(y, bufHi, FIX_DIG_HI);
    // fract
    y = (x ^ 0xffff) & 0xffff;
    itoa_fract(y, bufLo);
    // search for the negative sign
    *p = ' ';
    i = FIX_DIG_HI;
    while(--i>0) {
      if(bufHi[i] == ' ') {
	bufHi[i] = '-';
	break;
      }
      if(i == 1) {
	*p = '-';
      }
    }
    p++;
  } 
  // fixme: shouldn't need to copy if pointers are set up correctly
  i = 0;

  while (i < FIX_DIG_HI) {
    if(bufHi[i]) {
      *p = bufHi[i];
    } else {
      *p = ' ';
    }
    i++; p++;
  } 
  *p = '.'; p++;
  i = 0;
  while (i < FIX_DIG_LO) {
    *p = bufLo[i] ? bufLo[i] : ' '; 
    i++; p++;
  }
}
// format whole part, right justified
void itoa_whole(int val, char* buf, int len) {
  static char* p;
  p = buf + len - 1; // right justify; start at end
  if(val == 0) {
    *p = '0'; p--;
    while(p >= buf) {
      *p = ' ';
      p--;
    }
    return;
  }
  sign = BIT_SIGN(val);

  if ( sign ) {
    len--;
    val = BIT_INVERT(val) + 1; // FIXME: this will wrap at 0xffffffff
  }

  u = (unsigned int)val;

  while(p >= buf) {
    if (u > 0) {
      a = u % 10;
      u /= 10;
      *p = '0' + a;
    } else {
      *p = ' '; 
    }
    p--;

  }
  if(sign) { *buf = '-'; }
}


// format whole part, left justified, no length argument (!)
int itoa_whole_lj(int val, char* buf) {
  static char* p;
  char tmp;
  int i;
  int len = 0;
  
  if(val == 0) {
    *buf = '0';
    return 1;
  }

  sign = BIT_SIGN(val);
  p = buf;

  if ( sign ) {
    *p = '-';
    ++p;
    ++len;
    val = BIT_INVERT(val) + 1; // FIXME: this will wrap at 0xffffffff
  }

  u = (unsigned int)val;

  while (u > 0) {
    a = u % 10;
    u /= 10;
    *p = '0' + a;
    ++p;
    ++len;
  }

  /* print_dbg("\r\n printing int: "); */
  /* print_dbg_ulong(val); */
  /* print_dbg(" ; len: "); */
  /* print_dbg_ulong(len); */
  /* print_dbg(" ; buf: "); */
  /* print_dbg(buf); */
  
  //// FIXME
  /// ugh, swap digits
  if(sign) {
    for (i=1; i<len; i++) {
      tmp = buf[i];
      buf[i] = buf[len - i];
      buf[len - i + 1] = tmp;
    }
  } else {
    for (i=0; i<(len >>1); i++) {
      //      print_dbg("\r\n digit: ");
      //      print_dbg_ulong(i);
      tmp = *(buf + i);
      //      print_dbg(" ; tmp: ");
      //      print_dbg_char(tmp);
      //      print_dbg(" , swap with : ");
      //      print_dbg_char( *(buf + len - i - 1) );
      *(buf + i) = *(buf + len - i - 1);
      *(buf + len - i - 1) = tmp;
    }
  }
  //  print_dbg(" ; buf (swap): ");
  //  print_dbg(buf);

  return len;
}

void itoa_fract(int val, char* buf) {  
  static char* p;
  int i;
  unsigned int mul;

  p = buf;
  u = (unsigned int)val;
  
  for(i=0; i<FIX_DIG_LO; i++) {
    mul = places[i];
    a = (u / mul);
    if (a > 9) { a = 9; }
    u -= (mul * a);
    *p++ = a + '0';
  } 
}
