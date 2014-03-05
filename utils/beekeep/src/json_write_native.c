/*
  json_write_native.c

  beekeep
  aleph

  this module writes scene data as .json in our own "readable" format.

*/

#include "jansson.h"

#include "op.h"
#include "net_protected.h"
#include "param.h"
#include "preset.h"
#include "scene.h"


static json_t* net_write_json_scene(void);
static json_t* net_write_json_ops(void);
static json_t* net_write_json_ins(void);
static json_t* net_write_json_outs(void);
static json_t* net_write_json_params(void);
static json_t* net_write_json_presets(void);


void net_write_json_native(const char* name) {
  json_t *root;

  root = json_object();

  json_object_set(root, "scene", net_write_json_scene());
  json_object_set(root, "operators", net_write_json_ops());
  json_object_set(root, "params", net_write_json_params());
  /// ins and outs are stored hierarchichally in the ops list.
  //  json_object_set(root, "ins", net_write_json_ins());
  //  json_object_set(root, "outs", net_write_json_outs());
  json_object_set(root, "presets", net_write_json_presets());

  json_dump_file(root, name, JSON_INDENT(4) | JSON_PRESERVE_ORDER | JSON_ESCAPE_SLASH);  
}


static json_t* net_write_json_scene(void) {
  json_t* o = json_object(); 
  json_t* p;

  json_object_set(o, "sceneName", json_string(sceneData->desc.sceneName));
  json_object_set(o, "moduleName", json_string(sceneData->desc.moduleName));
  p = json_object();
  json_object_set(p, "maj", json_integer(sceneData->desc.moduleVersion.maj));
  json_object_set(p, "min", json_integer(sceneData->desc.moduleVersion.min));
  json_object_set(p, "rev", json_integer(sceneData->desc.moduleVersion.rev));
  json_object_set(o, "moduleVersion", p);
  p = json_object();
  json_object_set(p, "maj", json_integer(sceneData->desc.beesVersion.maj));
  json_object_set(p, "min", json_integer(sceneData->desc.beesVersion.min));
  json_object_set(p, "rev", json_integer(sceneData->desc.beesVersion.rev));
  json_object_set(o, "beesVersion", p);  
  return o;
}


static json_t* net_write_json_ops(void) { 
  json_t* ops = json_array();
  int i, j;
  // binary blob for operator state
  // a large but arbitrary maximum size!
  u8 bin[0x10000];
  u8* dst;
  int binCount;

  for(i=0; i<net->numOps; ++i) {
    op_t* op = net->ops[i];
    // tmp objets
    json_t* o;
    json_t* p;
    json_t* q;
    // list
    json_t* ins = json_array();
    json_t* outs = json_array();
    json_t* state = json_array();

    o = json_object();
    json_object_set(o, "index", json_integer(i));
    json_object_set(o, "class", json_integer(op->type));
    json_object_set(o, "name", json_string(op->opString));
    // write inputs
    for(j=0; j< (op->numInputs); ++j) {
      // index in global input array
      int inIdx = net_op_in_idx(i, j);
      p = json_object();
      json_object_set(p, "name", json_string( net_in_name(inIdx) ) );
      json_object_set(p, "value", json_integer(net_get_in_value(inIdx)) );
      json_array_append(ins, p);
    }
    // write outputs
    for(j=0; j<op->numOutputs; ++j) {
      // index in global input array
      int outIdx = net_op_out_idx(i, j);
      // target index
      int target = net_get_target(outIdx);
      p = json_object();
      q = json_object();
      // output objects include full descriptors for targets:
      // input idx, input name, op idx, op name
      json_object_set(p, "name", json_string( net_out_name(outIdx) ) );
      // target object
      if(target == -1) {
	// no target, short descriptor with index
	json_object_set(q, "inIdx", json_integer( -1 ) );
      } else {
	// has target
	int inOpIdx = net_in_op_idx(target);
	if(inOpIdx < 0) {
	  // target is param, short descriptor with param name
	  json_object_set( q, "paramName", json_string( net_in_name(target) ) );
	} else {
	  // target is op input, long descriptor
	  json_object_set( q, "opIdx", json_integer( inOpIdx ) );
	  json_object_set( q, "opName", json_string( net_op_name(net_in_op_idx(target)) ) );
	  json_object_set( q, "opInIdx", json_integer( net->ins[target].opInIdx ) );
	  json_object_set( q, "opInName", json_string( net_in_name(target) ) );
	}

      }
      json_object_set(p, "target", q);
      json_array_append(outs, p);
    }
    // op state is still just a binary blob, sorry!
    // eventually, we should have per-class json creation methods... ech
    if(op->pickle == NULL) {
      // no state
      binCount = 0;
    } else {
      dst = bin;
      dst = (*(op->pickle))(op, dst);
      binCount = (size_t)dst - (size_t)(bin);
    }
    for(j=0; j<binCount; j++) {
      json_array_append(state, json_integer(bin[j]));
    }
    json_object_set(o, "ins", ins);
    json_object_set(o, "outs", outs);
    json_object_set(o, "state", state);
    json_array_append(ops, o);
  }
  return ops;
}


