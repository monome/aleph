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
#include "print_funcs.h"
#include "delay.h"

// aleph-avr32
#include "app.h"
#include "bfin.h"
#include "control.h"
#include "memory.h"
#include "types.h"

// bees
#include "net.h"
#include "net_protected.h"
#include "op.h" 
#include "op_derived.h"
#include "op_gfx.h"
#include "pages.h"
#include "param.h"
#include "play.h"
#include "preset.h"
#include "util.h"
#include "op_pool.h"


//=========================================
//===== constants


// define for serialization debugging
// #define PRINT_PICKLE 1


//=========================================
//===== variables

//----- static
// when unset, node activation will not propagate 
static u8 netActive = 0;

//---- external
ctlnet_t* net;

// pointers to system-created ops
// encoders
op_enc_t* opSysEnc[4] = { NULL, NULL, NULL, NULL };
// function keys and footswitches
op_sw_t* opSysSw[6] = { NULL, NULL, NULL, NULL, NULL, NULL };
// adc
op_adc_t* opSysAdc = NULL;
// preset
op_preset_t* opSysPreset = NULL;

static const char emptystring[] = "            ";

//===============================================
//========= static functions


/// stupid hack function to identify switch input
/// returns switch index in [1, numSwitches]
/// otherwise 0
/// FIXME: obviously this is magic # bs
static inline int in_get_switch_index(s16 in) { 
  if(in > 3 && in < 10) {
    return in - 3;
  } else {
    return 0;
  }
}

// create all system operators
static void add_sys_ops(void);
static void add_sys_ops(void) {
  /// FIXME: 
  /* dangerous for scene storage, 
     will break in the unlikely event that op pool is assigned differently.
     should either:
     a) reassign these pointers after unpickling 
         probably by index like the old hack, or
     b) don't pickle system ops at all, only their inputs.
         still needs to make a fixed assumption about order.
... i dunno
   */

  // 4 encoders
  net_add_op(eOpEnc);
  opSysEnc[0] = (op_enc_t*)net->ops[net->numOps - 1];
  net_add_op(eOpEnc);
  opSysEnc[1] = (op_enc_t*)net->ops[net->numOps - 1];
  net_add_op(eOpEnc);
  opSysEnc[2] = (op_enc_t*)net->ops[net->numOps - 1];
  net_add_op(eOpEnc);
  opSysEnc[3] = (op_enc_t*)net->ops[net->numOps - 1];
  // 4 function switches
  net_add_op(eOpSwitch);
  opSysSw[0] = (op_sw_t*)net->ops[net->numOps - 1];
  net_add_op(eOpSwitch);
  opSysSw[1] = (op_sw_t*)net->ops[net->numOps - 1];
  net_add_op(eOpSwitch);
  opSysSw[2] = (op_sw_t*)net->ops[net->numOps - 1];
  net_add_op(eOpSwitch);
  opSysSw[3] = (op_sw_t*)net->ops[net->numOps - 1];
  // 2 footswitches  
  net_add_op(eOpSwitch);
  opSysSw[4] = (op_sw_t*)net->ops[net->numOps - 1];
  net_add_op(eOpSwitch);
  opSysSw[5] = (op_sw_t*)net->ops[net->numOps - 1];
  // 1 adc
  net_add_op(eOpAdc);
  opSysAdc = (op_adc_t*)net->ops[net->numOps -1];
  // 1 preset receiver
  net_add_op(eOpPreset);
  opSysPreset = (op_preset_t*)net->ops[net->numOps -1];
}

///----- node pickling

static u8* onode_pickle(onode_t* out, u8* dst) {
  // target
  dst = pickle_32((u32)(out->target), dst);
  return dst;
}

static const u8* onode_unpickle(const u8* src, onode_t* out) {
  u32 v32;

  // output target
  src = unpickle_32(src, &v32);
  out->target = (s16)v32;


#ifdef PRINT_PICKLE
  print_dbg(" ; opIdx: ");
  print_dbg_ulong(out->opIdx);
  print_dbg(" ; opOutIdx: ");
  print_dbg_ulong(out->opOutIdx);
  print_dbg(" ; target: ");
  print_dbg_ulong(out->target);
#endif

  return src;
}

