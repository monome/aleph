#ifndef _ALEPH_AVR32_FILESYSTEM_H_
#define _ALEPH_AVR32_FILESYSTEM_H_

#include "types.h"
#define FS_BUF_SIZE 512

extern volatile u8 fsEndTransfer;
extern volatile u8 pdcaRxBuf[FS_BUF_SIZE];

extern int fat_init(void);

#endif // h guard
