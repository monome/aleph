#ifndef _ALEPH_BFIN_CORE_H_
#define _ALEPH_BFIN_CORE_H_ 

#include <sys/exception.h>
#include <cdefBF533.h>
#include "ccblkfn.h"
#include "sysreg.h"

#include "types.h"

/// data buffer for sport1 tx
/// we want 3x24 words, so need 9 single bytes
/// make this a union so that we can properly access the data words
typedef union { u8 bytes[9]; struct { u8 com; u16 dat; } dacs[3]; } txbuf_t;
//extern volatile txbuf_t sport1TxBuf;
extern volatile u8 sport1TxBuf[9];

//// test
extern volatile u8 testTxBuf[4];

#endif // header guard
