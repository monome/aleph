// asf
#include "print_funcs.h"
#include "hid.h"
// bees
#include "net_hid.h"


//--------------
//--- types

// list structure
typedef struct _op_hid_list {
  op_hid_t* top;
  op_hid_t* cur;
  u32 num;
} op_hid_list_t;


//-----------------------------
//-- static variables

// list of hid operators
static op_hid_list_t hl = {
  .top = NULL,
  .cur = NULL,
  .num = 0
};

//--------------------------
//-----  extern functions

// push an op to the top of the list
void net_hid_list_push(op_hid_t* op) {
  /// sanity check: loop over list to make sure op isn't already in it.
  int i;
  op_hid_t* p = hl.top;
  for(i=0; i<hl.num; ++i) {
    if(op == p) {
      // already in list, so bail
      return;
    }
    p = p->next;
  }
  ////
  if(hl.num == 0) {
    hl.top = op;
    op->prev = op;
    op->next = op;
  } else {
    op->prev = (hl.top)->prev;
    op->next = hl.top;
    ((hl.top)->prev)->next = op;
    hl.top->prev = op;
    hl.top = op;
  }
  hl.num += 1;
}


// remove an operator
void net_hid_list_remove(op_hid_t* op) {
    // FIXME: should sanity-check that op is actually in the list
  if(hl.num == 1) {
    hl.top = NULL;
    hl.num = 0;
  } else {
    if(op->prev == NULL || op->next == NULL) {
      print_dbg("\r\n error unlinking hid operator");
      return;
    }
    op->prev->next = op->next;
    op->next->prev = op->prev;
    op->next = NULL;
    op->prev = NULL;
    hl.num -= 1;
  }

}

// handle incoming hid packet
void net_handle_hid_packet() { //u32 data) {
  u32 i;
  op_hid_t* op = hl.top;
  //  print_dbg("\r\n net_handle_hid_packet, num ops: ");
  // print_dbg_ulong(hl.num);
  //  print_dbg("\r\n bees: hid handler. ");
  //  print_dbg(" ; dirty: 0x");
  //  print_dbg_hex(hid_get_frame_dirty());
  //  print_dbg_hex(data)
  
  for(i=0; i < hl.num; ++i) {
    (*(op->handler))(op);
		     //		     hid_get_frame_data(), 
		     //		     hid_get_frame_dirty(),
		     //		     hid_get_frame_size() );
    op = op->next;
  }

  // clear the dirty fields
  hid_clear_frame_dirty();

}