static u8* inode_pickle(inode_t* in, u8* dst) {
  /// don't need to pickle indices because we recreate the op list from scratch
#ifdef PRINT_PICKLE
  print_dbg("\r\n pickling input node, op index: ");
  print_dbg_ulong(in->opIdx);
  print_dbg(" , input idx: ");
  print_dbg_ulong(in->opInIdx);
  print_dbg(" , play flag: ");
  print_dbg_ulong(in->play);
#endif

  // play inclusion flag
  *dst++ = in->play;
  // dummy byte for alignment
  *dst++ = 0;
  // dummy byte for alignment
  *dst++ = 0;
  // dummy byte for alignment
  *dst++ = 0;
  return dst;
}

static const u8* inode_unpickle(const u8* src, inode_t* in) {
  /// don't need to pickle indices because we recreate the op list from scratch
  // only need these flags:
  //  in->preset = *src++;
  // play inclusion flag
  in->play = *src++;

#ifdef PRINT_PICKLE
  print_dbg(" ; opIdx: ");
  print_dbg_ulong(in->opIdx);
  print_dbg(" ; opInIdx: ");
  print_dbg_ulong(in->opInIdx);

  print_dbg("; got flag: ");
  print_dbg_ulong(in->play);
#endif

  // dummy byte for alignment
  ++src; 
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
    freeOp((u8*)net->ops[i]);
  }
  
  print_dbg("\r\n finished de-initializing network");

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

  // make sure to get out of op-graphics mode
  op_gfx_reset();

}

// clear ops and i/o
void net_clear_user_ops(void) {
  /// no... this seems wrong.
  net_deinit();
  add_sys_ops();
}

// initialize an input node
void net_init_inode(u16 idx) {
  net->ins[idx].opIdx = -1;
  net->ins[idx].play = 0;
}

// initialize an output node
void net_init_onode(u16 idx) {
  net->outs[idx].opIdx = -1;
  net->outs[idx].target = -1;
}

// activate an input node with a value
void net_activate(s16 inIdx, const io_t val, void* op) {
  static inode_t* pIn;
  s16 pIndex;
  u8 vis;

  /* print_dbg("\r\n net_activate, input idx: "); */
  /* print_dbg_hex(inIdx); */
  /* print_dbg(" , value: "); */
  /* print_dbg_hex(val); */

  /* print_dbg(" , op index: "); */
  /* print_dbg_ulong(net->ins[inIdx].opIdx); */
  /* print_dbg(" , input idx: "); */
  /* print_dbg_ulong(net->ins[inIdx].opInIdx); */

  if(!netActive) {
    if(op != NULL) {
      // if the net isn't active, dont respond to requests from operators
      print_dbg(" ... ignoring node activation from op.");
      return;
    }
  }



  if(inIdx < 0) {
    return;
  }

  vis = net_get_in_play(inIdx);

  if(inIdx < net->numIns) {      
    // this is an op input
    pIn = &(net->ins[inIdx]);
    
    op_set_in_val(net->ops[pIn->opIdx],
		  pIn->opInIdx,
		  val);
    
  } else { 
    // this is a parameter
    //// FIXME this is horrible
    pIndex = inIdx - net->numIns;
    if (pIndex >= net->numParams) { return; }
    set_param_value(pIndex, val);
  }

  /// only process for play mode if we're in play mode
  if(pageIdx == ePagePlay) {
    if(opPlay) {
      // operators have focus, do nothing
    } else {
      // process if play-mode-visibility is set on this input
      if(vis) {
	play_input(inIdx);
      }
    }
  }  
  
}

