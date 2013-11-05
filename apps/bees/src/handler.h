#ifndef _ALEPH_APP_BEES_HANDLER_H_
#define _ALEPH_APP_BEES_HANDLER_H_

#include "app.h"
#include "types.h"

// ui handlers  set_alt(val);3
//extern void bees_handler(event_t* ev);
extern void assign_bees_event_handlers(void);

extern s32 scale_knob_value(s32 val);
extern s32 scale_knob_value_small(s32 val);

#endif 
