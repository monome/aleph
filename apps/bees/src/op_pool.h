#ifndef _ALEPH_BEES_OP_POOL_H_
#define _ALEPH_BEES_OP_POOL_H_

#include <stdlib.h>
#include <string.h>
#include "types.h"


#define SMALL_OP_SIZE 128
#define MAX_SMALL_OPS 256

#define BIG_OP_SIZE (1024 * 16)
#define MAX_BIG_OPS 8

void initBigMemPool (void);
u8* allocBigOp(void);

void initSmallMemPool (void);
u8* allocSmallOp(void);

int freeBigOp(u8* region);
int freeSmallOp(u8* region);
int freeOp(u8* region);

#endif