// attempt to allocate a new operator from the static memory pool, return index
s16 net_add_op(op_id_t opId) {
  u16 ins, outs;
  int i, j;
  int idxOld, idxNew;
  op_t* op = NULL;
  s32 numInsSave = net->numIns;
  s32 numOutsSave = net->numOuts;

  print_dbg("\r\n adding operator; old input count: ");
  print_dbg_ulong(numInsSave);

  if (net->numOps >= NET_OPS_MAX) {
    return -1;
  }
  print_dbg(" , op class: ");
  print_dbg_ulong(opId);
  print_dbg(" , size: ");
  print_dbg_ulong(op_registry[opId].size);


  print_dbg(" ; allocating... ");
  size_t opChunk = op_registry[opId].size;
  if (opChunk <= SMALL_OP_SIZE) {
    op = (op_t*)allocSmallOp();
  }
  else if (opChunk <= BIG_OP_SIZE) {
    op = (op_t*)allocBigOp();
  }
  if (op == NULL){
    op = (op_t*)malloc(opChunk);
  }

  if (op == NULL) {
    print_dbg("\r\ncouldn't get enough memory for new op");
    return -1;
  }
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

  if(net->numOps > 0) {
    // if we added input nodes, need to adjust connections to DSP params
    for(i=0; i < numOutsSave; i++) {            
      if(net->outs[i].target >= numInsSave) {
	// preset target, add offset for new inputs
	net_connect(i, net->outs[i].target + ins);
      }

      /// do the same in all presets!
      for(j=0; j<NET_PRESETS_MAX; j++) {
	if(preset_out_enabled(j, i)) {
	  s16 tar = presets[j].outs[i].target;
	  if(tar >= numInsSave) {
	    tar = tar + ins;
	    presets[j].outs[i].target = tar;
	  }
	}
      } // preset loop
    } // outs loop
    
    for(i=0; i<NET_PRESETS_MAX; i++) {
      // shift parameter nodes in preset data
      for(j=net->numParams - 1; j>=0; j--) {
	// this was the old param index
	idxOld = j + numInsSave;
	// copy to new param index
	idxNew = idxOld + ins;
	if(idxNew >= PRESET_INODES_COUNT) {
	  print_dbg("\r\n out of preset input nodes in new op creation! ");
	  continue;
	} else {
	  presets[i].ins[idxNew].value = presets[i].ins[idxOld].value;
	  presets[i].ins[idxNew].enabled = presets[i].ins[idxOld].enabled;
	  // clear the old data. it may correspond to new operator inputs.
	  presets[i].ins[idxOld].enabled = 0;
	  presets[i].ins[idxOld].value = 0;
	}
      }
    }
    
  }

  ++(net->numOps);
  return net->numOps - 1;
}

