#ifndef _ALEPH_BEES_OP_POOL_H_
#define _ALEPH_BEES_OP_POOL_H_

#include <stdlib.h>
#include <string.h>
#include "types.h"


#define SMALL_OP_SIZE 128
#define MAX_SMALL_OPS 256

#define BIG_OP_SIZE (1024 * 16)
#define MAX_BIG_OPS 16

u8* smallOpData;

typedef struct _smallOpCons {
  u8* head;
  struct _smallOpCons *tail;
} smallOpCons;

smallOpCons smallOpPool[MAX_SMALL_OPS];

smallOpCons *smallOpHead;

u8* bigOpData;

typedef struct _bigOpCons {
  u8* head;
  struct _bigOpCons *tail;
} bigOpCons;

bigOpCons bigOpPool[MAX_BIG_OPS];

bigOpCons *bigOpHead;

void initBigMemPool (void);
u8* allocBigOp(void);

void initSmallMemPool (void);
u8* allocSmallOp(void);


int freeOp(u8* region);

#endif
