/* net.c
 * bees
 * aleph
 *
 * definition of a network of control operators
 * and points of IO connection between them
 */

// std
#include <stdio.h>
//#include <string.h>

// asf
#ifdef ARCH_AVR32
#include "print_funcs.h"
#endif

// aleph
#include "util.h"
#include "op.h" 
#include "op_derived.h"
#include "memory.h"
#include "menu_protected.h"
#include "net.h"
#include "net_protected.h"
#include "param.h"
#include "types.h"
#include "util.h"

//=========================================
//===== variables
//----- staticltn
//__attribute__((__section__(".bss_extram")))
//static ctlnet_t netPrivate;

static s32 inSearchIdx = 0;
static s32 outSearchIdx = 0;

//---- external
ctlnet_t* net;

//===============================================
//========= static functions

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

  /* print_dbg("\r\n cltnet address: "); */
  /* print_dbg_hex((unsigned long int)net); */

  /* print_dbg("\r\n test-write to cltnet, test value:  "); */
  /* print_dbg_hex(0xDEADBEEF); */
  /* *((u32*)net) = 0xDEADBEEF; */
  /* print_dbg("\r\n ; reading... result: "); */
  /* res = *((u32*)net); */
  /* print_dbg_hex(res); */
 
  for(i=0; i<NET_OP_POOL_SIZE; i++) {
    net->opPoolMem[i] = (u8)0;
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
  printf("initialized network, using %d bytes\n", (int)sizeof(ctlnet_t));
  add_sys_ops();
}

