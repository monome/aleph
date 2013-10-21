#ifndef _ALEPH_BFIN_CORE_H_
#define _ALEPH_BFIN_CORE_H_ 

#include <sys/exception.h>
#include <cdefBF533.h>
#include "ccblkfn.h"
#include "sysreg.h"

#include "types.h"

#define DAC_RESET_PIN 6
#define LED0_PIN 14
#define LED1_PIN 15
//#define DAC_LDAC_PIN 7 // not used


/// data buffer for sport1 tx
/// we want 3x24 words, so need 9 single bytes
/// make this a union so that we can properly access the data words
//typedef union { u8 bytes[9]; struct { u8 com; u16 dat; } dacs[3]; } txbuf_t;
//extern volatile txbuf_t sport1TxBuf;
extern volatile u32 txBuf;
//// test
//extern volatile u8 testTxBuf[4];

// which channel to update on next TX interrupt
extern u8 upCh;

#endif // header guard
