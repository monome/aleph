#ifndef _ALEPH_APP_BEES_HANDLER_H_
#define _ALEPH_APP_BEES_HANDLER_H_

#include "app.h"
#include "op_math.h"
#include "types.h"

// assign ui handlers  
extern void assign_bees_event_handlers(void);

extern io_t scale_knob_value(io_t val);
extern io_t scale_knob_value_fast(io_t val);
extern io_t scale_knob_value_small(io_t val);

#endif 
