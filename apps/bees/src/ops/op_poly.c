#include "net_protected.h"
#include "op_poly.h"
#include "stddef.h"

//-------------------------------------------------
//----- static function declaration
static void op_poly_in_pitch(op_poly_t* poly, const io_t v);
static void op_poly_in_amp(op_poly_t* poly, const io_t v);
static void op_poly_in_off(op_poly_t* poly, const io_t v);
static void op_poly_in_mono(op_poly_t* poly, const io_t v);

// aux functions
static void init_voices(op_poly_t *poly);
static poly_voice_t *poly_find_used_pitch(op_poly_t *poly, io_t pitch);
static poly_voice_t *poly_find_prev(poly_voice_t *where, poly_voice_t *what);
static void poly_free_voice(op_poly_t *poly, poly_voice_t *v);
static poly_voice_t *poly_alloc_voice(op_poly_t *poly);

// pickle / unpickle
static u8* op_poly_pickle(op_poly_t* op, u8* dst);
static const u8* op_poly_unpickle(op_poly_t* op, const u8* src);


//-------------------------------------------------
//----- static vars
static op_in_fn op_poly_in_fn[4] = {
  (op_in_fn)&op_poly_in_pitch,
  (op_in_fn)&op_poly_in_amp,
  (op_in_fn)&op_poly_in_off,
  (op_in_fn)&op_poly_in_mono
};

static const char* op_poly_instring  = "PITCH\0  AMP\0    OFF\0    MONO\0   ";
static const char* op_poly_outstring = "IDX\0    PITCH\0  AMP\0    ";
static const char* op_poly_opstring  = "POLY";

//-------------------------------------------------
//----- external function definitions
void op_poly_init(void* mem) {
  op_poly_t* poly = (op_poly_t*)mem;
  poly->super.numInputs = 4;
  poly->super.numOutputs = 3;
  poly->outs[0] = -1;
  poly->outs[1] = -1;
  poly->outs[2] = -1;

  poly->super.in_fn = op_poly_in_fn;
  poly->super.pickle = (op_pickle_fn) (&op_poly_pickle);
  poly->super.unpickle = (op_unpickle_fn) (&op_poly_unpickle);

  poly->super.in_val = poly->in_val;
  poly->super.out = poly->outs;
  poly->super.opString = op_poly_opstring;
  poly->super.inString = op_poly_instring;
  poly->super.outString = op_poly_outstring;
  poly->super.type = eOpPoly;  

  poly->in_val[0] = &(poly->pitch);
  poly->in_val[1] = &(poly->amp);
  poly->in_val[2] = &(poly->off);
  poly->in_val[3] = &(poly->mono);

  poly->pitch = 0;
  poly->amp = 0;
  poly->off = 0;
  poly->mono = 0;

  init_voices(poly);
}

void init_voices(op_poly_t *poly) {
  poly->used_voices = NULL;
  poly->unused_voices = &(poly->voice_pool[0]);
  poly->voice_pool[0].idx = 0;
  int i;
  for(i=1; i < POLY_N_VOICES; i++) {
    poly->voice_pool[i-1].next = &(poly->voice_pool[i]);
    poly->voice_pool[i].idx = i;
    poly->voice_pool[i].pitch = 0;
  }
  poly->voice_pool[POLY_N_VOICES-1].next = NULL;
}

poly_voice_t *poly_alloc_voice(op_poly_t *poly) {
  poly_voice_t *ret = poly->unused_voices;
  if(ret) {
    poly->unused_voices = ret->next;
    ret->next = poly->used_voices;
    poly->used_voices = ret;
  }
  return ret;
}

poly_voice_t *poly_find_used_pitch(op_poly_t *poly, io_t pitch) {
  poly_voice_t *l = poly->used_voices;
  while(l) {
    if(l->pitch == pitch) {
      return l;
    }
    l = l->next;
  }
  return NULL;
}

poly_voice_t *poly_find_prev(poly_voice_t *where, poly_voice_t *what) {
  poly_voice_t *l = where;
  while(l) {
    if(l->next == what) {
      return l;
    }
    l = l->next;
  }
  return NULL;
}

void poly_free_voice(op_poly_t *poly, poly_voice_t *v) {
  if(v == poly->used_voices) {
    poly->used_voices = v->next;
    v->next = poly->unused_voices;
    poly->unused_voices = v;
  }
  else {
    poly_voice_t *prev = poly_find_prev(poly->used_voices, v);
    if(prev) {
      prev->next = v->next;
      v->next = poly->unused_voices;
      poly->unused_voices = v;
    }
  }
}

//-------------------------------------------------
//----- static function definitions
static void op_poly_in_pitch(op_poly_t* poly, const io_t pitch) {
  /* printf("\npitch: %d\n", v); */
  poly->pitch = pitch;
}

static void op_poly_in_amp(op_poly_t* poly, const io_t amp) {
  poly_voice_t *v;
  // note press
  if(amp != 0) {
    v = poly_alloc_voice(poly);
    if(v) {
      v->pitch = poly->pitch;
      /* printf("note press: idx = %d\n", v->idx); */
      if ( ! poly->mono ) {
	net_activate(poly, 0, v->idx);
      }
      net_activate(poly, 1, v->pitch);
      net_activate(poly, 2, amp);
    }
  }
  // note release
  else {
    /* printf("note release: %d, %d\n", amp, poly->pitch); */
    v = poly_find_used_pitch(poly, poly->pitch);
    if(v) {
      // monophonic note release
      if (poly->mono) {
	poly_free_voice(poly, v);
	if(poly->used_voices == NULL) {
	  net_activate(poly, 2, 0);
	}
	else {
	  net_activate(poly, 1, poly->used_voices->pitch);
	}
      }
      // polyphonic note release
      else {
	poly_free_voice(poly, v);
	net_activate(poly, 0, v->idx);
	net_activate(poly, 2, 0);
      }
    }
  }
}

static void op_poly_in_off(op_poly_t* poly, const io_t v) {
  int i;
  for(i=0; i < POLY_N_VOICES; i++) {
    net_activate(poly, 0, i);
    net_activate(poly, 2, 0);
  }
  init_voices(poly);
}

static void op_poly_in_mono(op_poly_t* poly, const io_t v) {
  /* printf("mono banged\n"); */
  if(v > 0) {
    poly->mono = 1;
  }
  else {
    poly->mono = 0;
  }
}


// pickle / unpickle
u8* op_poly_pickle(op_poly_t* op, u8* dst) {
  dst = pickle_io(op->pitch, dst);
  dst = pickle_io(op->amp, dst);
  dst = pickle_io(op->off, dst);
  dst = pickle_io(op->mono, dst);
  return dst;
}

const u8* op_poly_unpickle(op_poly_t* op, const u8* src ) {
  src = unpickle_io(src, &(op->pitch));
  src = unpickle_io(src, &(op->amp));
  src = unpickle_io(src, &(op->off));
  src = unpickle_io(src, &(op->mono));
  return src;
}
