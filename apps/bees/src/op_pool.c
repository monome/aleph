#include <stdlib.h>
#include <string.h>
#include "op_pool.h"
#include "print_funcs.h"
#include "memory.h"


void initBigMemPool (void) {
  bigOpData = (u8*)alloc_mem(BIG_OP_SIZE * MAX_BIG_OPS);
  int i;
  for(i=0; i < MAX_BIG_OPS-1; i++) {
    bigOpPool[i].head = bigOpData + i * BIG_OP_SIZE;
    bigOpPool[i].tail = &(bigOpPool[i+1]);
  }
  bigOpPool[MAX_BIG_OPS-1].head = NULL;
  bigOpHead = bigOpPool;
}

u8* allocBigOp(void) {
  if (bigOpHead == NULL) {
    print_dbg("all out of memory\n");
    return NULL;
  }
  u8* region = bigOpHead->head;
  bigOpHead = bigOpHead->tail;
  return region;
}

int freeBigOp(u8* region) {
  int idx = region - bigOpData;
  if(idx % BIG_OP_SIZE !=0) {
    print_dbg("Warning non-snapping chunk pointer (idx = ");
    print_dbg_ulong(idx);
    print_dbg(") passed to freeBigOp\n");
    return -1;
  }
  idx = idx / BIG_OP_SIZE;
  if (idx >= MAX_BIG_OPS || idx < 0) {
    print_dbg("Warning out-of-range chunk pointer passed to freeBigOp\n");
    return -1;
  }
  
  bigOpPool[idx].tail = bigOpHead;
  bigOpHead = &(bigOpPool[idx]);
  return 0;
}


void initSmallMemPool (void) {
  smallOpData = (u8*)alloc_mem(SMALL_OP_SIZE * MAX_SMALL_OPS);
  int i;
  for(i=0; i < MAX_SMALL_OPS-1; i++) {
    smallOpPool[i].head = smallOpData + i * SMALL_OP_SIZE;
    smallOpPool[i].tail = &(smallOpPool[i+1]);
  }
  smallOpPool[MAX_SMALL_OPS-1].head = NULL;
  smallOpHead = smallOpPool;
}

u8* allocSmallOp(void) {
  if (smallOpHead == NULL) {
    print_dbg("all out of memory\n");
    return NULL;
  }
  u8* region = smallOpHead->head;
  smallOpHead = smallOpHead->tail;
  return region;
}

int freeSmallOp(u8* region) {
  int idx = region - smallOpData;
  if(idx % SMALL_OP_SIZE !=0) {
    print_dbg("Warning non-snapping chunk pointer (idx = ");
    print_dbg_ulong(idx);
    print_dbg(") passed to freeSmallOp\n");
    return -1;
  }
  idx = idx / SMALL_OP_SIZE;
  if (idx >= MAX_SMALL_OPS || idx < 0) {
    print_dbg("Warning out-of-range chunk pointer passed to freeSmallOp\n");
    return -1;
  }
  
  smallOpPool[idx].tail = smallOpHead;
  smallOpHead = &(smallOpPool[idx]);
  return 0;
}

int freeOp (u8* region) {
  int ret = freeSmallOp(region);
  if (ret != -1) {
    return ret;
  }
  ret = freeBigOp(region);
  if (ret != -1) {
    return ret;
  }
  free_mem(region);
  return 0;
}
