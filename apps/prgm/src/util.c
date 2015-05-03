//util.c
//aleph-prgm-avr32

#include "util.h"

s32 swap_s32 (s32 val) {
    //load union with u8 trigs
    //            ctl_param_change(eParamTrig, (1 >> 24) & 0xff);
    //            ctl_param_change(eParamTrig, (1 >> 16) & 0xff);
    //            ctl_param_change(eParamTrig, (1 >> 8) & 0xff);
    //            ctl_param_change(eParamTrig, 1 & 0xff);
    
    //byteswap union
    return ((val << 24) & 0xff000000) | ((val << 8) & 0x00ff0000) | ((val >> 8) & 0x0000ff00) | ((val >> 24) & 0x000000ff);
    
    //    p = data->trig[c];
    //    env_tcd_set_trig(&(track[0]->envAmp), (p >> 24) & 0xff);
    //    env_tcd_set_trig(&(track[1]->envAmp), (p >> 16) & 0xff);
    //    env_tcd_set_trig(&(track[2]->envAmp), (p >> 8) & 0xff);
    //    env_tcd_set_trig(&(track[3]->envAmp), p & 0xff);

    //    return ((val&0xff)<<24)+((val&0xff00)<<8)+((val&0xff0000)>>8)+((val>>24)&0xff);
}

void parameter_print(char* buf, u16 x) { itoa_whole((s32)(x), (buf), 6); }
static char hexCharLc[16] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f' };
static char hexCharUc[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };


typedef union {
  u32 ui;
  u8 b[4];
} uword_t ;


char* byte_to_hex_ascii(char* dst, u8 byte) {
  *dst = hexCharLc[byte >> 4];
  dst++;
  *dst = hexCharLc[byte & 0xf]; 
  dst++;
  return dst;
}


char* hex_ascii_to_byte(u8* dst, char* src) {
  u8 i;
  for(i=0; i<16; i++) {
    if(*src == hexCharUc[i] || *src == hexCharLc[i]) {
      *dst = i << 4;
    }
  }
  src++;
  for(i=0; i<16; i++) {
    if( *src == hexCharUc[i] || *src == hexCharLc[i] ) {
      *dst |= i;
    }
  } 
  return ++src;
}


void uint_to_hex_ascii(char* dst, u32 src) {
  uword_t x = { .ui = src };
  char* p = dst;
  // big-endian
  p = byte_to_hex_ascii(p, x.b[0]);
  p = byte_to_hex_ascii(p, x.b[1]);
  p = byte_to_hex_ascii(p, x.b[2]);
  p = byte_to_hex_ascii(p, x.b[3]);
}


u32 hex_ascii_to_uint(char* str) {
  uword_t x = {.ui = 0x00000000 };
  char *p = str;
  // big-endian
  p = hex_ascii_to_byte(&(x.b[0]), p);
  p = hex_ascii_to_byte(&(x.b[1]), p);
  p = hex_ascii_to_byte(&(x.b[2]), p);
  p = hex_ascii_to_byte(&(x.b[3]), p);
  return x.ui;
}


f32 fr32_to_float(fract32 x) {
    if(x > 0)
    {
        return (f32)((f64)x / (f64)INT_MAX);
    }
    else
    {
        return (f32)((f64)x / (f64)INT_MIN * -1.0);
    }
}


fract32 float_to_fr32(f32 x) {
    if(x < 0.f)
    {
        if(x < -1.f)
        {
            return INT_MIN;
        }
        else
        {
            return (fract32)( ( (f64)(INT_MAX) + 1 ) * (f64)x);
        }
    }
    else
    {
        if(x > 1.f)
        {
            return INT_MAX;
        }
        else
        {
            return (fract32)((f64)INT_MAX * (f64)x);
        }
    }
}


fract32 add_fr1x32(fract32 _x, fract32 _y) {
    return _x + _y;
}


fract32 sub_fr1x32(fract32 _x, fract32 _y) {
    return _x - _y;
}


fract32 mult_fr1x32x32(fract32 _x, fract32 _y) {
    return (((s64)_x * (s64)_y)) >> 31;
}


fract32 negate_fr1x32(fract32 x) {
    if(x == INT_MIN)
    {
        return (x+1) ^ 0xffffffff;
    }
    else
    {
        return x ^ 0xffffffff;
    }
}
