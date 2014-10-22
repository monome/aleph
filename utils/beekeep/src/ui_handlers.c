// bees
#include "op.h"
// beekeep
#include "ui.h"
#include "ui_handlers.h"

extern void ui_select_op(int id) {
  newOpSelect = (op_id_t)id;
  printf("\r\n selected operator type: %s", op_registry[newOpSelect].name);
}

extern void ui_select_out(int id) {
}

extern void ui_select_in(int id) {
}

extern void ui_select_param(int id) {
}

extern void ui_select_preset(int id) {
}

extern void ui_in_value(int id) {
}

extern void ui_param_value(int id) {
}

extern void ui_in_button(int id) {
}
