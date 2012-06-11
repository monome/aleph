/* encoders.h
 * aleph
 */

#ifndef _ENCODERS_H_
#define _ENCODERS_H_

#include "compiler.h"

#define NUM_ENC    4
#define NUM_ENC_x2 8

// encoder pin array (see interrupt handler for usage)
extern const U16 encPins[NUM_ENC_x2];
// generate events from encoder interrupts
extern void handle_enc( const U8 encIdx, const U8 p0, const U8 p1 );
// intialize encoder positions
extern void init_encoders(void);
#endif // header guard
