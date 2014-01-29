#include "jansson.h"

#include "op.h"
#include "net_protected.h"
#include "preset.h"
#include "scene.h"


static json_t* net_write_json_scene(void);
static json_t* net_write_json_ops(void);
static json_t* net_write_json_ins(void);
static json_t* net_write_json_outs(void);
static json_t* net_write_json_params(void);
static json_t* net_write_json_presets(void);


void net_write_json(const char* name) {
  json_t *root;

  root = json_object();

  json_object_set(root, "scene", net_write_json_scene());
  json_object_set(root, "ops", net_write_json_ops());
  json_object_set(root, "ins", net_write_json_ins());
  json_object_set(root, "outs", net_write_json_outs());
  json_object_set(root, "params", net_write_json_params());
  json_object_set(root, "presets", net_write_json_presets());

  json_dump_file(root, name, JSON_INDENT(4) | JSON_PRESERVE_ORDER | JSON_ESCAPE_SLASH);  
}


static json_t* net_write_json_scene(void) {
  //  json_t* scene = json_object();
  json_t* o = json_object();
  json_t* p;

  json_object_set(o, "sceneName", json_string(sceneData->desc.sceneName)); 
  json_object_set(o, "moduleName", json_string(sceneData->desc.moduleName)); 
  p = json_object();
  json_object_set(p, "min", json_integer(sceneData->desc.moduleVersion.maj));
  json_object_set(p, "maj", json_integer(sceneData->desc.moduleVersion.min));
  json_object_set(p, "rev", json_integer(sceneData->desc.moduleVersion.rev));
  json_object_set(o, "moduleVersion", p);
  p = json_object();
  json_object_set(p, "min", json_integer(sceneData->desc.beesVersion.maj));
  json_object_set(p, "maj", json_integer(sceneData->desc.beesVersion.min));
  json_object_set(p, "rev", json_integer(sceneData->desc.beesVersion.rev));
  json_object_set(o, "beesVersion", p);  
  
  return o;
}

static json_t* net_write_json_ops(void) { 
  json_t* ops = json_object();
  json_t* o;
  json_t* l = json_array();
  op_t* op;
  int i, j;
  // binary blob for operator state
  // a large but arbitrary maximum size!
  u8 bin[0x10000];
  int binCount;
  json_t* state;
  u8* dst;

  json_object_set(ops, "count", json_integer(net->numOps));

  for(i=0; i<net->numOps; i++) {
    o = json_object();
    op = net->ops[i];
    json_object_set(o, "idx", json_integer(i));
    json_object_set(o, "type", json_integer(op->type));
    json_object_set(o, "name", json_string(op->opString));
    json_object_set(o, "numIns", json_integer(op->numInputs));
    json_object_set(o, "numOuts", json_integer(op->numInputs));
    /// ok, operator state data is going to be weird.
    /// we could write a proper json parser for each operator type (insane.)
    /// but for now i am just going to use the operator pickling/unpickling functions,
    /// and stick an ugly byte-array in the json. sorry!
    if(op->pickle == NULL) {
      // no state
      binCount = 0;
    } else {
      dst = bin;
      dst = (*(op->pickle))(op, dst);
      binCount = (u32)dst - (u32)(bin);
    }
    state = json_array();
    for(j=0; j<binCount; j++) {
      json_array_append(state, json_integer(bin[j]));
    }
    json_object_set(o, "state", state);
    json_array_append(l, o);
  }
  json_object_set(ops, "data", l);
  return ops;
}

static json_t* net_write_json_ins(void) {

  json_t* ins = json_object();
  json_t* l = json_array();
  json_t* o;
  int i;
  json_object_set(ins, "count", json_integer(net->numIns));

  for(i=0; i<net->numIns; i++) {
    o = json_object();
    json_object_set(o, "idx", json_integer(i));

    json_object_set(o, "name", json_string(net_in_name(i)));
    json_object_set(o, "opIdx", json_integer(net->ins[i].opIdx));
    json_object_set(o, "opInIdx", json_integer(net->ins[i].opInIdx));
    json_object_set(o, "value", json_integer(net_get_in_value(i)));
    json_object_set(o, "play", json_boolean(net_get_in_play(i)));
    json_array_append(l, o);
  }
  json_object_set(ins, "data", l);
  return ins;
}

static json_t* net_write_json_outs(void) {
  json_t* outs = json_object();
  json_t* l = json_array();
  json_t* o;
  int i;
  json_object_set(outs, "count", json_integer(net->numOuts));

  for(i=0; i<net->numOuts; i++) {
    o = json_object();
    json_object_set(o, "idx", json_integer(i));
    json_object_set(o, "opIdx", json_integer(net->outs[i].opIdx));
    json_object_set(o, "opOutIdx", json_integer(net->outs[i].opOutIdx));
    json_object_set(o, "name", json_string(net_out_name(i)));
    json_object_set(o, "target", json_integer(net_get_target(i)));
    json_array_append(l, o);
  }
  json_object_set(outs, "data", l);

  return outs;
}

static json_t* net_write_json_params(void) {
  json_t *params = json_object();
  json_t *l = json_array();
  json_t* o;
  int i;

  json_object_set(params, "count", json_integer(net->numParams));
  
  for(i=0; i<net->numParams; i++) {
    o = json_object();
    json_object_set(o, "idx", json_integer(i));
    json_object_set(o, "label", json_string(net->params[i].desc.label));
    json_object_set(o, "type", json_integer(net->params[i].desc.type));
    json_object_set(o, "min", json_integer(net->params[i].desc.min));
    json_object_set(o, "max", json_integer(net->params[i].desc.max));

    json_object_set(o, "value", json_integer(net->params[i].data.value));
    /// FIXME: this dumb indexing. play flag not stored correctly...
    json_object_set(o, "play", json_boolean(net_get_in_play(i + net->numIns)));
    json_array_append(l, o);
  }
  json_object_set(params, "data", l);

  return params;
}

static json_t* net_write_json_presets(void) {
  json_t* pres = json_object();
  json_t* ins;
  json_t* outs;
  json_t* l;
  json_t* m;
  json_t* o;
  json_t* p;
  int i, j;

  json_object_set(pres, "count", json_integer(NET_PRESETS_MAX));
  m = json_array();

  for(i=0; i<NET_PRESETS_MAX; i++) {
    p = json_object();
    json_object_set(p, "name", json_string( preset_name(i)) );
    l = json_array();
    for(j=0; j<PRESET_INODES_COUNT; j++) {
      /// 
      o = json_object();
      json_object_set(o, "enabled", json_integer( presets[i].ins[j].enabled ));
      /// FIXME: shouldn't need idx here
      //      json_object_set(o, "idx", json_integer( presets[i].ins[j].idx ));
      /// store for readibility anyhow
      json_object_set(o,"idx", json_integer( j ));
      json_object_set(o, "value", json_integer( presets[i].ins[j].value ));
      json_array_append(l, o);
    }
    json_object_set(p, "ins", l);

    for(j=0; j<NET_OUTS_MAX; j++) {
      o = json_object();
      /// FIXME: shouldn't need idx here
      //      json_object_set(o, "idx", json_integer( presets[i].outs[j].outIdx ));
      /// store for readibility anyhow
      json_object_set(o,"idx", json_integer( j ));
      json_object_set(o, "target", json_integer( presets[i].outs[j].target ));
      json_object_set(o, "enabled", json_integer( presets[i].outs[j].enabled ));
      json_array_append(l, o);
    }
    json_object_set(pres, "outs", l);
    json_array_append(m, p);
  }
  json_object_set(pres, "data", m);
  return pres;
}
