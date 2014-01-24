/* encoders.h
 * aleph
 */

#ifndef _ENCODERS_H_
#define _ENCODERS_H_

//#include "compiler.h"
#include "event_types.h"
#include "types.h"

#define NUM_ENC    4

typedef struct _enc {
  // pin numbers
  U8 pin[2];
  // current position (p0 | (p1 << 1))
  U8 pos;
  // current value of accumulator
  S16 val;
  // threshold to set sensitivity
    S16 thresh;
  // event number to post on change
  etype event;
} enc_t;

// encoder pin array (see interrupt handler for usage)
extern enc_t enc[NUM_ENC];
// process inputs and determine direction of given encoder
extern void process_enc( const U8 enc );
// intialize encoder positions
extern void init_encoders(void);
// set threshold of encoder at given idx
extern void set_enc_thresh( const u8 idx, const u8 v );

#endif // header guard
