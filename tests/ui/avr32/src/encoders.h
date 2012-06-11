/* encoders.h
 * aleph
 */

#ifndef _ENCODERS_H_
#define _ENCODERS_H_

#include "compiler.h"

#define NUM_ENC    4

typedef struct _enc {
  // pin numbers
  U8 pin[2];
  // current position (p0 | (p1 << 1))
  U8 pos;
  // event number to post on change
  eEventType event;
} enc_t;

// encoder pin array (see interrupt handler for usage)
extern enc_t enc[NUM_ENC];
// generate events from encoder interrupts
extern void process_enc( const U8 encIdx );
// intialize encoder positions
extern void init_encoders(void);

#endif // header guard
