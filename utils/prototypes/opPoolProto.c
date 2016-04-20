#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef char u8;
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

void initBigMemPool (void) {
  bigOpData = malloc(BIG_OP_SIZE * MAX_BIG_OPS);
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
    fprintf(stderr, "all out of memory\n");
    return NULL;
  }
  u8* region = bigOpHead->head;
  bigOpHead = bigOpHead->tail;
  return region;
}

int freeBigOp(u8* region) {
  int idx = region - bigOpData;
  if(idx % BIG_OP_SIZE !=0) {
    fprintf(stderr, "Warning non-snapping chunk pointer (idx = %d) passed to freeBigOp\n", idx);
    return -1;
  }
  idx = idx / BIG_OP_SIZE;
  if (idx >= MAX_BIG_OPS || idx < 0) {
    fprintf(stderr, "Warning out-of-range chunk pointer passed to freeBigOp\n");
    return -1;
  }
  
  bigOpPool[idx].tail = bigOpHead;
  bigOpHead = &(bigOpPool[idx]);
  return 0;
}


void initSmallMemPool (void) {
  smallOpData = malloc(SMALL_OP_SIZE * MAX_SMALL_OPS);
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
    fprintf(stderr, "all out of memory\n");
    return NULL;
  }
  u8* region = smallOpHead->head;
  smallOpHead = smallOpHead->tail;
  return region;
}

int freeSmallOp(u8* region) {
  int idx = region - smallOpData;
  if(idx % SMALL_OP_SIZE !=0) {
    fprintf(stderr, "Warning non-snapping chunk pointer (idx = %d) passed to freeSmallOp\n", idx);
    return -1;
  }
  idx = idx / SMALL_OP_SIZE;
  if (idx >= MAX_SMALL_OPS || idx < 0) {
    fprintf(stderr, "Warning out-of-range chunk pointer passed to freeSmallOp\n");
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
  return freeBigOp(region);
}

int main () {
  initSmallMemPool();
  int i;
  for(i=0;i < SMALL_OP_SIZE * MAX_SMALL_OPS; i++) {
    *smallOpData = 0;
  }

  u8* opOne = allocSmallOp();

  u8* opTwo = allocSmallOp();

  u8* opThree = allocSmallOp();
  
  strcpy(opOne, "foo");
  strcpy(opTwo, "bar");
  strcpy(opThree, "baz");

  printf("opOne: %s\n", opOne);
  printf("opTwo: %s\n", opTwo);
  printf("opThree: %s\n...\nfreeing opTwo:\n", opThree);

  printf("error status of freeSmallOp: %d\n", freeOp(opTwo));

  u8* opFour = allocSmallOp();
  strcpy(opFour, "qux");
  printf("opOne: %s\n", opOne);
  printf("opTwo: %s\n", opTwo);
  printf("opThree: %s\n", opThree);
  printf("opFour: %s\n", opFour);

  for(i=0; i < MAX_SMALL_OPS; i++) {
    allocSmallOp();
  }

  initBigMemPool();
  i;
  for(i=0;i < BIG_OP_SIZE * MAX_BIG_OPS; i++) {
    *bigOpData = 0;
  }

  opOne = allocBigOp();

  opTwo = allocBigOp();

  opThree = allocBigOp();
  
  strcpy(opOne, "foo");
  strcpy(opTwo, "bar");
  strcpy(opThree, "baz");

  printf("opOne: %s\n", opOne);
  printf("opTwo: %s\n", opTwo);
  printf("opThree: %s\n...\nfreeing opTwo:\n", opThree);

  printf("error status of freeBigOp: %d\n", freeOp(opTwo));

  opFour = allocBigOp();
  strcpy(opFour, "qux");
  printf("opOne: %s\n", opOne);
  printf("opTwo: %s\n", opTwo);
  printf("opThree: %s\n", opThree);
  printf("opFour: %s\n", opFour);

  for(i=0; i < MAX_BIG_OPS; i++) {
    allocBigOp();
  }

}
