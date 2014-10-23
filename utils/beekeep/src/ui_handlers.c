// bees
#include "net_protected.h"
#include "op.h"
// beekeep
#include "ui.h"
#include "ui_handlers.h"
#include "ui_lists.h"

// select new opeator type
extern void ui_select_new_op(int id) {
  newOpSelect = (op_id_t)id;
  //  printf("\r\n selected operator type: %s", op_registry[newOpSelect].name);
  // update new-op label
  gtk_label_set_text(GTK_LABEL(newOpLabel), op_registry[newOpSelect].name);
}

// select existing operator
extern void ui_select_op(int id) {
  opSelect = id;
  // ... select inputs/outputs? or wait for hotkey?
}


extern void ui_select_out(int id) {
  // scroll to connected input, if exists
  GtkListBoxRow* row;
  GtkListBox* list;
  int t;
  int i;
  outSelect = id;
  t = net_get_target(id);
  if(t < 0) return;
  // this is horrible:
  // net_num_ins() reports net->numIns + net->numParams.
  // really needs to be renamed!
  if(t < net_num_ins() ) {
    if (t < net->numIns) {
      i = t;
      // update inode selection
      list = GTK_LIST_BOX(boxIns.list);
    } else {	
      i = t - net->numIns;
      // update param selection
      list = GTK_LIST_BOX(boxParams.list);
    } 
    row = gtk_list_box_get_row_at_index(list, i );
    gtk_list_box_select_row ( list, row ); 
  } else {
    ;; // bad target
  }
}

extern void ui_select_in(int id) {
  inSelect = id;
  //... ?
}

extern void ui_select_param(int id) {
  paramSelect = id; 
  //... ?
}

extern void ui_select_preset(int id) {
  presetSelect = id;
  //... ?
}

extern void ui_out_preset(int id) {
  //...
}

extern void ui_in_preset(int id) {
  //...
}

extern void ui_in_value(int id) {
  //...
}

extern void ui_param_preset(int id) {
  //...
}

extern void ui_param_value(int id) {
  //...
}

extern void ui_create_op(void) {
  scroll_box_clear(&boxOps);
  scroll_box_clear(&boxIns);
  scroll_box_clear(&boxOuts);

  net_add_op(newOpSelect);

  fill_ops(boxOps.list);
  fill_outs(boxOuts.list);
  fill_ins(boxIns.list);
}

extern void ui_delete_op(void) {
  scroll_box_clear(&boxOps);
  scroll_box_clear(&boxIns);
  scroll_box_clear(&boxOuts);

  net_pop_op();

  fill_ops(boxOps.list);
  fill_outs(boxOuts.list);
  fill_ins(boxIns.list);

}
