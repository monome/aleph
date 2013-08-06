
//-------------------------------------------------
//------ accumulator
#if 0 // needs work
static const char* op_accum_instring = "VALUE   COUNT   MIN     MAX     WRAP    ";
static const char* op_accum_outstring = "VALUE   WRAP    ";
static const char* op_accum_opstring = "ACCUM";

static void op_accum_boundscheck(op_accum_t* accum) {
  if(accum->val > accum->max) {
    if(accum->carry != 0) {
      while(accum->val > accum->max) {
        //accum->val -= (accum->max > 0 ? accum->max : accum->max * -1);
	if(accum->max > 0) {
	  accum->val = OP_SUB(accum->val, accum_max);
	} else {
	  accum->val = OP_ADD(accum->val, accum_max);
	}
      }
      net_activate(accum->outs[1], accum->val); // carry output with wrapped value
    } else {
      accum->val = accum->max;
    }
  }
  if(accum->val < accum->min) {
    if(accum->carry) {
      while(accum->val < accum->min) {
	if(accum->min < 0) {
	  accum->val = OP_SUB(accum->val, accum_min);
	} else {
	  accum->val = OP_ADD(accum->val, accum_min);
	}
      }
      net_activate(accum->outs[1], accum->val); // carry output with wrapped value
    } else {
      accum->val = accum->min;
    }
  }  
}

static void op_accum_in_value(op_accum_t* accum, const io_t* v) {
  printf("accum at %d received VALUE %d\n", (int)accum, (int)*v);
  accum->val = *v;
  op_accum_boundscheck(accum);
  net_activate(accum->outs[0], accum->val);
}

static void op_accum_in_count(op_accum_t* accum, const io_t* v) {
  printf("accum at %d received COUNT %d\n", (int)accum, (int)*v);
  acum->val = OP_ADD(accum->val, *v);
  op_accum_boundscheck(accum);
  net_activate(accum->outs[0], accum->val);
}

static void op_accum_in_min(op_accum_t* accum, const io_t* v) {
  printf("accum at %d received MIN %d\n", (int)accum, (int)*v);
  accum->min = *v;
}

static void op_accum_in_max(op_accum_t* accum, const io_t* v) {
  printf("accum at %d received MAX %d\n", (int)accum, (int)*v);
  accum->max = *v;
}

static void op_accum_in_carry(op_accum_t* accum, const io_t* v) {
  printf("accum at %d received CARRY %d\n", (int)accum, (int)*v);
  accum->carry = (io_t)(v != 0);
}

static op_in_func_t op_accum_inputs[5] = {
(op_in_func_t)&op_accum_in_value,
(op_in_func_t)&op_accum_in_count,
(op_in_func_t)&op_accum_in_min, 
(op_in_func_t)&op_accum_in_max,
(op_in_func_t)&op_accum_in_carry
};

void op_accum_init(op_accum_t* accum) {
  accum->super.numInputs = 5;
  accum->super.numOutputs = 2;
  accum->outs[0] = -1;
  accum->outs[1] = -1;
  accum->super.in_func = op_accum_inputs;
  accum->super.out = accum->outs;
  accum->super.opString = op_accum_opstring;
  accum->super.inString = op_accum_instring;
  accum->super.outString = op_accum_outstring;
  accum->super.type = eOpAccum; 
  accum->super.status = eUserOp;  
//  accum->super.size = sizeof(op_accum_t);
}
