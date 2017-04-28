#include <string.h>

#include "jansson.h"

#include "op.h"
#include "net_protected.h"
#include "preset.h"
#include "scene.h"

#include "json.h"

static void net_read_json_scene(json_t* o);
static void net_read_json_ops(json_t* o);
static void net_read_json_ins(json_t* o);
static void net_read_json_outs(json_t* o);
static void net_read_json_params(json_t* o);
static void net_read_json_presets(json_t* o);

// convert from minor version 3
static void net_json_convert_min3(json_t* r);

void net_read_json_raw(const char* name) {
  json_t *root;
  json_error_t err;
  FILE* f = fopen(name, "r");

  root = json_loadf(f, 0, &err);  
  fclose(f);

  json_t* scene = json_object_get(root, "scene");
  json_t* vers = json_object_get(scene, "beesVersion");
  int min = json_integer_value(json_object_get(vers, "min"));
  printf("\r\n got minor version number from json: %d", min);
  if(min == 3) {
    //    if(sceneData->desc.moduleVersion.min == 3) {
    printf(" !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! converting from 0.3");
    net_json_convert_min3(root);
  }
  
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
	bin[j] = (u8)(json_integer_value(json_array_get(state, j)));
      }
      src = bin;
      src = (*(op->unpickle))(op, src);
      // sanity check
      if(binCount != ((u32)src - (u32)bin)) {
	printf("warning! mis-sized byte array in operator state unpickle?");
	printf("\r\n   bin: 0x%08x ; src: 0x%08x ", (u32)bin, (u32)src);
      }
    }
  }
}

static void net_read_json_ins(json_t* o) {
  json_t* p;
  json_t* arr;
  int i;
  int count = json_integer_value(json_object_get(o, "count"));

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
    //    param->idx = json_integer_value(json_object_get(p, "idx"));
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


void net_json_convert_min3(json_t* r) {
 
  // ADC has changed i/o count from [2,2] to [3,2],
  // and likewise size of byte-arry of state data.

  // there is only one ADC operator, so this isn't too bad.
  
  /// magic number based on what 0.3.x scenes looked like.
  int lastAdcIn = 29;

  json_t* ins = json_object_get(r, "ins");
  json_t* insData = json_object_get(ins, "data");
  int insCount = json_integer_value(json_object_get(ins, "count"));
  json_integer_set(json_object_get(ins, "count"), insCount + 1);

  // need to fux 

  // all we should have to do for input nodes is: 
  // - insert a new node
  // - fix everyone's index
  json_t* o = json_object();
  json_object_set(o, "idx", json_integer(lastAdcIn));
  json_object_set(o, "name", json_string("MODE    "));
  json_object_set(o, "opIdx", json_integer(10));
  json_object_set(o, "opInIdx", json_integer(2));
  json_object_set(o, "value", json_integer(0));
  json_object_set(o, "play", json_integer(0));
  int err  = json_array_insert(insData, lastAdcIn + 1, o );

  if(err != 0) { printf(" error inserting input node in ADC op conversion."); }
  
  // loop over input nodes and fix idx fields
  for(int i=0; i<NET_INS_MAX; i++) {
    json_integer_set(json_object_get(json_array_get(insData, i), "idx"), i);
  }

  // we also have to add some dummy values to the byte array of the ADC op's state.
  json_t* ops = json_object_get(r, "ops");
  // 10 is the magic number of the ADC op in 0.3.x
  json_t* adc = json_array_get(json_object_get(ops, "data"), 10);
  // mode pickle: 2 bytes input value, zero is fine
  json_array_append(json_object_get(adc, "state"),  json_integer(0));
  json_array_append(json_object_get(adc, "state"),  json_integer(0));

  /* // copy all input nodes above this operator's... */
  /* // count down from top. src has fewer input nodes; otherwise we would lose last value. */
  /* for(int i = NET_INS_MAX - 1; i > lastAdcIn; i--) { */
  /*   // get json object at this index in node array */
  /*   json_t* dst = json_array_get(insData, i); */
  /*   json_t* src = json_array_get(insData, i - 1);     */
  /*   if(src != NULL) { */
  /*     if(dst == NULL) { */
  /* 	int err; */
  /* 	dst = json_object(); */
  /* 	json_object_set(dst, "idx", json_integer(i)); */
  /* 	err = json_array_insert(insData, i, dst); */
  /* 	if(err != 0) { printf(" error inserting input node in ADC op conversion."); } */
  /*     } */
  /*     // shallow copy ok? */
  /*     json_array_set(insData, src); */
  /*     // custom-copy, leaving idx */
  /*     /\* json_object_(dst, "name", json_object_get(src, "name")); *\/ */
  /*     /\* json_object_set(dst, "opIdx", json_object_get(src, "opIdx")); *\/ */
  /*     /\* json_object_set(dst, "opInIdx", json_object_get(src, "opInIdx")); *\/ */
  /*     /\* json_object_set(dst, "value", json_object_get(src, "value")); *\/ */
  /*     /\* json_object_set(dst, "play", json_object_get(src, "play")); *\/ */
  /*   } */
  /* } */
  /* // finally, we need to fix up the values for the new input ("mode");   */
  /* ///... FIXME */

  
  //---- -outs
  json_t* outs = json_object_get(r, "outs");
  json_t* outsData = json_object_get(outs, "data");
  // loop over all outputs and check targets
  for(int i = 0; i< NET_OUTS_MAX; i++) {
    json_t* o = json_array_get(outsData, i);
    int t = json_integer_value(json_object_get(o, "target"));
      if(t > lastAdcIn) {
	json_integer_set(json_object_get(o, "target"), t + 1);
      }
  }

  // i don't think anyone really used presets in this rev, so skipping those for now.

  // write the output of the converted json for a visual check.
  json_dump_file(r, "converted.json", JSON_INDENT(4) | JSON_PRESERVE_ORDER | JSON_ESCAPE_SLASH);
}
