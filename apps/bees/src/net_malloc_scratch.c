
#if NET_USE_MALLOC
/* // allocate a new operator dynamically. */
/* // return index in op pointer array */
/* s16 net_add_op(op_id_t opId) { */
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
