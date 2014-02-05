#include "json.h"

static json_t* net_write_json_max_ops(void);
static json_t* net_write_json_max_ins(void);
static json_t* net_write_json_max_outs(void);
static json_t* net_write_json_max_params(void);

static json_t* boxes;
static json_t* lines;

void net_write_json_max(const char* name) {
  json_t *root;
  json_t* patcher;

  root = json_object();
  patcher = json_object();

  boxes = json_array();
  lines = json_array();


  json_object_set(root, "patcher", patcher);
  json_object_set(patcher, "boxes", boxes);
  json_object_set(patcher, "lines", lines);

  /// populate operators
  net_write_json_max_ops();
  /// populate inputs
  net_write_json_max_ins();
  /// populate params 
  net_write_json_max_params();
  /// populate outputs (patchlines)
  net_write_json_max_outs();
  
  json_dump_file(root, name, JSON_INDENT(4) | JSON_PRESERVE_ORDER | JSON_ESCAPE_SLASH);  
  
}


json_t* net_write_json_max_ops(void) {
}

json_t* net_write_json_max_ins(void) {
}

json_t* net_write_json_max_outs(void) {
}

json_t* net_write_json_max_params(void) {
}
