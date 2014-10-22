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
extern void ui_in_value(int id);
extern void ui_param_value(int id);

//--- button handlers

// preset inclusion
extern void ui_in_preset_button(int id);
extern void ui_out_preset_button(int id);
// export/load
extern void ui_export_scn(const char* name);
extern void ui_export_json(const char* name);
extern void ui_import_scn(const char* name);
extern void ui_import_json(const char* name);

// create/destroy operator
extern void ui_create_op(void);
extern void ui_destroy_op(void);

//--- other

// handle scene name
extern void ui_scene_name(const char* txt);
// handle new-op selection
extern void ui_select_op(int id);

#endif
