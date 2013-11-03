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
#include "bfin.h"
#include "control.h"
#include "memory.h"
#include "types.h"
// bees
#include "net.h"
#include "net_protected.h"
#include "op.h" 
#include "op_derived.h"
#include "pages.h"
#include "param.h"
#include "play.h"
#include "util.h"

//=========================================
//===== variables

//----- static
// when unset, node activation will not propagate 
static u8 netActive = 0;

//---- external
ctlnet_t* net;

//-- indices of system-created ops
// encoders
s32 opSysEncIdx[4] = { -1, -1, -1, -1 };
// function keys and footswitches
s32 opSysSwIdx[6] = { -1, -1, -1, -1, -1, -1};
// adc
s32 opSysAdcIdx = -1;
// preset
s32 opSysPresetIdx = -1;


//===============================================
//========= static functions

// create all system operators
static void add_sys_ops(void);
static void add_sys_ops(void) {
  /// create each system operator and store its index.
  /// this is kind of a bad hack, 
  // because we assume that the indices of sys ops in scene data 
  // will always have the same values as when we created them.
  // the assumption holds as long as we don't mess with order of creation...

  // 4 encoders
  net_add_op(eOpEnc);
  opSysEncIdx[0] = net->numOps - 1;
  net_add_op(eOpEnc);
  opSysEncIdx[1] = net->numOps - 1;
  net_add_op(eOpEnc);
  opSysEncIdx[2] = net->numOps - 1;
  net_add_op(eOpEnc);
  opSysEncIdx[3] = net->numOps - 1;
  // 4 function switches
  net_add_op(eOpSwitch);
  opSysSwIdx[0] = net->numOps - 1;
  net_add_op(eOpSwitch);
  opSysSwIdx[1] = net->numOps - 1;
  net_add_op(eOpSwitch);
  opSysSwIdx[2] = net->numOps - 1;
  net_add_op(eOpSwitch);
  opSysSwIdx[3] = net->numOps - 1;
  // 2 footswitches  
  net_add_op(eOpSwitch);
  opSysSwIdx[4] = net->numOps - 1;
  net_add_op(eOpSwitch);
  opSysSwIdx[5] = net->numOps - 1;
  // 1 adc
  net_add_op(eOpAdc);
  opSysAdcIdx = net->numOps -1;
  // 1 preset receiver
  //// FIXME
  //  net_add_op(eOpPreset);
}

///----- node pickling

static u8* onode_pickle(onode_t* out, u8* dst) {
  // operator output index
  dst = pickle_32((u32)(out->opOutIdx), dst);
  // target
  dst = pickle_32((u32)(out->target), dst);
  // parent op's index in net list
  dst = pickle_32((u32)(out->opIdx), dst);
  // preset inclusion flag ; cast to 4 bytes for alignment
  dst = pickle_32((u32)(out->preset), dst);
  return dst;
}

static const u8* onode_unpickle(const u8* src, onode_t* out) {
  u32 v32;
  // operator output index
  src = unpickle_32(src, &v32);
  out->opOutIdx = (u8)v32;
  // output target
  src = unpickle_32(src, &v32);
  out->target = (s16)v32;

  // index of parent op
  src = unpickle_32(src, &v32);
  out->opIdx = (s32)v32;
  // preset flag: 32 bits for alignment
  src = unpickle_32(src, &v32);
  out->preset = (u8)v32;
  return src;
}

static u8* inode_pickle(inode_t* in, u8* dst) {
  /// don't need to pickle indices because we recreate the op list from scratch
  // preset inclusion flag
  *dst++ = in->preset;
  // play inclusion flag
  *dst++ = in->play;
  // dummy byte for alignment
  *dst++ = 0;
  // dummy byte for alignment
  *dst++ = 0;
  return dst;
}

static const u8* inode_unpickle(const u8* src, inode_t* in) {
  /// don't need to pickle indices because we recreate the op list from scratch
  // only need these flags:
  in->preset = *src++;
  // play inclusion flag
  in->play = *src++;
  // dummy byte for alignment
  ++src; 
  // dummy byte for alignment
  ++src; 
  return src;
}


//==================================================
//========= public functions

