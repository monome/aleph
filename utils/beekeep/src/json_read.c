#include <stdlib.h>

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
  net_read_json_scene(json_object_get(root, "ops"));
  net_read_json_scene(json_object_get(root, "ins"));
  net_read_json_scene(json_object_get(root, "outs"));
  net_read_json_scene(json_object_get(root, "params"));
  net_read_json_scene(json_object_get(root, "presets"));
}


static void net_read_json_scene(json_t* o) {
  strcpy(sceneData->desc.sceneName, json_string_value(json_object_get(o, "sceneName")));
  strcpy(sceneData->desc.moduleName, json_string_value(json_object_get(o, "moduleName")));
    
    
}

static void net_read_json_ops(json_t* o) { 

}

static void net_read_json_ins(json_t* o) {
}

static void net_read_json_outs(json_t* o) {
}

static void net_read_json_params(json_t* o) {
}

static void net_read_json_presets(json_t* o) {
}
