/*
  op_gfx.h

  graphics operator superclass and management.
  
 */

#ifndef _ALEPH_BEES_OP_GFX_H_
#define _ALEPH_BEES_OP_GFX_H_

#include "types.h"

// global flag, set if op is focused.
extern u8 opPlay;


// operator declares focus
extern void op_gfx_focus(void);
// operator releases focus
extern void op_gfx_unfocus(void);

// abstract superclass for gfx operators
/// might not even need this.

/* typedef struct _op_gfx { */
/*   //... */
/* } op_gfx_t; */

#endif