// de-initialize network
void net_deinit(void) {
  ;;
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
void net_activate(s16 inIdx, const io_t val) {
  /* print_dbg("\r\n net activate: "); */
  /* print_dbg_hex(inIdx); */
  /* print_dbg(", val: "); */
  /* print_dbg_hex(val); */
  /* print_dbg(" , op idx: "); */
  /* print_dbg_hex(net->ins[inIdx].opIdx); */
  /* print_dbg(" , op in idx: "); */
  /* print_dbg_hex(net->ins[inIdx].opInIdx); */

  //    param_feedback(inIdx, val);
  if(inIdx >= 0) {
    if(inIdx < net->numIns) {
      //(*(net->ins[inIdx].in))(net->ops[net->ins[inIdx].opIdx], val);
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


#if NET_USE_MALLOC
/* // allocate a new operator dynamically. */
/* // return index in op pointer array */
/* s16 net_add_op(opId_t opId) { */
/*   s32 opIdx = -1; */
/*   s32 inIdx = -1; */
/*   s16 outIdx = -1; */
/*   s32 inAssign[OP_INS_MAX]; */
/*   s32 outAssign[OP_OUTS_MAX]; */
/*   op_t* op; */
/*   s32 i, j; */

/*   // print_dbg("\r\n adding operator; type: "); */
/*   // print_dbg_ulong(opId); */

/*   if (net->numOps >= NET_OPS_MAX) { */
/*     // print_dbg("\r\n too many ops already; bail "); */
/*     return -1; */
/*   } */

/*   // search for an unused operator index */
/*   for(i=0; i<NET_OPS_MAX; i++) { */
/*     if( !(BITTEST(net->opsUsed, i)) ) { */
/*       opIdx = i; */
/*       break; */
/*     } */
/*   } */

/*   if(opIdx == -1) { */
/*     // print_dbg("\r\n out of operator slots; bail "); */
/*     return -1; */
/*   } */

/*   op = malloc(op_registry[opId].size); */
/*   if( op == NULL ) {  */
/*     // print_dbg("\r\n out of memory; bail "); */
/*     return -1; */
/*   } */

/*   // initialize allocated memory with requested operator subclass */
/*   op_init(op, opId); */

/*   //  search for available input nodes */
/*   for(i=0; i<op->numInputs; i++) { */
/*     // search for unused input idx */
/*     inIdx = -1; */
/*     for(j=0; j<NET_INS_MAX; j++) { */
/*       //if( !(BITTEST(net->insUsed, j)) ) { */
/*       if(net->ins[j].opIdx == -1) { */
/* 	inIdx = j; */
/* 	break; */
/*       } */
/*     } */
/*     if(inIdx == -1) { */
/*       // print_dbg("\r\n no free input slots; bail "); */
/*       return -1; */
/*     } */
/*     // save the index */
/*     inAssign[i] = inIdx; */
/*   } */

/*   //  search for available output nodes */
/*   for(i=0; i<op->numOutputs; i++) { */
/*     // search for unused output idx */
/*     outIdx = -1; */
/*     for(j=0; j<NET_OUTS_MAX; j++) { */
/*       if(net->outs[j].opIdx == -1) { */
/* 	outIdx = j; */
/* 	break; */
/*       } */
/*     } */
    
/*     if(outIdx == -1) { */
/*       // print_dbg("\r\n no free output slots; bail "); */
/*       return -1; */
/*     } */
/*     // save the index */
/*     outAssign[i] = outIdx; */
/*   } */

/*   net->ops[opIdx] = op; */

/*   // print_dbg("\r\n assigned operator at slot: "); */
/*   // print_dbg_ulong(opIdx);  */
/*   // print_dbg(" to memory at: "); */
/*   // print_dbg_hex(op); */

/*   // update op allocation flag */
/*   BITSET(net->opsUsed, opIdx); */
/*   // assign i/o nodes at free indices */
/*   for(i=0; i<op->numInputs; i++) { */
/*     net->ins[inAssign[i]].opIdx = opIdx; */
/*     net->ins[inAssign[i]].opInIdx = i; */
/*   } */
/*   for(i=0; i<op->numOutputs; i++) { */
/*     net->outs[outAssign[i]].opIdx = opIdx; */
/*     net->outs[outAssign[i]].opOutIdx = i; */
/*     net->outs[outAssign[i]].target = -1; */
/*   } */
/*   net->numIns  += op->numInputs; */
/*   net->numOuts += op->numOutputs; */
/* } */
#else
// attempt to allocate a new operator from the static memory pool, return index
s16 net_add_op(opId_t opId) {
  u16 ins, outs;
  u8 i;
  op_t* op;

  if (net->numOps >= NET_OPS_MAX) {
    return -1;
  }

  if (op_registry[opId].size > NET_OP_POOL_SIZE - net->opPoolOffset) {
    // not enough memory in op pool
    return -1;
  }
  op = (op_t*)((u8*)net->opPool + net->opPoolOffset);
  // use the class ID to initialize a new object in scratch
  switch(opId) {
  case eOpSwitch:
    op_sw_init((void*) op);
    break;
  case eOpEnc:
    op_enc_init((void*)op);
    break;
  case eOpAdd:
    op_add_init((void*)op);
    break;
  case eOpMul:
    op_mul_init((void*)op);
    break;
  case eOpGate:
    op_gate_init((void*)op);
    break;
#if 0
  /* case eOpAccum: */
  /*   op_accum_init((void*)op); */
  /*   break; */
  /* case eOpSelect: */
  /*   return -1; */
  /*   break; */
  /* case eOpMapLin: */
  /*   return -1; */
  /*   break; */
#endif
  default:
    return -1;
  }

  ins = op->numInputs;
  outs = op->numOutputs;
  op->type = opId;
 
  if (ins > (NET_INS_MAX - net->numIns)) {
    return -1;
  }

  if (outs > (NET_OUTS_MAX - net->numOuts)) {
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
#endif

// destroy last operator created
s16 net_pop_op(void) {
#if NET_USE_MALLOC
#else
  op_t* op = net->ops[net->numOps - 1];
  net->numIns -= op->numInputs;
  net->numOuts -= op->numOutputs;
  net->opPoolOffset += op_registry[op->type].size;
  net->numOps -= 1;
  return 0;
#endif
}

/// delete an arbitrary operator, and do horrible ugly management stuff
void net_remove_op(const u32 idx) {
  /// FIXME: network processing MUST be halted during this procedure!
#if NET_USE_MALLOC
#else
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
      // print_dbg("\r\n oops! couldn't find inlet for deleted operator! \r\n");
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
#endif
}


// create a connection between given idx pairxs
void net_connect(u32 oIdx, u32 iIdx) {
  net->ops[net->outs[oIdx].opIdx]->out[net->outs[oIdx].opOutIdx] = iIdx;
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
      // not a param input either
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
  for(which=0; which<net->numIns; which++) {
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
opStatus_t net_op_status(u16 opIdx) {
  return net->ops[opIdx]->status;
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
  //  print_dbg("\r\n retreiving input val at idx: ");
  //  print_dbg_hex(inIdx);
  //  print_dbg(" , address: ");
  //  print_dbg_hex( &(net->ins[inIdx]) );
  if(inIdx < 0) {
    return 0;
  }
  if (inIdx >= net->numIns) {
    inIdx -= net->numIns;
    return get_param_value(inIdx);
  } else {
    //return (net->ins[inIdx].val);
    return op_get_in_val(net->ops[net->ins[inIdx].opIdx], net->ins[inIdx].opInIdx);
  }
}

void net_set_in_value(s32 inIdx, io_t val) {
  //net->ins[inIdx].val = val;
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
  net->params[net->numParams].desc = *pdesc;  
  //  memcpy( &(net->params[net->numParams].desc), (const void*)pdesc, sizeof(ParamDesc) );make
  /* print_dbg("\r\n added param: "); */
  /* print_dbg_ulong(idx); */
  /* print_dbg(" , addr: "); */
  /* print_dbg_hex(  &(net->params[net->numParams]) ); */

  /* print_dbg("\r\n src label: "); */
  /* print_dbg( pdesc->label ); */
  /* print_dbg("\r\n dst label: "); */
  /* print_dbg( &(net->params[net->numParams].desc.label) ); */

  net->params[net->numParams].idx = idx; 
  net->params[net->numParams].preset = 1; 
  net->params[net->numParams].data.value.asInt = fix16_from_float(pdesc->min);
  net->numParams++;

  /* print_dbg("\r\n ctlnet: added parameter at index "); */
  /* print_dbg_ulong(idx); */
}

// clear existing parameters
void net_clear_params(void) {
  net->numParams = 0;
}

// forward/backward search for input/output nodes
s32 net_next_in(void) {
  while( inSearchIdx < NET_INS_MAX ) {
    if(net->ins[inSearchIdx].opIdx >= 0) {
      return inSearchIdx;
    }
    inSearchIdx++;
 } 
  return -1; // no inputs
}

s32 net_prev_in(void) {
  while( inSearchIdx >=0 ) {
    if(net->ins[inSearchIdx].opIdx >= 0) {
      return inSearchIdx;
    }
    inSearchIdx--;
  }
  return -1; // no inputs
}

s32 net_next_out(void) {
  while( outSearchIdx < NET_OUTS_MAX ) {
    if(net->outs[outSearchIdx].opIdx >= 0) {
      return outSearchIdx;
    }
    outSearchIdx++;
  }
  return -1; // no outputs
}

s32 net_prev_out(void) {
  while( outSearchIdx >=0 ) {
    if(net->outs[outSearchIdx].opIdx >= 0) {
      return outSearchIdx;
    }
    outSearchIdx--;
  }
  return -1; // no outputs
}

void net_reset_in_search(void) {
  inSearchIdx = 0;
}

void net_reset_out_search(void) {
  outSearchIdx = 0;
}
