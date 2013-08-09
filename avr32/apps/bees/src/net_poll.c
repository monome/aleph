/*
  net_poll.c
  bees
  aleph
  
  define routines for handling polled-operator queue.

 */

#include "net_poll.h"

// initialize 
extern void op_list_init(op_list_t* l) {
  l->top = NULL;
  l->cur = NULL;
  l->num = 0;
}

// add op pointer after the current position
extern void net_poll_list_add(op_list_t* l, op_poll_t* op) {
  op_poll_t* saveNext; 
  op_poll_t* savePrev;
  op_poll_t* saveCur;
  op_poll_t* curOp = (op_poll_t*)(l->cur);
  if(l->num == 0) {
    l->cur = op;
    curOp->next = l->cur;
    curOp->prev = l->cur;
    l->top = l->cur;
  } else {
    saveNext = curOp->next;
    saveCur = l->cur;
    savePrev = (((op_poll_t*)l->cur))->prev;
    curOp->next = op;
    l->cur = curOp->next;
    curOp->next = saveNext;
    curOp->prev = savePrev;
    (curOp->next)->prev = l->cur;
    if( curOp->next == l->top ) {
      ((op_poll_t*)(l->top))->prev = l->cur;
    }
    l->cur = curOp->next;
  }
  l->num++;
}

// delete op pointer at current position, return next pointer in list
extern op_poll_t* net_poll_list_remove(op_list_t* l, op_poll_t* op) {
  op_poll_t* saveNext;
  op_poll_t* savePrev;
  op_poll_t* curOp = (op_poll_t*)(l->cur);
  if(curOp == l->top) {
    l->top = ((op_poll_t*)(curOp))->next;
  }
  saveNext = ((op_poll_t*)(curOp))->next;
  savePrev = ((op_poll_t*)(curOp))->prev;
  l->cur = saveNext;
  curOp->prev = savePrev;
  (((op_poll_t*)(curOp))->prev)->next = curOp;
  l->num = l->num - 1;
  return (op_poll_t*)(l->cur);
}
