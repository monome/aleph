/*! module.h
 
  static base class for audio modules.

 */


#ifndef _MODULE_H_
#define _MODULE_H_

#include "audio.h"

//! initialize the module
extern void module_init(void);
//! process a block of samples
extern void module_process_block(buffer_t *inChannels, buffer_t *outChannels,
				 const u8 numChannels, const u8 numFrames);

#endif
