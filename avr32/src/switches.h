/* switches.h
 * aleph
 */

#ifndef _SWITCHES_H_
#define _SWITCHES_H_

#include "compiler.h"
#include "event_types.h"

#define NUM_SW    5

// generate events from switch interrupts
extern void process_sw( const U8 swIdx );

#endif // header guard