static json_t* net_write_json_params(void) {
  json_t *params = json_array();
  json_t* o;
  int i;

  json_object_set(params, "count", json_integer(net->numParams));
  
  for(i=0; i<net->numParams; i++) {
    o = json_object();
    
    json_object_set(o, "name", json_string(net->params[i].desc.label));
    json_object_set(o, "type", json_integer(net->params[i].desc.type));
    json_object_set(o, "min", json_integer(net->params[i].desc.min));
    json_object_set(o, "max", json_integer(net->params[i].desc.max));
    json_object_set(o, "radix", json_integer(net->params[i].desc.radix));
    json_object_set(o, "value", json_integer(net->params[i].data.value));
    /// FIXME: this dumb indexing. play flag not stored correctly...
    json_object_set(o, "play", json_boolean(net_get_in_play(i + net->numIns)));
    json_array_append(params, o);
  }

  return params;
}

static json_t* net_write_json_presets(void) {
  json_t* pres = json_array();
  int i, j;

  for(i=0; i<NET_PRESETS_MAX; i++) {
    // object for this preset
    json_t* pre = json_object();
    // array for this preset's entries
    json_t* l = json_array();
    // tmp
    json_t* o;
    json_t* p;

    json_object_set( pre, "name", json_string( preset_name(i)) );
    
    // loop over nodes.
    // only create entries for enabled nodes.
    for(j=0; j<PRESET_INODES_COUNT; j++) {
      if(presets[i].ins[j].enabled) {
	o = json_object();
	if(j < net->numIns) {
	  // op input 
	  // cheating and using net handle directly...
	  json_object_set( o, "opIdx", json_integer( net->ins[j].opIdx ) );
	  json_object_set( o, "opName", json_string( net_op_name(net->ins[j].opIdx) ) );
	  json_object_set( o, "opInName", json_string( net_in_name(j) ) );
	  json_object_set( o, "value", json_integer( presets[i].ins[j].value ) );
	} else {
	  // parameter
	  int pId = j - net->numIns;
	  json_object_set( o, "paramName", json_string ( get_param_name(pId) ) ) ; 
	  json_object_set( o, "value", json_integer( presets[i].ins[j].value ) );
	}
	json_array_append(l, o);	
      }
    }

    for(j=0; j<NET_OUTS_MAX; j++) {
      if(presets[i].outs[j].enabled) {
	int target;
	o = json_object();
	p = json_object();

	target = presets[i].outs[j].target;
	if(target < 0) {
	  // disconnect
	  json_object_set(p, "inIdx", json_integer( -1 ) );
	} else {
	  if(target >= net->numIns) {
	    // parameter target
	    json_object_set( p, "paramName", json_string( get_param_name(target - net->numIns) ) ) ;
	  } else {
	    // op input target
	    json_object_set( p, "opIdx", json_integer( net->ins[target].opIdx ) ) ;
	    json_object_set( p, "opName", json_string( net_op_name(net->ins[target].opIdx) ) );
	    json_object_set( p, "opInName", json_string( net_in_name(target) ) );
	  }
	  json_object_set( o, "opIdx", json_integer( net_out_op_idx(j) ) );
	  json_object_set( o, "opName", json_string( net_op_name(net_out_op_idx(j) ) ) );
	  json_object_set( o, "opOutName", json_string( net_out_name(j) ) );
	  json_object_set( o, "target", p);
	  json_array_append( l, o ); 
	}
      }    
    }
    
    json_object_set(pre, "entries", l);
    json_array_append(pres, pre);

  }
  return pres; 
}
