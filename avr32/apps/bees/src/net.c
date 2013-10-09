/* net.c
 * bees
 * aleph
 *
 * definition of a network of control operators
 * and points of IO connection between them
 */

// std
#include <stdio.h>
#include <string.h>

// asf
#ifdef ARCH_AVR32
#include "print_funcs.h"
#include "delay.h"
#endif

// aleph-avr32
#include "control.h"

// bees
#include "util.h"
#include "op.h" 
#include "op_derived.h"
#include "memory.h"
#include "menu_protected.h"
#include "net.h"
#include "net_protected.h"
#include "param.h"
#include "play.h"
#include "types.h"
#include "util.h"


//=========================================
//===== variables

//----- static
// when unset, node activation will not propagate 
static u8 netActive = 0;

//---- external
ctlnet_t* net;

//===============================================
//========= static functions


// forward/backward search for input/output nodes
/* static s32 net_next_in(void); */
/* static s32 net_prev_in(void); */
/* static s32 net_next_out(void); */
/* static s32 net_prev_out(void); */
/* static void net_reset_in_search(void); */
/* static void net_reset_out_search(void); */

// create all system operators
static void add_sys_ops(void);
static void add_sys_ops(void) {
  // print_dbg("\r\n creating system operators.");
  // 4 encoders
  net_add_op(eOpEnc);
  net_add_op(eOpEnc);
  net_add_op(eOpEnc);
  net_add_op(eOpEnc);
  // 4 switches
  net_add_op(eOpSwitch);
  net_add_op(eOpSwitch);
  net_add_op(eOpSwitch);
  net_add_op(eOpSwitch);
  // 1 preset receiver
  //  net_add_op(eOpPreset);
}

//==================================================
//========= public functions

// initialize network at pre-allocated memory
void net_init(void) {
  u32 i;
  //  u32 res;
  
  //  net = &netPrivate;
  net = (ctlnet_t*)alloc_mem(sizeof(ctlnet_t));
  print_dbg("\r\n network address: 0x");
  print_dbg_hex((u32)net);

  for(i=0; i<NET_OP_POOL_SIZE; i++) {
    net->opPoolMem[i] = 0x00;
  }

  net->opPool = (void*)&(net->opPoolMem);
  net->opPoolOffset = 0;

  net->numOps = 0;
  net->numIns = 0;
  net->numOuts = 0;
  net->numParams = 0;
  // unassign all I/O nodes
  for(i=0; i<NET_INS_MAX; i++) {
    net_init_inode(i);
  }
  for(i=0; i<NET_OUTS_MAX; i++) {
    net_init_onode(i);
  }
  print_dbg("\r\n initialized ctlnet, byte count: ");
  print_dbg_hex(sizeof(ctlnet_t));
  add_sys_ops();
  netActive = 1;
}

// de-initialize network
void net_deinit(void) {
  u32 i;
  print_dbg("\r\n deinitializing network");
  for(i=0; i<net->numOps; i++) {
    op_deinit(net->ops[i]);
  }
}

// initialize an input node
void net_init_inode(u16 idx) {
  net->ins[idx].opIdx = -1;
  net->ins[idx].preset = 0;
}

// initialize an output node
void net_init_onode(u16 idx) {
  net->outs[idx].opIdx = -1;
  net->outs[idx].target = -1;
  net->outs[idx].preset = 0;
}

// activate an input node with a value
void net_activate(s16 inIdx, const io_t val, void* op) {
  /* print_dbg("\r\n net_activate, input idx: 0x"); */
  /* print_dbg_hex(inIdx); */
  /* print_dbg(", val: 0x"); */
  /* print_dbg_hex(val); */
  /* print_dbg(" , op idx: 0x"); */
  /* print_dbg_hex(net->ins[inIdx].opIdx); */
  /* print_dbg(" , op in idx: 0x"); */
  /* print_dbg_hex(net->ins[inIdx].opInIdx); */
  /* print_dbg(" , caller: 0x"); */
  /* print_dbg_hex((u32)op); */  
    
  if(!netActive) {
    if(op != NULL) {
      // if the net isn't active, dont respond to requests from operators
      print_dbg(" ... ignoring node activation from op.");
      return;
    }
  }

  if(inIdx >= 0) {
    play_input(inIdx);

    if(inIdx < net->numIns) {
      op_set_in_val(net->ops[net->ins[inIdx].opIdx],
		    net->ins[inIdx].opInIdx,
		    val);
    } else { 
      // index in dsp param list
      inIdx -= net->numIns;
      if (inIdx >= net->numParams) {
	return ;
      } else {
	set_param_value(inIdx, val);
      }
    }
  }  
}


