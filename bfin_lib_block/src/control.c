#include "control.h"
#include "gpio.h"
#include "module.h"

#define CONTROL_Q_SIZE 64

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
  for(i=0; i<qCount; i++) {
    idx = q[qIdxRd].idx;
    val = q[qIdxRd].val;
    gModuleData->paramData[idx].value = val;
    module_set_param(idx, val);
    qIdxRd = (qIdxRd + 1) & (CONTROL_Q_SIZE - 1);
  }
  qCount = 0;
}
