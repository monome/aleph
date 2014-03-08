#include <string.h>

#include "jansson.h"

#include "op.h"
#include "net_protected.h"
#include "param.h"
#include "preset.h"
#include "scene.h"

#include "json.h"

static void net_read_json_scene(json_t* o);
static void net_read_json_ops(json_t* o);
static void net_read_json_params(json_t* o);
static void net_read_json_presets(json_t* o);

// search for input idx, given op id and input name
static int search_op_input(int opIdx, const char* name);
// search for output idx, given op id and input name
static int search_op_output(int opIdx, const char* name);
// search for param idx, given name
static int search_param(const char* name);


void net_read_json_native(const char* name) {
  json_t *root;
  json_error_t err;
  FILE* f = fopen(name, "r");

  root = json_loadf(f, 0, &err);
  fclose(f);

  json_t* scene = json_object_get(root, "scene");
  net_read_json_scene(scene);


  // clear out extant user ops / params
  net_deinit();
  // params before ops, so target assignment can work
  net_read_json_params(json_object_get(root, "params"));
  net_read_json_ops(json_object_get(root, "operators"));
  net_read_json_presets(json_object_get(root, "presets"));
}


static void net_read_json_scene(json_t* o) {
  json_t* p;
  strcpy(sceneData->desc.sceneName, json_string_value(json_object_get(o, "sceneName")));
  strcpy(sceneData->desc.moduleName, json_string_value(json_object_get(o, "moduleName")));
  // module version
  p = json_object_get(o, "moduleVersion");
  sceneData->desc.moduleVersion.maj = json_integer_value(json_object_get(p, "maj"));
  sceneData->desc.moduleVersion.min = json_integer_value(json_object_get(p, "min"));
  sceneData->desc.moduleVersion.rev = json_integer_value(json_object_get(p, "rev"));
  // bees version
  p = json_object_get(o, "beesVersion");
 sceneData->desc.beesVersion.maj = json_integer_value(json_object_get(p, "maj"));
  sceneData->desc.beesVersion.min = json_integer_value(json_object_get(p, "min"));
  sceneData->desc.beesVersion.rev = json_integer_value(json_object_get(p, "rev"));
 
}

static void net_read_json_ops(json_t* o) { 
  int count = json_array_size(o);
  /// binary state data..
  u8* src;
  u8 bin[0x10000];
  int binCount;
  int i, j;
  int id;
  op_t* op;
  int opIdx, opInIdx, inIdx;

  for( i=0; i<count; i++) {
    json_t* p = json_array_get(o, i);
    json_t* q;
    json_t* r;
    id = (op_id_t)json_integer_value(json_object_get(p, "class"));
    // add operator of indicated type
    net_add_op(id);
    op = net->ops[net->numOps - 1];

    /// set inputs
    q = json_object_get(p, "ins");
    for(j=0; j< op->numInputs; j++) {
      r = json_array_get(q, j);
      op_set_in_val(op, j, json_integer_value(json_object_get(r, "value")));
    }

    // unpickle state
    if(op->unpickle != NULL) {
      json_t* state = json_object_get(p, "state");
      binCount = json_array_size(state);
      for(j=0; j<binCount; j++) {
  	bin[j] = (u8)(json_integer_value(json_array_get(state, j)));
      }
      src = bin;
      src = (*(op->unpickle))(op, src);
      // sanity check
      if(binCount != ((size_t)src - (size_t)bin)) {
  	printf("warning! mis-sized byte array in operator state unpickle?");
  	printf("\r\n   bin: 0x%08x ; src: 0x%08x ", (unsigned int)bin, (unsigned int)src);
      }
    }
  }

  // perform another loop for outputs, now that all ops have been created
  for( i=0; i<count; i++) {
    json_t* p = json_array_get(o, i);
    json_t* q;
    json_t* r;
    json_t* s;

    op = net->ops[i];
    q = json_object_get(p, "outs");
    for(j=0; j< op->numOutputs; j++) {
      r = json_object_get(json_array_get(q, j), "target");

      printf("\r\n parsing operator target at op idx %d, out idx %d", 
	     i, j);
      // switch on target representations...
      //...
      s = json_object_get(r, "inIdx");      
      printf(", checking input idx...");
      if (s != NULL) {
	// target is raw input idx
	inIdx =json_integer_value(s);
	if(inIdx == -1) { 
	  printf(", no target");
	} else {
	  printf(" , target is raw input index: %d", inIdx);
	}
	if(inIdx > -1) {
	  net_connect(net_op_out_idx(i, j), inIdx);
	}
	continue;
      }
      //...
      s = json_object_get(r, "paramName");
      printf(", checking param name...");
      if(s != NULL) {
	// target is param name
	printf(" , target is parameter name: %s", json_string_value(s));
	net_connect(
		    net_op_out_idx(i,j),
		    search_param(json_string_value(s))
		    );
	continue;
      }
      //...
      s = json_object_get(r, "opIdx");
      printf(", checking op idx...");
      if(s != NULL) {
	// target is op and input idx
	opIdx = json_integer_value(json_object_get(r, "opIdx") );
	opInIdx = json_integer_value(json_object_get(r, "opInIdx") );
	inIdx = net_op_in_idx(opIdx, opInIdx);

	printf(" , target is op input; raw index: %d", inIdx);

	net_connect(net_op_out_idx(i, j), inIdx);
	continue;
      }
    }
  }
}

