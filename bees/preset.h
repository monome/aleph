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
// store all enabled inputs in current preset
void preset_store(U32 idx);
// store (and enable) a particular input
void preset_store_in(U32 preIdx, U32 inIdx);
// 
#endif
