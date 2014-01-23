#include "jansson.h"

#include "net_protected.h"
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
  json_t* p = json_object();

  json_object_set(o, "sceneName", json_string(sceneData->desc.sceneName)); 
  json_object_set(o, "moduleName", json_string(sceneData->desc.moduleName)); 
  json_object_set(p, "min", json_integer(sceneData->desc.moduleVersion.min));
  json_object_set(p, "maj", json_integer(sceneData->desc.moduleVersion.min));
  json_object_set(p, "rev", json_integer(sceneData->desc.moduleVersion.rev));
  json_object_set(o, "moduleVersion", p);
  json_object_set(p, "min", json_integer(sceneData->desc.beesVersion.min));
  json_object_set(p, "maj", json_integer(sceneData->desc.beesVersion.min));
  json_object_set(p, "rev", json_integer(sceneData->desc.beesVersion.rev));
  json_object_set(o, "beesVersion", p);  
  
  return o;
}

static json_t* net_write_json_ops(void) { 
  json_t* ops = json_object();
  return ops;

}

static json_t* net_write_json_ins(void) {
  json_t* ins = json_object();
  return ins;
}

static json_t* net_write_json_outs(void) {
  json_t* outs = json_object();
  return outs;
}

static json_t* net_write_json_params(void) {
  json_t *params = json_object();
  json_t *l = json_array();
  json_t* o = json_object();
  int i;

  json_object_set(params, "count", json_integer(net->numParams));
  
  for(i=0; i<net->numParams; i++) {
    json_object_set(o, "label", json_string(net->params[i].desc.label));
    json_object_set(o, "value", json_integer(net->params[i].data.value));
    json_array_append(l, o);
  }
  json_object_set(params, "data", l);

  return params;
}

static json_t* net_write_json_presets(void) {
  json_t* presets = json_object();

  return presets;
}