// attempt to allocate a new operator from the static memory pool, return index
s16 net_add_op(op_id_t opId) {
  u16 ins, outs;
  u8 i;
  op_t* op;

  print_dbg("\r\n createing new operator, type: ");
  print_dbg_ulong(opId);

  if (net->numOps >= NET_OPS_MAX) {
    return -1;
  }

  if (op_registry[opId].size > NET_OP_POOL_SIZE - net->opPoolOffset) {
    print_dbg("\r\n op creation failed; out of memory.");
    return -1;
  }
  
  print_dbg("\r\n allocating memory location: 0x");
  print_dbg_hex((u32)(net->opPool + net->opPoolOffset));

  print_dbg("\r\n size of requested op size: 0x");
  print_dbg_hex(op_registry[opId].size);

  op = (op_t*)((u8*)net->opPool + net->opPoolOffset);
  // use the class ID to initialize a new object in scratch
  op_init(op, opId);

  ins = op->numInputs;
  outs = op->numOutputs;

  if (ins > (NET_INS_MAX - net->numIns)) {
    print_dbg("\r\n op creation failed; too many inputs in network.");
    return -1;
  }

  if (outs > (NET_OUTS_MAX - net->numOuts)) {
    print_dbg("\r\n op creation failed; too many outputs in network.");
    return -1;
  }

  // add op pointer to list
  net->ops[net->numOps] = op;
  net->opPoolOffset += op_registry[opId].size;

  //---- add inputs and outputs to node list
    for(i=0; i<ins; i++) {
      net->ins[net->numIns].opIdx = net->numOps;
      net->ins[net->numIns].opInIdx = i;
      net->numIns++;
    }
  for(i=0; i<outs; i++) {
    net->outs[net->numOuts].opIdx = net->numOps;
    net->outs[net->numOuts].opOutIdx = i;
    net->outs[net->numOuts].target = -1;
    net->numOuts++;
  }
  net->numOps++;
  return net->numOps - 1;
}

// destroy last operator created
s16 net_pop_op(void) {
  op_t* op = net->ops[net->numOps - 1];
  int i=0;
  int x=0;
  /* print_dbg("\r\n deleting op, addr : 0x"); */
  /* print_dbg_hex((u32)op); */
  /* print_dbg("; ins : "); */
  /* print_dbg_ulong(op->numInputs); */
  /* print_dbg("; outs : "); */
  /* print_dbg_ulong(op->numOutputs); */

  // de-init
  op_deinit(net->ops[net->numOps - 1]); 
  // store the global index of the first input
  x = net_op_in_idx(net->numOps - 1, 0); 
  // erase input nodes
  for(i=0; i<op->numInputs; i++) {
    net_init_inode(x++);
  }
  // store the global index of the first output
  x = net_op_out_idx(net->numOps - 1, 0);
  // erase output nodes
  for(i=0; i<op->numOutputs; i++) {
    net_init_onode(x++);
  }
  net->numIns -= op->numInputs;
  net->numOuts -= op->numOutputs;

  net->opPoolOffset += op_registry[op->type].size;
  net->numOps -= 1;
  return 0;

}