static void net_read_json_params(json_t* o) {
  int i;
  int v;
  pnode_t* param;
  int count = json_array_size( o );
  //  json_t* arr;

  net->numParams = count;
  //  arr = json_object_get(o, "data");
  for(i=0; i<count; i++) {
    json_t* p = json_array_get(o, i);
    param = &(net->params[i]);
    //    param->idx = json_integer_value(json_object_get(p, "idx"));
    param->data.value = json_integer_value(json_object_get(p, "value"));
    param->desc.type = json_integer_value(json_object_get(p, "type"));
    param->desc.min = json_integer_value(json_object_get(p, "min"));
    param->desc.max = json_integer_value(json_object_get(p, "max"));
    param->desc.radix = json_integer_value(json_object_get(p, "radix"));
    strcpy(param->desc.label, json_string_value(json_object_get(p, "name")));
    //ahh... inputs aren't set up yet here. 
    //    net_set_in_play(i + net->numIns, json_integer_value(json_object_get(p, "play") ) );
    net->params[i].play = json_is_true(json_object_get(p, "play") );
  }
}

static void net_read_json_presets(json_t* o) {
  int count = json_array_size( o );
  int n;
  int i, j;
  char* p;

  json_t* r;
  json_t* s;
  int opIdx;
  int opInIdx, inIdx;
  int opOutIdx, outIdx;
  char name[32];


  // sanity check
  if( count != NET_PRESETS_MAX) {
    printf(" \n warning! preset array size in json does not match network preset count.\n");
  }

  // empty out extant preset data
  for(i=0; i<NET_PRESETS_MAX; i++) {
    // empty name
    for(j=0; j<PRESET_NAME_LEN; j++) {
      presets[i].name[j] = '\0';
    }
    // empty inputs
    for(j=0; j<PRESET_INODES_COUNT; ++j) {
      presets[i].ins[j].value = 0;
      presets[i].ins[j].enabled = 0;
    }
    // empty outputs
    for(j=0; j<NET_OUTS_MAX; ++j) {
      presets[i].outs[j].target = -1;
      presets[i].outs[j].enabled = 0;
    }
  }

  for(i=0; i<count; ++i) {
    json_t* p = json_array_get(o, i);
    json_t* arr = json_object_get(p, "entries");
    strcpy(presets[i].name, json_string_value(json_object_get(p, "name")));
    n = json_array_size(arr);

    printf("\r\n processing preset name: %s , %d entries", presets[i].name, n);

    for(j=0; j<n; ++j) {
      json_t* q = json_array_get(arr, j);
      printf("\r\n processing preset %s , entry %d, contents: %s", 
	     presets[i].name, j, json_dumps(q, 0));
      
      /*
	ok... here we need to do some parsing stuff,
        because there are a number of possible formats for preset entries.

	initially, let's just assume the format used in beekeep:json_write_native()
	
       */

      // input node preset entry
      ///////////////////
      r = json_object_get(q, "inIdx");
      if(r != NULL) {
	/// this is an input node with raw index
	// set value
	inIdx = json_integer_value(r);
	presets[i].ins[inIdx].value = json_integer_value(json_object_get(q, "value"));
	presets[i].ins[inIdx].enabled = 1;
	continue;
      }
      ////////////////////
      r = json_object_get(q, "opInName");
      if(r != NULL) {
	/// this is an input node with input name and op idx
	r = json_object_get(q, "opIdx");
	if(r == NULL) { 
	  printf("error parsing preset entry (input), preset no. %d", i);
	  continue;
	}
	opIdx = json_integer_value(r);
	r = json_object_get(q, "opInName");
	if(r == NULL) { 
	  printf("error parsing preset entry (input), preset no. %d", i);
	  continue;
	}
	strcpy(name, json_string_value(r) );
	opInIdx = search_op_input(opIdx, name);
	if(opInIdx == -1) { 
	  printf("error parsing preset (input), preset no. %d - couldn't find named input", i);
	  continue;
	}
	// set value
	r = json_object_get(q, "value");
	inIdx = net_op_in_idx(opIdx, opInIdx);
	printf("\r\n assigning preset input value, preset %d, input %d, value %d",
	       i, inIdx, json_integer_value(r));

	presets[i].ins[inIdx].value = (io_t)(json_integer_value(r));
	presets[i].ins[inIdx].enabled = 1;
	continue;
      }
      //////////////////
      r = json_object_get(q, "opOutName");
      if(r != NULL) {
	/// this is an output node with output name and op idx
	strcpy(name, json_string_value(r) );
	opIdx = json_integer_value( json_object_get(q, "opIdx") );
	opOutIdx = search_op_output(opIdx, name);
	outIdx = net_op_out_idx(opIdx, opOutIdx);
	// get target data...
	r = json_object_get(q, "target");
	s = json_object_get(r, "paramName");
	if(s != NULL) {
	  // param target
	  strcpy(name, json_string_value(s) );
	  inIdx = search_param(name);
	} else {
	  // op input target (or so we assume...)
	  s = json_object_get(r, "opIdx");
	  opIdx = json_integer_value(s);
	  s = json_object_get(r, "opInName");
	  strcpy(name, json_string_value(s) );
	  inIdx = net_op_in_idx(opIdx, search_op_input(opIdx, name));
	}
	if(inIdx == -1) { printf("error parsing target in preset %d", i); continue; }
	printf("\r\n assigning preset output value, preset %d, output %d, target %d",
	       i, outIdx, inIdx);
	presets[i].outs[outIdx].target = inIdx;
	presets[i].outs[outIdx].enabled = 1;
	continue;
      }
      //////////
      r = json_object_get(q, "paramName");
      if(r != NULL) {
	/// this is a param entry
	strcpy(name, json_string_value(r));
	inIdx = search_param(name);
	r = json_object_get(q, "value");
	printf("\r\n assigning preset param value, preset %d, input %d, value %d",
	       i, inIdx, json_integer_value(r));
	presets[i].ins[inIdx].value = json_integer_value(r);
	presets[i].ins[inIdx].enabled = 1;
	continue;
      }
    }
  }
}

// search for input idx, given op id and input name
int search_op_input(int opIdx, const char* name) {
  int i;
  op_t* op = net->ops[opIdx];
  for(i=0; i < op->numInputs; ++i) {
    if(strcmp(op_in_name(op, i), name) == 0) {
      return i;
    }
  }
  return -1;
}

// search for output idx, given op id and input name
int search_op_output(int opIdx, const char* name) {
  int i;
  op_t* op = net->ops[opIdx];
  for(i=0; i < op->numOutputs; ++i) {
    if(strcmp(op_out_name(op, i), name) == 0) {
      return i;
    }
  }
  return -1;
}

// search for param idx (in inputs list), given name
static int search_param(const char* name) {
  int i;
  for(i=0; i < net_num_params(); ++i) {
    if( strcmp( get_param_name(i), name) == 0) {
      // offset into global input list
      return i + net->numIns;
    }
  }
  return -1;
}
