/* switches.h
 * aleph
 */

#ifndef _SWITCHES_H_
#define _SWITCHES_H_

#ifdef __cplusplus
extern "C" {
#endif

//#include "compiler.h"
#include "event_types.h"

#define NUM_SW    8

// generate events from switch interrupts
extern void process_sw( const U8 swIdx );
// initialize switch debouncing
extern void init_switches(void);


#ifdef __cplusplus
}
#endif

#endif // header guard