/// delete an arbitrary operator, and do horrible ugly management stuff
void net_remove_op(const u32 idx) {
  /// FIXME: network processing must be halted during this procedure!
  op_t* op = net->ops[idx];
  u8 nIns = op->numInputs;
  u8 nOuts = op->numOutputs;
  s16 firstIn, lastIn;
  s16 firstOut, lastOut;
  u32 opSize;
  u32 i;
  u8* pMem; // raw pointer to op pool memory

  opSize = op_registry[op->type].size;

  if ( nIns > 0 ) {
    /// find the first input idx
    firstIn = -1;
    for(i=0; i<net->numIns; i++) {
      if (net->ins[i].opIdx == idx) {
	firstIn = i;
	break;
      }
    }
    if(firstIn == -1 ) {
      // supposed to be a first inlet but we couldn't find it; we are fucked
      print_dbg("\r\n oh dang! couldn't find first inlet for deleted operator! \r\n");
    }
    lastIn = firstIn + nIns - 1;
    // check if anything connects here
    for(i=0; i<net->numIns; i++) {
      if( net->outs[i].target >= firstIn ) {
	if( net->outs[i].target > lastIn ) {
	  // connections to higher inlets get moved down
	  net->outs[i].target -= nIns;
	} else {
	  // disconnect from this op's inlets
	  net->outs[i].target =  -1;
	}
      }
    }
    // if higher input nodes are used...
    for(i=(lastIn + 1); i<net->numIns; i++) {
      /// revise op Idx
      net->ins[i].opIdx -= 1;
      /// copy all the data down
      net->ins[i - nIns] = net->ins[i];
      /// then erase
      net_init_inode(i);
    }
  }

  /// update output nodes for this op and higher
  firstOut = -1;
  lastOut = NET_OUTS_MAX;
  if ( nOuts > 0 ) {
    for(i=0; i<net->numOuts; i++) {
      if (net->outs[i].opIdx == idx) {
	if(firstOut == -1) {
	  firstOut = i;
	  lastOut = firstOut + nOuts - 1;
	}
	// deleted op owns this outlet, so erase it
	net_init_onode(i);
      }
      if( i > lastOut ) {
	/// for onodes above...
	/// revise op Idx
	net->outs[i].opIdx -= 1;
	/// copy all the data down
	net->outs[i - nOuts] = net->outs[i];
	/// then erase
	net_init_onode(i);
      }
    }
  }

  //// VERY DANGEROUSly move all the op memory above this, byte by byte
  for(pMem = (u8*)op + opSize; (u32)pMem<((u32)(net->opPool) + net->opPoolOffset); pMem++) {
    *((u8*)(pMem - opSize)) = *((u8*)pMem);
  }
  /// move the memory offset back
  net->opPoolOffset -= opSize;
  /// update node and op counts
  net->numIns -= nIns;
  net->numOuts -= nOuts;
  net->numOps -= 1;
  //... and, uh, don't crash
}


// create a connection between given idx pairs
void net_connect(u32 oIdx, u32 iIdx) {
  net->ops[net->outs[oIdx].opIdx]->out[net->outs[oIdx].opOutIdx] = iIdx;

  /* print_dbg("\r\n net_connect, output idx: 0x"); */
  /* print_dbg_hex(oIdx); */
  /* print_dbg(", in idx: 0x"); */
  /* print_dbg_hex(iIdx); */
  /* print_dbg(" , op idx: 0x"); */
  /* print_dbg_hex(net->outs[oIdx].opIdx); */
  /* print_dbg(" , op out idx: 0x"); */
  /* print_dbg_hex(net->outs[oIdx].opOutIdx); */

  /* print_dbg("\r\n output operator name: "); */
  /* print_dbg(net_op_name( net->outs[oIdx].opIdx) ); */
  /* print_dbg("\r\n output name: "); */
  /* print_dbg(net_out_name(oIdx)); */

  /* print_dbg("\r\n input name: "); */
  /* print_dbg(net_in_name(iIdx)); */

  net->outs[oIdx].target = iIdx;
}

// disconnect given output
void net_disconnect(u32 outIdx) {
  net->ops[net->outs[outIdx].opIdx]->out[net->outs[outIdx].opOutIdx] = -1;
  net->outs[outIdx].target = -1;
}

//---- queries
// get current count of operators
u16 net_num_ops(void) {
  return net->numOps;
}

// get current count of inputs
u16 net_num_ins(void) {
  return net->numIns + net->numParams;
}

