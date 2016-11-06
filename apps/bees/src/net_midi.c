// asf
#include "print_funcs.h"

// bees
#include "net_midi.h"


//--------------
//--- types

// list structure
typedef struct _op_midi_list {
  op_midi_t* top;
  //  op_midi_t* cur;
  u32 num;
} op_midi_list_t;


//-----------------------------
//-- static variables

// list of midi operators
static op_midi_list_t ml = {
  .top = NULL,
  //  .cur = NULL,
  .num = 0
};

//--------------------------
//-----  extern functions

// push an op to the top of the list
void net_midi_list_push(op_midi_t* op) {
  /// sanity check: loop over list to make sure op isn't already in it.
  print_dbg("\r\nentered net_midi_list_push 0x");
  print_dbg_hex(op);
  int i;
  op_midi_t* p = ml.top;
  for(i=0; i<ml.num; ++i) {
    if(op == p) {
      print_dbg("\r\nrequested push of midi op already in list - bailing");
      return;
    }
    p = p->next;
  }
  ////
  if(ml.num == 0) {
    ml.top = op;
    op->prev = op;
    op->next = op;
  } else {
    op->prev = (ml.top)->prev;
    op->next = ml.top;
    ((ml.top)->prev)->next = op;
    ml.top->prev = op;
    ml.top = op;
  }
  ml.num += 1;
  print_dbg("\r\nnet_midi_list_push exited normally");
}


// remove an operator
void net_midi_list_remove(op_midi_t* op) {
  print_dbg("\r\nentered net_midi_list_remove 0x");
  print_dbg_hex(op);

  int i;
  op_midi_t* p = ml.top;
  u8 opInList = 0;
  for(i=0; i<ml.num; ++i) {
    if(op == p) {
      opInList = 1;
    }
    p = p->next;
  }
  if(! opInList) {
    print_dbg("\r\nrequested deletion of midi op not in list - bailing");
    return;
  }

  if(ml.num == 1) {
    ml.top = NULL;
    ml.num = 0;
  } else {
    if(op->prev == NULL || op->next == NULL) {
      print_dbg("\r\n error unlinking midi operator");
      return;
    }
    op->prev->next = op->next;
    op->next->prev = op->prev;
    if(ml.top == op) {
      ml.top = op->next;
    }
    op->next = NULL;
    op->prev = NULL;
    ml.num -= 1;
    print_dbg("\r\nnet_midi_list_remove exited normally");

  }
}

// handle incoming midi packet
void net_handle_midi_packet(u32 data) {
  u32 i;
  op_midi_t* op = ml.top;
  //  print_dbg("\r\n net_handle_midi_packet, num ops: ");
  // print_dbg_ulong(ml.num);
  for(i=0; i < ml.num; ++i) {
    (*(op->handler))(op, data);
    op = op->next;
  }
}