// initialize network at pre-allocated memory
void net_init(void) {
  u32 i;
  net = (ctlnet_t*)alloc_mem(sizeof(ctlnet_t));

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
  // ???
  netActive = 1;
}

// de-initialize network
void net_deinit(void) {
  u32 i;
  print_dbg("\r\n deinitializing network");
  for(i=0; i<net->numOps; i++) {
    op_deinit(net->ops[i]);
  }
  
  print_dbg("\r\n finished de-initializing network");

  net->opPoolOffset = 0;
  net->numOps = 0;
  net->numIns = 0;
  net->numOuts = 0;
  net->numParams = 0;
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
  static inode_t* pIn;
  if(!netActive) {
    if(op != NULL) {
      // if the net isn't active, dont respond to requests from operators
      print_dbg(" ... ignoring node activation from op.");
      return;
    }
  }

  if(inIdx >= 0) {
    // input exists
    pIn = &(net->ins[inIdx]);

    /// only process for play mode if we're in play mode
    if(pageIdx == ePagePlay) {
      /// FIXME: use play-included flag
      // for now, process all activations with play mode
      //if(pIn->play) {
      play_input(inIdx);
      //    }
    }

    if(inIdx < net->numIns) {
      // this is an op input
      op_set_in_val(net->ops[pIn->opIdx],
		    pIn->opInIdx,
		    val);
    } else { 
      // this is a parameter
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

  if (net->numOps >= NET_OPS_MAX) {
    return -1;
  }

  if (op_registry[opId].size > NET_OP_POOL_SIZE - net->opPoolOffset) {
    print_dbg("\r\n op creation failed; out of memory.");
    return -1;
  }

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
  // advance offset for next allocation
  net->opPoolOffset += op_registry[opId].size;

  //---- add inputs and outputs to node list
    for(i=0; i<ins; ++i) {
      net->ins[net->numIns].opIdx = net->numOps;
      net->ins[net->numIns].opInIdx = i;
      ++(net->numIns);
    }
  for(i=0; i<outs; i++) {
    net->outs[net->numOuts].opIdx = net->numOps;
    net->outs[net->numOuts].opOutIdx = i;
    net->outs[net->numOuts].target = -1;
    ++(net->numOuts);
  }
  ++(net->numOps);

  return net->numOps - 1;
}

// destroy last operator created
s16 net_pop_op(void) {
  op_t* op = net->ops[net->numOps - 1];
  int i=0;
  int x=0;
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

  net->opPoolOffset -= op_registry[op->type].size;
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
  for( pMem = (u8*)op + opSize; 
       (u32)pMem < ((u32)(net->opPool) + net->opPoolOffset);
       pMem++
       ) {
    *((u8*)(pMem - opSize)) = *((u8*)pMem);
  }
  /// move the memory offset back
  net->opPoolOffset -= opSize;
  /// update node and op counts
  net->numIns -= nIns;
  net->numOuts -= nOuts;
  net->numOps -= 1;
  //... and, uh, don't crash?
}

// create a connection between given idx pairs
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

// get current count of inputs (including DSP parameters)
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

// probably only called from UI,
// can err on the side of caution vs speed
io_t net_inc_in_value(s32 inIdx, io_t inc) {
  op_t* op;
  if(inIdx >= net->numIns) {
    // hack to get preset idx
    inIdx -= net->numIns;
    set_param_value(inIdx, OP_SADD(get_param_value(inIdx), inc));
    return get_param_value(inIdx);
  } else {
    op = net->ops[net->ins[inIdx].opIdx];
    (*(op->inc_fn))(op, net->ins[inIdx].opInIdx, inc);
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
void net_add_param(u32 idx, const ParamDesc * pdesc) {
  memcpy( &(net->params[net->numParams].desc), (const void*)pdesc, sizeof(ParamDesc) );
  net->params[net->numParams].idx = idx; 
  net->params[net->numParams].preset = 1; 
  net->params[net->numParams].data.value.asInt = pdesc->min;
  net->numParams += 1;
}

// clear existing parameters
void net_clear_params(void) {
  print_dbg("\r\n clearing parameter list... ");
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

// query the blackfin for parameter list and populate pnodes
u8 net_report_params(void) {
  volatile char buf[64];
  volatile ParamDesc pdesc;
  volatile u32 numParams;
  u8 i;
 
  bfin_get_num_params(&numParams);
  
  print_dbg("\r\nnumparams: ");
  print_dbg_ulong(numParams);

  if(numParams == 255) {
    print_dbg("\r\n report_params fail (255)");
    return 0;
  }
  
  if(numParams > 0) {

    net_clear_params();

    for(i=0; i<numParams; i++) {
      bfin_get_param_desc(i, &pdesc);

      print_dbg("\r\n received descriptor for param : ");
      print_dbg((const char* )pdesc.label);

      net_add_param(i, (const ParamDesc*)&pdesc);

    }
  } else {
    print_dbg("\r\n bfin: no parameters reported");
    return 0;
  }
  
  delay_ms(100);

  print_dbg("\r\n checking module label ");
  bfin_get_module_name(buf);

  delay_ms(10);

  print_dbg("\r\n bfin module name: ");
  print_dbg((const char*)buf);

  return (u8)numParams;

}


// pickle the network!
u8* net_pickle(u8* dst) {
  u32 i;
  op_t* op;
  u32 val = 0;

  // store count of operators
  // (use 4 bytes for alignment)
  dst = pickle_32((u32)(net->numOps), dst);
  pickle_32((u32)(net->numOps), (u8*)(&val));

  // loop over operators
  for(i=0; i<net->numOps; ++i) {
    op = net->ops[i];
    // store type id
    dst = pickle_32(op->type, dst);
    // pickle the operator state (if needed)
    if(op->pickle != NULL) {
      dst = (*(op->pickle))(op, dst);
    }
  }

  // write input nodes
  for(i=0; i < net->numIns; ++i) {
    dst = inode_pickle(&(net->ins[i]), dst);
  }

  // write output nodes
  for(i=0; i < net->numOuts; ++i) {
    dst = onode_pickle(&(net->outs[i]), dst);
  }

  // write count of parameters
  val = (u32)(net->numParams);
  dst = pickle_32(val, dst);

  // write parameter nodes (includes value and descriptor)
  for(i=0; i<net->numParams; ++i) {
    dst = param_pickle(&(net->params[i]), dst);
  }

  return dst;
}

// unpickle the network!
u8* net_unpickle(const u8* src) {
  u32 i, count, val;
  op_id_t id;
  op_t* op;

  // reset operator count, param count, pool offset, etc
  net_deinit();

  // get count of operators
  // (use 4 bytes for alignment)
  src = unpickle_32(src, &count);

  // loop over operators
  for(i=0; i<count; ++i) {
    // get operator class id
    src = unpickle_32(src, &val);
    id = (op_id_t)val;

    // add and initialize from class id
    /// .. this should update the operator count, inodes and onodes
    net_add_op(id);

    // unpickle operator state (if needed)
    op = net->ops[net->numOps - 1];

    if(op->unpickle != NULL) {
      src = (*(op->unpickle))(op, src);
    }
  }

  // read input nodes
  for(i=0; i < net->numIns; ++i) {
    src = inode_unpickle(src, &(net->ins[i]));
  }

  // read output nodes
  for(i=0; i < net->numOuts; ++i) {
    src = onode_unpickle(src, &(net->outs[i]));
    // reconnect so the parent operator knows what to do
    net_connect(i, net->outs[i].target);
  }

  // get count of parameters
  src = unpickle_32(src, &val);
  net->numParams = (u16)val;
  
  // read parameter nodes (includes value and descriptor)
  for(i=0; i<(net->numParams); ++i) {
    src = param_unpickle(&(net->params[i]), src);
  }
  
  return (u8*)src;
}

///////////////
// test / dbg
void net_print(void) {
  print_dbg("\r\n net address: 0x");
  print_dbg_hex((u32)(net));

  print_dbg("\r\n net input count: ");
  print_dbg_ulong(net->numIns);
  print_dbg("\r\n net output count: ");
  print_dbg_ulong(net->numOuts);
  print_dbg("\r\n net op count: ");
  print_dbg_ulong(net->numOps);
}
