/* 
   net_hid.h

   HID device management routines for the bees control network.
   handles operator focus, routes handler/refresh requests

*/

#ifndef _ALEPH_BEES_NET_HID_H_
#define _ALEPH_BEES_NET_HID_H_

#include "types.h"

//-----------------------
//---- types
typedef void(*hid_handler_t)(void* op);

// abstract superclass for hid operators
// has event handler and focus flag
typedef struct _op_hid {
  // handler function, will connect to app event handler
  hid_handler_t handler;
  // pointer to operator subclass
  void* sub;
  // pointers to adjacent list elements
  struct _op_hid* next;
  struct _op_hid* prev;
} op_hid_t;

//-------------------------
//--- extern functions

// add op pointer after the current position
//extern void net_hid_list_add(op_hid_t* op);
extern void net_hid_list_push(op_hid_t* op);
// delete op pointer at current position, return next pointer in list
extern void net_hid_list_remove(op_hid_t* op);
// handle incoming hid packet
extern void net_handle_hid_packet(void); //u32 data);

#endif // h guard
