//prgm
//aleph-avr32

#ifndef _ALEPH_APP_MIX_RENDER_H_
#define _ALEPH_APP_MIX_RENDER_H_

#include "font.h"
#include "region.h"

//BOOT
// render to scrolling boot buffer
extern void render_boot(char *str);


//PRGM
//external variables
extern region *headRegion;


//external functions
extern void render_init(void);

extern void render_startup(void);

extern void render_update(void);

#endif // h guard