// get current count of outputs
u16 net_num_outs(void) {
  return net->numOuts;
}

// get num params (subset of inputs)
u16 net_num_params(void) {
  return net->numParams;
}

// get param index given idx in (input + params)
s16 net_param_idx(u16 inIdx) {
  return inIdx - net->numIns;
}

// get string for operator at given idx
const char* net_op_name(const s16 idx) {
  //  print_dbg("\r\n get op string, idx: ");
  //  print_dbg_ulong(idx);
  //  print_dbg("\r\n op addr: @0x: ");
  //  print_dbg_hex( &(net->ops[idx] ) );
  if (idx < 0) {
    return "";
  }

  return net->ops[idx]->opString;
}

// get name for input at given idx
const char* net_in_name(u16 idx) {
  if (idx >= net->numIns) {
    // not an operator input
    idx -= net->numIns;
    if (idx >= net->numParams) {
      // not a param input either, whoops
      return "";
    } else {
      // param input
      return net->params[idx].desc.label;
    }
  } else {
    // op input
    return op_in_name(net->ops[net->ins[idx].opIdx], net->ins[idx].opInIdx);
  }
}

// get name for output at given idx
const char* net_out_name(const u16 idx) {
  return op_out_name(net->ops[net->outs[idx].opIdx], net->outs[idx].opOutIdx);
}

// get op index for input at given idx
s16 net_in_op_idx(const u16 idx) {
  if (idx >= net->numIns) return -1;
  return net->ins[idx].opIdx;
}

// get op index for output at given idx
s16 net_out_op_idx(const u16 idx) {
  if (idx > net->numOuts) return -1;
  return net->outs[idx].opIdx;
}

// get global index for a given input of given op
u16 net_op_in_idx(const u16 opIdx, const u16 inIdx) {
  u16 which;
  for(which=0; which < (net->numIns); which++) {
    if(net->ins[which].opIdx == opIdx) {
      return (which + inIdx);
    }
  }
  return 0; // shouldn't get here
}

// get global index for a given output of given op
u16 net_op_out_idx(const u16 opIdx, const u16 outIdx) {
  u16 which;
  for(which=0; which<net->numOuts; which++) {
    if(net->outs[which].opIdx == opIdx) {
      return (which + outIdx);
    }
  }
  return 0; // shouldn't get here
}

// get connection index for output
s16 net_get_target(u16 outIdx) {
  return net->outs[outIdx].target;
}

// is this input connected to anything?
u8 net_in_connected(s32 iIdx) {
  u8 f=0;
  u16 i;
  for(i=0; i<net->numOuts; i++) {
    if(net->outs[i].target == iIdx) {
      f = 1;
      break;
    }
  }
  return f;
}

// get status (user/system) of op at given idx
/* opStatus_t net_op_status(u16 opIdx) { */
/*   return net->ops[opIdx]->status; */
/* } */

u8 net_op_flag(const u16 opIdx, op_flag_t flag) {
  return net->ops[opIdx]->flags & (1 << flag);
}

// populate an array with indices of all connected outputs for a given index
// returns count of connections
u32 net_gather(s32 iIdx, u32(*outs)[NET_OUTS_MAX]) {
  u32 iTest;
  u32 iOut=0;
  for(iTest=0; iTest<NET_OUTS_MAX; iTest++) {
    if(net->outs[iTest].target == iIdx) {
      (*outs)[iOut] = iTest;
      iOut++;
    }
  }
  return iOut;
}

//--- get / set / increment input value
io_t net_get_in_value(s32 inIdx) {
  if(inIdx < 0) {
    return 0;
  }
  if (inIdx >= net->numIns) {
    inIdx -= net->numIns;
    return get_param_value(inIdx);
  } else {
    return op_get_in_val(net->ops[net->ins[inIdx].opIdx], net->ins[inIdx].opInIdx);
  }
}