// attempt to allocate a new operator from the static memory pool, return index
s16 net_add_op_at(op_id_t opId, int opIdx) {
  u16 ins, outs;
  int i, j;
  int idxOld, idxNew;
  op_t* op = NULL;
  s32 numInsSave = net->numIns;
  opIdx +=1;
  if (opIdx < 12) {
    opIdx = 12;
  }
  if (opIdx > net->numOps) {
    opIdx = net->numOps;
  }

  if (net->numOps >= NET_OPS_MAX) {
    return -1;
  }
  print_dbg(" , op class: ");
  print_dbg_ulong(opId);
  print_dbg(" , size: ");
  print_dbg_ulong(op_registry[opId].size);


  print_dbg(" ; allocating... ");
  size_t opChunk = op_registry[opId].size;
  if (opChunk <= SMALL_OP_SIZE) {
    op = (op_t*)allocSmallOp();
  }
  else if (opChunk <= BIG_OP_SIZE) {
    op = (op_t*)allocBigOp();
  }
  if (op == NULL){
    op = (op_t*)malloc(opChunk);
  }

  if (op == NULL) {
    print_dbg("\r\ncouldn't get enough memory for new op");
    return -1;
  }
  op_init(op, opId);

  ins = op->numInputs;
  outs = op->numOutputs;
  int opFirstIn =0;
  int opFirstOut = 0;
  i=0;
  while (i < opIdx) {
    opFirstIn += net->ops[i]->numInputs;
    opFirstOut += net->ops[i]->numOutputs;
    i++;
  }

  if (ins > (NET_INS_MAX - net->numIns)) {
    print_dbg("\r\n op creation failed; too many inputs in network.");
    return -1;
  }

  if (outs > (NET_OUTS_MAX - net->numOuts)) {
    print_dbg("\r\n op creation failed; too many outputs in network.");
    return -1;
  }

  net->numIns +=ins;
  net->numOuts += outs;
  net->numOps += 1;

  for(i=net->numOps - 1; i >= opIdx; i--) {
    net->ops[i] = net->ops[i-1];
  }
  for(i=net->numOuts - 1; i >= opFirstOut; i--) {
    net->outs[i] = net->outs[i-outs];
    net->outs[i].opIdx += 1;
  }
  for(i=net->numIns - 1; i >= opFirstIn; i--) {
    net->ins[i] = net->ins[i-ins];
    net->ins[i].opIdx += 1;
  }
  
  // add op pointer to list
  // advance offset for next allocation
  net->ops[opIdx] = op;
  //---- add inputs and outputs to node list
  for (i=0; i<ins; ++i) {
    net->ins[opFirstIn + i].opIdx = opIdx;
    net->ins[opFirstIn + i].opInIdx = i;
  }
  
  for (i=0; i<outs; i++) {
    net->outs[opFirstOut + i].opIdx = opIdx;
    net->outs[opFirstOut + i].opOutIdx = i;
    net->outs[opFirstOut + i].target = -1;
  }

  if(net->numOps > 0) {
    for(i=0; i < net->numOuts; i++) {
      // if we added input nodes, need to adjust connections to
      // subsequent ins (including DSP params)
      if (net->outs[i].target >= opFirstIn) {
	net_connect(i, net->outs[i].target + ins);
      }

      /// do the same in all presets!
      for(j=0; j<NET_PRESETS_MAX; j++) {
	if(preset_out_enabled(j, i)) {
	  s16 tar = presets[j].outs[i].target;
	  if(tar >= opFirstIn) {
	    tar = tar + ins;
	    presets[j].outs[i].target = tar;
	  }
	}
      } // preset loop
    } // outs loop
    
    for(i=0; i<NET_PRESETS_MAX; i++) {
      // shift parameter nodes in preset data
      for(j=net->numParams - 1; j>=0; j--) {
	// this was the old param index
	idxOld = j + numInsSave;
	// copy to new param index
	idxNew = idxOld + ins;
	if(idxNew >= PRESET_INODES_COUNT) {
	  print_dbg("\r\n out of preset input nodes in new op creation! ");
	  continue;
	} else {
	  presets[i].ins[idxNew].value = presets[i].ins[idxOld].value;
	  presets[i].ins[idxNew].enabled = presets[i].ins[idxOld].enabled;
	  // clear the old data. it may correspond to new operator inputs.
	  presets[i].ins[idxOld].enabled = 0;
	  presets[i].ins[idxOld].value = 0;
	}
      }
    }
    
  }

  return net->numOps - 1;
}

