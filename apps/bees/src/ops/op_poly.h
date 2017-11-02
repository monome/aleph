#ifndef _op_poly_H_
#define _op_poly_H_

#include "op.h"
#include "op_math.h"
#include "types.h"

typedef struct _poly_voice{
  io_t pitch;
  u8 idx;
  struct _poly_voice *next;
} poly_voice_t;

#define POLY_N_VOICES 4

//--- op_poly_t : polytiplication 
typedef struct op_poly_struct {
  op_t super;
  io_t val; 
  volatile io_t pitch;
  volatile io_t amp;
  volatile io_t off;
  volatile io_t mono;
  volatile io_t* in_val[4];
  op_out_t outs[3];
  poly_voice_t voice_pool[POLY_N_VOICES];
  poly_voice_t *used_voices;
  poly_voice_t *unused_voices;
  u8 active_voices;
} op_poly_t;

void op_poly_init(void* poly);


#endif // header guard
