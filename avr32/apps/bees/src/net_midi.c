// asf
#include "print_funcs.h"

// bees
#include "net_midi.h"

//-----------------------------
//-- static variables

// list of midi operators
static op_midi_list_t ml = {
  .top = NULL,
  .cur = NULL,
  .num = 0
};

//--------------------------
//-----  extern functions

void net_midi_list_add(op_midi_t* op) {
  op_midi_t* saveNext;
  op_midi_t* savePrev;
  op_midi_t* saveCur;
  op_midi_t* curOp;
  if(ml.num == 0) {
    ml.cur = op;
    curOp = op;
    curOp->next = op;
    curOp->prev = op;
  } else {
    curOp = ml.cur;
    saveNext = curOp->next;
    saveCur = curOp;
    savePrev = curOp->prev;
    curOp->next = op;
    curOp = ml.cur = op;
    curOp->next = saveNext;
    curOp->prev = savePrev;
    (curOp->next)->prev = op;
    if( curOp->next == ml.top ) {
      ((op_midi_t*)(ml.top))->prev = ml.cur;
    }
    ml.cur = curOp->next;
  }
  ml.num += 1;
}


// delete op pointer at current position, return next pointer in list
op_midi_t* net_midi_list_remove(op_midi_t* op) {
  /// FIXME: WTF this is totally wrong... 
  /// assumes target op is current selection! grrah
  op_midi_t* saveNext;
  op_midi_t* savePrev;
  op_midi_t* curOp = (op_midi_t*)(ml.cur);
  if(curOp == ml.top) {
    ml.top = ((op_midi_t*)(curOp))->next;
  }
  saveNext = ((op_midi_t*)(curOp))->next;
  savePrev = ((op_midi_t*)(curOp))->prev;
  ml.cur = saveNext;
  curOp->prev = savePrev;
  (((op_midi_t*)(curOp))->prev)->next = curOp;
  ml.num = ml.num - 1;
  return (op_midi_t*)(ml.cur);
}

// handle incoming midi packet
void net_handle_midi_packet(u32 data) {
  u32 i;
  op_midi_t* op = ml.top;
  print_dbg("\r\n net_handle_midi_packet, num ops: ");
  print_dbg_ulong(ml.num);
  for(i=0; i < ml.num; ++i) {
    (*(op->handler))(op, data);
    op = op->next;
  }
}
