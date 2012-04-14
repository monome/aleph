/* preset.h
 * bees
 * aleph
*/

#ifndef _PRESET_H_
#define _PRESET_H_

#define NUM_PRESETS 100

// intialize
void preset_init(void);
// de-initialize
void preset_deinit(void);
// store (and enable) a particular input
void preset_store_in(U32 preIdx, U32 inIdx);
// store (and enable) a particular output
void preset_store_out(U32 preIdx, U32 outIdx);
// store everything enabled in given preset
void preset_store(U32 preIdx);
// recall everything enabled in given preset
void preset_recall(U32 preIdx);

#endif
