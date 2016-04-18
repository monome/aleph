#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef char u8;
#define SMALL_OP_SIZE 128
#define MAX_SMALL_OPS 256

u8* smallOpData;

typedef struct _smallOpCons {
  u8* head;
  struct _smallOpCons *tail;
} smallOpCons;

smallOpCons smallOpPool[MAX_SMALL_OPS];

smallOpCons *smallOpHead;

void initMemPool (void) {
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

int main () {
  initMemPool();
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

  printf("error status of freeSmallOp: %d\n", freeSmallOp(opTwo));

  u8* opFour = allocSmallOp();
  strcpy(opFour, "qux");
  printf("opOne: %s\n", opOne);
  printf("opTwo: %s\n", opTwo);
  printf("opThree: %s\n", opThree);
  printf("opFour: %s\n", opFour);

  for(i=0; i < MAX_SMALL_OPS; i++) {
    allocSmallOp();
  }
}
