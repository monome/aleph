/* switches.h
 * aleph
 */

#ifndef _SWITCHES_H_
#define _SWITCHES_H_

#include "compiler.h"
#include "event_types.h"

/// debouncing constants
/* #define SW_POLL_MS   5 */
/* #define SW_PRESS_MS   10 */
/* #define SW_RELEASE_MS 100 */
/* #define SW_PRESS_TICKS 2 */
/* #define SW_RELEASE_TICKS 20 */

#define NUM_SW    8

// generate events from switch interrupts
extern void process_sw( const U8 swIdx );
// initialize switch debouncing
extern void init_switches(void);

/* // poll switch at given index */
/* extern void poll_sw(const u8 idx); */

#endif // header guard