// destroy last operator created
s16 net_pop_op(void) {
  const s16 opIdx = net->numOps - 1;
  op_t* op = net->ops[opIdx];
  int i, j;
  int x, y;
  int ins;
  int numInsSave = net->numIns;
  int idxOld, idxNew;

  app_pause();
  // bail if system op
  if(net_op_flag (opIdx, eOpFlagSys)) { 
    app_resume();
    return 1; 
  }
  
  // de-init
  op_deinit(op);
  freeOp((u8*)op);
  ins = op->numInputs;
  // store the global index of the first input
  x = net_op_in_idx(opIdx, 0);
  y = x + ins;

  // check if anything connects here
  for(i=0; i<net->numOuts; i++) {
    // this check works b/c we know this is last op in list
    if( net->outs[i].target >= x ) {
      if( net->outs[i].target < y) {
	net_disconnect(i);
      } else {
	/// this should take care of both param and op input targets.
	net_connect(i, net->outs[i].target - op->numInputs); 
      }
    }
  }
  // erase input nodes
  while(x < y) {
    net_init_inode(x++);
  }
  // store the global index of the first output
  x = net_op_out_idx(opIdx, 0);
  y = x + op->numOutputs;
  // erase output nodes
  while(x < y) {
    net_init_onode(x++);
  }

  net->numIns -= op->numInputs;
  net->numOuts -= op->numOutputs;

  net->numOps -= 1;

  // FIXME: shift preset param data and connections to params, 
  // since they share an indexing list with inputs and we just changed it.

  for(i=0; i<NET_PRESETS_MAX; ++i) {
    // shift parameter nodes in preset data
    for(j=0; j<net->numParams; ++j) {
      // this was the old param index
      idxOld = j + numInsSave;
      // copy to new param index
      idxNew = idxOld - ins;
      presets[i].ins[idxNew].value = presets[i].ins[idxOld].value;
      presets[i].ins[idxNew].enabled = presets[i].ins[idxOld].enabled;
      // clear the old data.
      presets[i].ins[idxOld].enabled = 0;
      presets[i].ins[idxOld].value = 0;
    }

  }


  app_resume();
  return 0;

}

s16 net_remove_op(const u32 opIdx) {
  op_t* op = net->ops[opIdx];
  int opNumInputs = op->numInputs;
  int opNumOutputs = op->numOutputs;
  int i, j;
  int numInsSave = net->numIns;
  int idxOld, idxNew;

  app_pause();
  // bail if system op
  if(net_op_flag (opIdx, eOpFlagSys)) {
    app_resume();
    return 1;
  }
  // bail if out of range
  if(opIdx < 0 || opIdx >= net->numOps) {
    print_dbg("\r\nout-of-range op deletion requested");
    app_resume();
    return 1;
  }

  print_dbg("\r\ndeinit-ing op");
  // de-init
  op_deinit(op);
  freeOp((u8*)op);
  print_dbg("\r\nde-inited op");
  // store the global index of the first input
  int opFirstIn = net_op_in_idx(opIdx, 0);

  // check each output, break connections to removed op,
  // adjust output indices after removed op down for the gap
  for(i=0; i<net->numOuts; i++) {
    print_dbg("\r\nmonkey");
    // break connections to removed op
    if( net->outs[i].target >= opFirstIn &&
	net->outs[i].target < opFirstIn + opNumInputs) {
      print_dbg("\r\nbadger");
      net_disconnect(i);
    } else if (net->outs[i].target >= opFirstIn + opNumInputs) {
      /// shuffle op indexes down past removed op
      print_dbg("\r\nrat");
      net_connect(i, net->outs[i].target - opNumInputs);
    }
  }

  print_dbg("\r\nreshuffling...");
  // reshuffle input indices & associated op indices above
  // the removed op
  for(i = opFirstIn; i < net->numIns - opNumInputs; i++) {
    print_dbg("\r\nshuffle shuffle...");
    net->ins[i] = net->ins[i + opNumInputs];
    net->ins[i].opIdx -= 1;
  }

  // reshuffle output indices & associated op indices above
  // the removed op
  for(i = 0; i < net->numOuts - opNumOutputs; i++) {
    if (net->outs[i].opIdx >= opIdx) {
      print_dbg("\r\nshiffle shiffle");
      net->outs[i] = net->outs[i + opNumOutputs];
      net->outs[i].opIdx -= 1;
    }
  }

  net->numIns -= opNumInputs;
  net->numOuts -= opNumOutputs;
  net->numOps -= 1;

  for(i=opIdx; i < net->numOps; i++) {
    print_dbg("\r\nshuffling superior op-idx");
    net->ops[i] = net->ops[i+1];
  }

  //HACK try re-indexing all outputs
  for(i=0; i<net->numOuts; i++) {
    if (net->outs[i].target >= 0) {
      net_connect(i, net->outs[i].target);
    }
  }

  // FIXME: shift preset param data and connections to params,
  // since they share an indexing list with inputs and we just changed it.
  print_dbg("\r\nwhatever...");
  for(i=0; i<NET_PRESETS_MAX; ++i) {
    // shift parameter nodes in preset data
    for(j=0; j<net->numParams; ++j) {
      // this was the old param index
      idxOld = j + numInsSave;
      // copy to new param index
      idxNew = idxOld - opNumInputs;
      presets[i].ins[idxNew].value = presets[i].ins[idxOld].value;
      presets[i].ins[idxNew].enabled = presets[i].ins[idxOld].enabled;
      // clear the old data.
      presets[i].ins[idxOld].enabled = 0;
      presets[i].ins[idxOld].value = 0;
    }
  }
  print_dbg("\r\nresume app...");
  app_resume();
  print_dbg("\r\nresumed app no probbers...");

  return 0;

}

