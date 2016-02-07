#include "cycle_count_aleph.h"

#include "control.h"
#include "gpio.h"
#include "module.h"

// size of control change FIFO...
// FIXME: should this be module-defined?
// how to choose a good value?
#define CONTROL_Q_SIZE 64

// globally visible cycle count result
// volatile u64 controlCycleCount;
// temp
// static volatile u64 startCycleCount;

typedef struct {
  u32 idx;
  ParamValue val;
} change_t;

static change_t q[CONTROL_Q_SIZE];
static u16 qIdxWr = 0;
static u16 qIdxRd = 0;
static u16 qCount = 0;

void control_add(u32 idx, ParamValue val) {
  if(qCount >= CONTROL_Q_SIZE) {
    // queue is full! indicate with LED3
    LED3_LO; 
  } else {  
    q[qIdxWr].idx = idx;
    q[qIdxWr].val = val;
    qIdxWr = (qIdxWr + 1) & (CONTROL_Q_SIZE - 1);
    qCount++;
  }
   
}

void control_process(void) {
  u16 i;
  ParamValue val;
  u32 idx;

  //  START_CYCLE_COUNT(startCycleCount);
 
  for(i=0; i<qCount; i++) {
    idx = q[qIdxRd].idx;
    val = q[qIdxRd].val;
    module_set_param(idx, val);
    qIdxRd = (qIdxRd + 1) & (CONTROL_Q_SIZE - 1);
  }
  qCount = 0;

  //  STOP_CYCLE_COUNT(controlCycleCount, startCycleCount);
}
