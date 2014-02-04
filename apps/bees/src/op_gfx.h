/*
  op_gfx.h

  graphics operator superclass and management.
  
 */

#ifndef _ALEPH_BEES_OP_GFX_H_
#define _ALEPH_BEES_OP_GFX_H_

#include "types.h"

// global flag, set if op is focused.
extern u8 opPlay;


// operator enabled
extern void op_gfx_enable(void);
// operator disabled
extern void op_gfx_disable(void);

// abstract superclass for gfx operators
/// might not even need this.

/* typedef struct _op_gfx { */
/*   //... */
/* } op_gfx_t; */

#endif