// create a connection between given idx pairs
void net_connect(u32 oIdx, u32 iIdx) {
  const s32 srcOpIdx = net->outs[oIdx].opIdx; 
  const s32 dstOpIdx = net->ins[iIdx].opIdx;

  net->outs[oIdx].target = iIdx;
  // FIXME: this could be smarter.
  // but for now, just don't allow an op to connect to itself 
  // (keep the target in the onode for UI purposes,
  // but don't actually update the operator output variable)
  if(srcOpIdx == dstOpIdx) {
    return; 
  } 
  
  /// something weird is happening!
  //  value seems to drift on each disconnect/reconnect...?
  if((srcOpIdx >=0) && (srcOpIdx < net->numOps)) {
    net->ops[srcOpIdx]->out[net->outs[oIdx].opOutIdx] = iIdx;
  } else {
    print_dbg(" !!!!!! WARNING ! invalid source operator index in net_connect() ");
  }
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

// get current count of inputs (including DSP parameters!)
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
  //  int sw;
  if (idx < 0) {
    return (const char*)emptystring;
  }
  /// dirty hack for switch labels
  switch(in_get_switch_index(idx)) {
  case 0:
    return net->ops[idx]->opString;
  case 1:
    return "SW1";
    break;
  case 2:
    return "SW2";
    break;
  case 3:
    return "SW3";
    break;
  case 4:
    return "SW4";
    break;
  case 5:
    return "FS1";
    break;
  case 6:
    return "FS2";
    break;
  default:
    return "!!!";
  }
}

// get name for input at given idx
const char* net_in_name(u16 idx) {
  if (idx >= net->numIns) {
    // not an operator input
    idx -= net->numIns;
    if (idx >= net->numParams) {
      // not a param input either, whoops
      return (const char*)emptystring;
    } else {
      // param input
      return net->params[idx].desc.label;
    }
  } else {
    return op_in_name(net->ops[net->ins[idx].opIdx], net->ins[idx].opInIdx);
  }
}

// get name for output at given idx
const char* net_out_name(const u16 idx) {
  if(idx < net->numOuts) {
    return op_out_name(net->ops[net->outs[idx].opIdx], net->outs[idx].opOutIdx);
  } else {
    return (const char*)emptystring;
  }  
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
  return 0; // get here if op has no inputs
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
    // hack to get param idx
    inIdx -= net->numIns;
    return inc_param_value(inIdx, inc);

  } else {
    op = net->ops[net->ins[inIdx].opIdx];
    op_inc_in_val(op, net->ins[inIdx].opInIdx, inc);    
    return net_get_in_value(inIdx);
  }
}

// toggle preset inclusion for input
u8 net_toggle_in_preset(u32 id) {
    return preset_get_selected()->ins[id].enabled ^= 1;
}

// toggle preset inclusion for output
u8 net_toggle_out_preset(u32 id) {
  u8 tmp = preset_out_enabled(preset_get_select(), id) ^ 1;
  preset_get_selected()->outs[id].enabled = tmp;
  return tmp;
}

