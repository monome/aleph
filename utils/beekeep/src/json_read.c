#include <string.h>

#include "jansson.h"

#include "op.h"
#include "net_protected.h"
#include "preset.h"
#include "scene.h"

static void net_read_json_scene(json_t* o);
static void net_read_json_ops(json_t* o);
static void net_read_json_ins(json_t* o);
static void net_read_json_outs(json_t* o);
static void net_read_json_params(json_t* o);
static void net_read_json_presets(json_t* o);

void net_read_json(const char* name) {
  json_t *root;
  json_error_t err;
  FILE* f = fopen(name, "r");

  root = json_loadf(f, 0, &err);  
  fclose(f);
  
  net_read_json_scene(json_object_get(root, "scene"));
  net_read_json_ops(json_object_get(root, "ops"));
  net_read_json_ins(json_object_get(root, "ins"));
  net_read_json_outs(json_object_get(root, "outs"));
  net_read_json_params(json_object_get(root, "params"));
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
  int count = json_integer_value(json_object_get(o, "count"));
  int i, j;
  json_t* arr = json_object_get(o, "data");
  op_id_t id;
  op_t* op;
  json_t* p;
  /// binary state data..
  u8* src;
  u8 bin[0x10000];
  int binCount;

  // sanity check
  if(count != json_array_size(arr)) {
    printf("\r\n warning, mismatched count / size in ops list ( %d / %d )", count, (int)json_array_size(arr));
  }

  // clear out any extant user ops in the network
  net_deinit();

  for( i=0; i<count; i++) {
    p = json_array_get(arr, i);
    id = (op_id_t)json_integer_value(json_object_get(p, "type"));
    // add operator of indicated type
    net_add_op(id);
    // unpickle the state, if needed
    op = net->ops[net->numOps - 1];
    
    if(op->unpickle != NULL) {
      json_t* state = json_object_get(p, "state");
      binCount = json_array_size(state);
      for(j=0; j<binCount; j++) {
	bin[j] = json_integer_value(json_array_get(state, j));
      }
      src = bin;
      src = (*(op->unpickle))(op, src);
      // sanity check
      if(binCount != (src - bin)) {
	printf("warning! mis-sized byte array in operator state unpickle?");
      }
    }
  }
}

static void net_read_json_ins(json_t* o) {
  json_t* p;
  json_t* arr;
  int i;
  int count = json_integer_value(json_object_get(o, "count"));
  if(net->numIns != count) {
    printf("\r\n warning! input count does not match");
  }
  arr = json_object_get(o, "data");
  for(i=0; i<count; i++) {
    p = json_array_get(arr, i);
    // should only need the play flag, since input list was populated from op creation, 
    // and values should be part of op pickle.
    net->ins[i].play = (u8)json_integer_value(json_object_get(p, "play"));
    // however, let's sanity-check the other fields...
    ///...... (yeah right)
  }
}

static void net_read_json_outs(json_t* o) {
  json_t* p;
  json_t* arr;
  int i;
  int v;
  int count = json_integer_value(json_object_get(o, "count"));
  if(net->numIns != count) {
    printf("\r\n warning! output count does not match");
  }
  arr = json_object_get(o, "data");
  for(i=0; i<count; i++) {
    p = json_array_get(arr, i);
  // output target
    v = json_integer_value(json_object_get(p, "target"));
    net->outs[i].target = (s16)v;

    // shouldn't really need to set these... 
    v = json_integer_value(json_object_get(p, "opIdx"));
    net->outs[i].opIdx = (u8)v;
    v = json_integer_value(json_object_get(p, "opOutIdx"));
    net->outs[i].opOutIdx = (s32)v;
  }
}

static void net_read_json_params(json_t* o) {
  json_t* p;
  json_t* arr;
  int i;
  int v;
  pnode_t* param;
  int count = json_integer_value(json_object_get(o, "count"));
  net->numParams = count;
  arr = json_object_get(o, "data");
  for(i=0; i<count; i++) {
    p = json_array_get(arr, i);
    param = &(net->params[i]);
    param->idx = json_integer_value(json_object_get(p, "idx"));
    param->data.value = json_integer_value(json_object_get(p, "value"));
    param->desc.type = json_integer_value(json_object_get(p, "type"));
    param->desc.min = json_integer_value(json_object_get(p, "min"));
    param->desc.max = json_integer_value(json_object_get(p, "max"));
    strcpy(param->desc.label, json_string_value(json_object_get(p, "label")));
  }
}

static void net_read_json_presets(json_t* o) {
  json_t* pres; // toplevel array of presets
  json_t* arr; // temp array of ins/outs per preset
  json_t* p; // tmp
  json_t* q; // tmp
  int i, j;
  int count = json_integer_value(json_object_get(o, "count"));
  int n; // tmp
  pres = json_object_get(o, "data");
  for(i=0; i<count; i++) {
    p = json_array_get(pres, i);
    strcpy(presets[i].name, json_string_value(json_object_get(p, "name")));
    /// ins
    arr = json_object_get(p, "ins");
    n = json_array_size(arr);
    for(j=0; j<n; j++) {
      /// FIXME: shouldn't need idx here
      q = json_array_get(arr, j);
      //      presets[i].ins[j].idx = json_integer_value(json_object_get(q, "idx"));
      presets[i].ins[j].value = json_integer_value(json_object_get(q, "value"));
      presets[i].ins[j].enabled = json_integer_value(json_object_get(q, "enabled"));
    }
    /// outs
    arr = json_object_get(p, "outs");
    n = json_array_size(arr);
    for(j=0; j<n; j++) {
      /// FIXME: shouldn't need idx here
      q = json_array_get(arr, j);
      //      presets[i].outs[j].outIdx = json_integer_value(json_object_get(q, "idx"));
      presets[i].outs[j].target = json_integer_value(json_object_get(q, "target"));
      presets[i].outs[j].enabled = json_integer_value(json_object_get(q, "enabled"));
    }
  }
}
