#ifndef _BEEKEEP_UI_HANDLERS_H_
#define _BEEKEEP_UI_HANDLERS_H_

#include "ui.h"

// list selection handlers
extern void ui_select_op(int id);
extern void ui_select_out(int id);
extern void ui_select_in(int id);
extern void ui_select_param(int id);
extern void ui_select_preset(int id);

//--- value handlers
/* TODO
extern void ui_in_value(int id);
extern void ui_param_value(int id);
*/

//--- button handlers
// connections (toggle)
extern void ui_connect_in(void);
extern void ui_connect_param(void);

// preset inclusion (toggle)
extern void ui_preset_in(void);
extern void ui_preset_out(void);

// export/load
extern void ui_export_scn(const char* name);
extern void ui_export_json(const char* name);
extern void ui_import_scn(const char* name);
extern void ui_import_json(const char* name);

// create/destroy operator
extern void ui_create_op(void);
extern void ui_delete_op(void);

//--- other

// scene name
extern void ui_scene_name(const char* txt);
// new-op-type selection
extern void ui_select_new_op(int id);
// input value change
extern void ui_set_input(int id, int val);
// param value change
extern void ui_set_param(int id, int val);

// toggle preset inclusion for input
extern void ui_toggle_preset_input(int id);
// store input value in preset
extern void ui_store_preset_input(int id);
// toggle preset inclusion for output
extern void ui_toggle_preset_output(int id);
// store output value in preset
extern void ui_store_preset_output(int id);

// set module by name
extern void ui_set_module(const char* name);

#endif