// set preset inclusion for input
void net_set_in_preset(u32 id, u8 val) {
    preset_get_selected()->ins[id].enabled = val;
}

  // set preset inclusion for output
void net_set_out_preset(u32 id, u8 val) {
  preset_get_selected()->outs[id].enabled = val;
}

// get preset inclusion for input
u8 net_get_in_preset(u32 id) {
    return preset_get_selected()->ins[id].enabled;
}

// get preset inclusion for output
u8 net_get_out_preset(u32 id) {
  return preset_get_selected()->outs[id].enabled;
}


// toggle play inclusion for input
u8 net_toggle_in_play(u32 inIdx) {
  u32 pidx;
  if(inIdx < net->numIns) {
    net->ins[inIdx].play ^= 1;
    return net->ins[inIdx].play;
  } else {
    pidx = inIdx - net->numIns;
    net->params[pidx].play ^= 1;
    return net->params[pidx].play;
  }
}

// set play inclusion for input
void net_set_in_play(u32 inIdx, u8 val) {
  if(inIdx < net->numIns) {
    net->ins[inIdx].play = val;
  } else {
    net->params[inIdx - net->numIns].play = val;
  }
}

// get play inclusion for input
u8 net_get_in_play(u32 inIdx) {
  if(inIdx < net->numIns) {
    return net->ins[inIdx].play;
  } else {
    return net->params[inIdx - net->numIns].play;
  }
}


//------------------------------------
//------ params

// add a new parameter
void net_add_param(u32 idx, const ParamDesc * pdesc) {
  s32 val;
  // copy descriptor, hm
  memcpy( &(net->params[net->numParams].desc), (const void*)pdesc, sizeof(ParamDesc) );

  // initialize scaler
  scaler_init(&(net->params[net->numParams].scaler), 
	      &(net->params[net->numParams].desc));

  //... FIXME: can't remember what this is about.
  // net->params[net->numParams].idx = idx; 

  net->params[net->numParams].play = 1;

  //  net->params[net->numParams].preset = 0; 
  net->numParams += 1;

  // query initial value
  val = bfin_get_param(idx);
  
  net->params[net->numParams - 1].data.value = 
    scaler_get_in( &(net->params[net->numParams - 1].scaler), val);

  net->params[net->numParams - 1].data.changed = 0; 
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
    set_param_value(i, net->params[i].data.value);
  }
}

// retrigger all inputs
/* void net_retrigger_ins(void) { */
/*   u32 i; */
/*   netActive = 0; */
/*   for(i=0; i<net->numIns; i++) { */
/*     net_activate(i, net_get_in_value(i), NULL); */
/*   } */
/*   netActive = 1; */
/* } */

