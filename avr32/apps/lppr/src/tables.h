#ifndef _ALEPH_AVR32_APP_LPPR_TABLES_H_
#define _ALEPH_AVR32_APP_LPPR_TABLES_H_

#include "fix.h"
#include "types.h"

// 24-but input
#define TABLE_IN_MAX 0xfffff
// size of each table : 4k
#define TABLE_LEN 0x1000

// initialize tables
extern void tables_init(void);

// get amplitude
extern fract32 lookup_amp(u32 in);
// get db
extern fix16 lookup_db(u32 in);

// get amplitude
extern fract32 lookup_svf_coeff(u32 in);
// get db
extern fix16 lookup_hz(u32 in);

#endif