void net_set_in_value(s32 inIdx, io_t val) {
  if (inIdx < 0) return;
  if (inIdx < net->numIns) {
    op_set_in_val(net->ops[net->ins[inIdx].opIdx], net->ins[inIdx].opInIdx, val);
  } else {
    // parameter
    inIdx -= net->numIns;
    set_param_value(inIdx, val);
  }
}

io_t net_inc_in_value(s32 inIdx, io_t inc) {
  op_t* op;
  if (inIdx >= net->numIns) {
    inIdx -= net->numIns;
    set_param_value(inIdx, OP_ADD(get_param_value(inIdx), inc));
    return get_param_value(inIdx);
  } else {
    op = net->ops[net->ins[inIdx].opIdx];
    (*(op->inc_func))(op, net->ins[inIdx].opInIdx, inc);
    return net_get_in_value(inIdx);
  }
}

// toggle preset inclusion for input
u8 net_toggle_in_preset(u32 id) {
  net->ins[id].preset ^= 1;
  return net->ins[id].preset;
}

// toggle preset inclusion for output
u8 net_toggle_out_preset(u32 id) {
  net->outs[id].preset ^= 1;
  return net->outs[id].preset;
}

// set preset inclusion for input
void net_set_in_preset(u32 id, u8 val) {
  net->ins[id].preset = val;
}

// set preset inclusion for output
void net_set_out_preset(u32 outIdx, u8 val) {
  net->outs[outIdx].preset = val;
}

// get preset inclusion for input
u8 net_get_in_preset(u32 id) {
  return net->ins[id].preset;
}

// get preset inclusion for output
u8 net_get_out_preset(u32 id) {
  return net->outs[id].preset;
}

//------------------------------------
//------ params

// add a new parameter
void net_add_param(u32 idx, volatile ParamDesc * pdesc) {
  memcpy( &(net->params[net->numParams].desc), (const void*)pdesc, sizeof(ParamDesc) );
  net->params[net->numParams].idx = idx; 
  net->params[net->numParams].preset = 1; 
  net->params[net->numParams].data.value.asInt = pdesc->min;
  net->numParams++;
}

// clear existing parameters
void net_clear_params(void) {
  net->numParams = 0;
}

// resend existing parameter values
void net_send_params(void) {
  u32 i;
  for(i=0; i<net->numParams; i++) {
    ctl_param_change(i, net->params[i].data.value.asInt);
  }
}

// retrigger all inputs
void net_retrigger_inputs(void) {
  u32 i;
  netActive = 0;
  for(i=0; i<net->numIns; i++) {
    net_activate(i, net_get_in_value(i), NULL);
  }
  netActive = 1;
}

//---------------------------------------------------
//----- static
// forward/backward search for input/output nodes
/* s32 net_next_in(void) { */
/*   while( inSearchIdx < NET_INS_MAX ) { */
/*     if(net->ins[inSearchIdx].opIdx >= 0) { */
/*       return inSearchIdx; */
/*     } */
/*     inSearchIdx++; */
/*  }  */
/*   return -1; // no inputs */
/* } */

/* s32 net_prev_in(void) { */
/*   while( inSearchIdx >=0 ) { */
/*     if(net->ins[inSearchIdx].opIdx >= 0) { */
/*       return inSearchIdx; */
/*     } */
/*     inSearchIdx--; */
/*   } */
/*   return -1; // no inputs */
/* } */

/* s32 net_next_out(void) { */
/*   while( outSearchIdx < NET_OUTS_MAX ) { */
/*     if(net->outs[outSearchIdx].opIdx >= 0) { */
/*       return outSearchIdx; */
/*     } */
/*     outSearchIdx++; */
/*   } */
/*   return -1; // no outputs */
/* } */

/* s32 net_prev_out(void) { */
/*   while( outSearchIdx >=0 ) { */
/*     if(net->outs[outSearchIdx].opIdx >= 0) { */
/*       return outSearchIdx; */
/*     } */
/*     outSearchIdx--; */
/*   } */
/*   return -1; // no outputs */
/* } */

/* void net_reset_in_search(void) { */
/*   inSearchIdx = 0; */
/* } */

/* void net_reset_out_search(void) { */
/*   outSearchIdx = 0; */
/* } */