// pickle the network!
u8* net_pickle(u8* dst) {
  u32 i;
  op_t* op;
  u32 val = 0;

  // write count of operators
  // ( 4 bytes for alignment)
  dst = pickle_32((u32)(net->numOps), dst);

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
  //// all nodes, even unused
  for(i=0; i < (NET_INS_MAX); ++i) {
    dst = inode_pickle(&(net->ins[i]), dst);
  }

  // write output nodes
  for(i=0; i < NET_OUTS_MAX; ++i) {
    dst = onode_pickle(&(net->outs[i]), dst);
  }

  // write count of parameters
  // 4 bytes for alignment
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
  // no system operators after this
  net_deinit();

  // confusingly though, we don't call deinit() after this.
  // system ops are in the pickled scene data along with everything else.
  // net_deinit() should maybe be renamed.

  // get count of operators
  // (use 4 bytes for alignment)
  src = unpickle_32(src, &count);

  #ifdef PRINT_PICKLE
    print_dbg("\r\n count of ops: ");
    print_dbg_ulong(count);
  #endif

  // loop over operators
  for(i=0; i<count; ++i) {
    // get operator class id
    src = unpickle_32(src, &val);
    id = (op_id_t)val;

    #ifdef PRINT_PICKLE
        print_dbg("\r\n adding op, class id: ");
        print_dbg_ulong(id);
    #endif

    // add and initialize from class id
    /// .. this should update the operator count, inodes and onodes
    net_add_op(id);

    // unpickle operator state (if needed)
    op = net->ops[net->numOps - 1];

    if(op->unpickle != NULL)  {
      #ifdef PRINT_PICKLE
            print_dbg(" ... unpickling op .... ");
            print_dbg_ulong(id);
      #endif
      src = (*(op->unpickle))(op, src);
    }
  }

  /// copy ALL i/o nodes, even unused!
  print_dbg("\r\n reading all input nodes ");
  
  for(i=0; i < (NET_INS_MAX); ++i) {
    src = inode_unpickle(src, &(net->ins[i]));
  }

  // read output nodes
  for(i=0; i < NET_OUTS_MAX; ++i) {
    src = onode_unpickle(src, &(net->outs[i]));
    if(i < net->numOuts) {
      if(net->outs[i].target >= 0) {
	// reconnect so the parent operator knows what to do
	net_connect(i, net->outs[i].target);
      }
    }
  }


  // get count of parameters
  src = unpickle_32(src, &val);
  net->numParams = (u16)val;

#ifdef PRINT_PICKLE
  print_dbg("\r\n reading params, count: ");
  print_dbg_ulong(net->numParams);
#endif

  // read parameter nodes (includes value and descriptor)
  for(i=0; i<(net->numParams); ++i) {
#ifdef PRINT_PICKLE
    print_dbg("\r\n unpickling param, idx: ");
    print_dbg_ulong(i);
#endif

    src = param_unpickle(&(net->params[i]), src);
  }
  
  return (u8*)src;
}

// get parameter string representations,
// given string buffer and index in inputs list
void net_get_param_value_string(char* dst, u32 idx) {
  //// FIXME
  /// get param index! rrrgg
  idx -= net->numIns;
  /// lookup representation from stored input value and print to buf
  scaler_get_str( dst,	
		  &(net->params[idx].scaler), 
		  net->params[idx].data.value
		  );
}


// same, with arbitrary value
void net_get_param_value_string_conversion(char* dst, u32 idx, s32 val) {
  /// lookup representation from stored input value and print to buf
  scaler_get_str( dst,	
		  &(net->params[idx].scaler), 
		  val
		  );
}


// disconnect from parameters
void net_disconnect_params(void) {
  int i;
  int j;
  int t = net->numIns; // test target
  for(i=0; i<net->numParams; ++i) {
    for(j=0; j<net->numOuts; ++j) {
      if(net->outs[j].target == t) {
	net_disconnect(j);
      }
    }
    t++;
  }
}


// insert a split after an output node
// return out11 of split if original out was unconnected,
// otherwise connect out1 of split to old target and return out2
s16 net_split_out(s16 outIdx) {
  // saved target
  s16 target =   net->outs[outIdx].target;
  // index of added split operator
  s16 split;
  if( target < 0) {
    // no target
    split = net_add_op(eOpSplit);
    if(split < 0) {
      // failed to add, do nothing
      return outIdx; 
    } else {
      // FIXME: net_op_in_idx is pretty slow
      net_connect(outIdx, net_op_in_idx(split, 0));
      return net_op_out_idx(split, 0);
    } // add ok
  } else {
    // had target; reroute
    split = net_add_op(eOpSplit);
    // get the target again, because maybe it was a DSP param
    // (if it was, its index will have shifted. 
    // patch and presets have been updated, but local var has not.)
    target =   net->outs[outIdx].target;
    if(split < 0) {
      // failed to add, do nothing
      return outIdx; 
    } else {
      // FIXME: net_op_in_idx is pretty slow
      net_connect(outIdx, net_op_in_idx(split, 0));
      net_connect(net_op_out_idx(split, 0), target);
      return net_op_out_idx(split, 1);
    } // add ok
  }
}

/////////////// 
// test / dbg
#if 0
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
#endif


// set active
void net_set_active(bool v) {
  netActive = v;
}
