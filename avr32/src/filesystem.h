#ifndef _ALEPH_AVR32_FILESYSTEM_H_
#define _ALEPH_AVR32_FILESYSTEM_H_

#include "types.h"
#include "fat_filelib.h"

#define FS_BUF_SIZE 512

//=====================================
//==== vars
// transfer-done flag
//extern volatile u8 fsEndTransfer;
// Local RAM buffer to store data  to/from the SD/MMC card
extern volatile U8 pdcaRxBuf[FS_BUF_SIZE];
extern volatile U8 pdcaTxBuf[FS_BUF_SIZE];
// PDCA rx and tx channels
extern volatile avr32_pdca_channel_t* pdcaRxChan;
extern volatile avr32_pdca_channel_t* pdcaTxChan;

//====================================
//==== funcs
extern int fat_init(void);

//PRGM SAMPLE TRANSFER PR
extern int bfin_sample_transfer(unsigned long sector, unsigned long bytes);

#endif  // h guard
