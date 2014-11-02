// bees
#include "net_protected.h"
#include "op.h"
// beekeep
#include "ui.h"
#include "ui_handlers.h"
#include "ui_lists.h"

//================================
//==== selection handlers

// select new operator type
 void ui_select_new_op(int id) {
  newOpSelect = (op_id_t)id;
  //  printf("\r\n selected operator type: %s", op_registry[newOpSelect].name);
  // update new-op label
  gtk_label_set_text(GTK_LABEL(newOpLabel), op_registry[newOpSelect].name);
}

// select existing operator
 void ui_select_op(int id) {
  opSelect = id;
  // ... select inputs/outputs? or wait for hotkey?
}


static void refresh_in_row_for_target(int t) {
  GtkListBoxRow* row;
  GtkListBox* list;
  int i;
  if(t < 0) { return; }
  // this is horrible:
  // net_num_ins() reports net->numIns + net->numParams.
  // really needs to be renamed!
  if(t < net_num_ins() ) {
    if (t < net->numIns) {
      i = t;
      // update inode selection
      list = GTK_LIST_BOX(boxIns.list);
      refresh_row_ins(i);
    } else {	
      i = t - net->numIns;
      // update param selection
      list = GTK_LIST_BOX(boxParams.list);
      refresh_row_params(i);
    } 
    row = gtk_list_box_get_row_at_index(list, i );
    gtk_list_box_select_row ( list, row ); 
  } else {
    ;; // bad target
  }

}

 void ui_select_out(int id) {
  // scroll to connected input, if exists
  int t;
  int tLast;
  if(outSelect >=0 ) { 
    tLast = net_get_target(outSelect);
  } else {
    tLast = -1;
  }
  //  printf("\r\n old target: %d", tLast);
  outSelect = id;
  t = net_get_target(id);
  refresh_in_row_for_target(t);

  if(tLast >=0) {
    refresh_in_row_for_target(tLast);
  }

  refresh_connect_input_but();
  refresh_connect_param_but();
}

 void ui_select_in(int id) {
  inSelect = id;
  refresh_connect_input_but();
  //... ?
}

 void ui_select_param(int id) {
  paramSelect = id; 
  refresh_connect_param_but();
  //... ?
}

 void ui_select_preset(int id) {
  presetSelect = id;
  printf("\r\n selecting preset (TODO!) ; id: %d", id);
}

//==================================
//==== editing handlers

void ui_connect_in(void) {
  // store the old target for refresh
  int t = net_get_target(outSelect);
  if(t == inSelect) {
    // was connected, disconnect
    net_disconnect(outSelect);
  } else {
    net_connect(outSelect, inSelect);
    refresh_row_ins(inSelect);
  }
  if(t < net->numIns) {
    refresh_row_ins(t);
  } else {
    refresh_row_params(t - net->numIns);
  }
  refresh_row_outs(outSelect);
}

void ui_connect_param(void) {
   // store the old target for refresh
  int t = net_get_target(outSelect);
  if(t == (paramSelect + net->numIns)) {
    // was connected here, disconnect
    net_disconnect(outSelect);
  } else {
    net_connect(outSelect, paramSelect + net->numIns);
    refresh_row_params(paramSelect);
  }
  if(t < net->numIns) {
    refresh_row_ins(t);
  } else {
    refresh_row_params(t - net->numIns);
  }
  refresh_row_outs(outSelect);
}

void ui_preset_in(void) {
  //...
}

void ui_preset_out(void) {
  //...
}

/* TODO
void ui_in_value() {
  //...
}

void ui_param_preset() {
  //...
}

void ui_param_value() {
  //...
}
*/

 void ui_create_op(void) {
  scroll_box_clear(&boxOps);
  scroll_box_clear(&boxOuts);
  scroll_box_clear(&boxIns);

  net_add_op(newOpSelect);

  fill_ops(GTK_LIST_BOX(boxOps.list));
  fill_outs(GTK_LIST_BOX(boxOuts.list));
  fill_ins(GTK_LIST_BOX(boxIns.list));
}

 void ui_delete_op(void) {
  scroll_box_clear(&boxOps);
  scroll_box_clear(&boxOuts);
  scroll_box_clear(&boxIns);

  net_pop_op();

  fill_ops(GTK_LIST_BOX(boxOps.list));
  fill_outs(GTK_LIST_BOX(boxOuts.list));
  fill_ins(GTK_LIST_BOX(boxIns.list));
}

// handle input value change
void ui_set_input(int id, int val) {
  //  net_set_active(0);
  net_set_in_value(id, val);
  //  net_set_active(1);
  refresh_row_ins(id);
}


// handle param value change
void ui_set_param(int id, int val) {

  printf("\r\n setting param %d to val 0x%08x ... "
	 //	     " need to implement scaling or this will crash.",  id, val);
	 , id, val);
  net_set_in_value(id + net->numIns, val);
  refresh_row_params(id);
}
