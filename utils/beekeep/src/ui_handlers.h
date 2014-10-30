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

// handle scene name
extern void ui_scene_name(const char* txt);

// handle new-op-type selection
extern void ui_select_new_op(int id);

// handle input value change
extern void ui_set_input(int id, int val);

#endif
