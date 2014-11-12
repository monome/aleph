// bees
#include "net_protected.h"
#include "op.h"
#include "preset.h"

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
  printf("\r\n selecting preset ; id: %d", id);
  preset_set_select(id);
  printf(" ; recalling...");
  preset_recall((u32)id);
  printf(" ; refreshing...");

  /// FIXME: this is pretty slow..
  /// better to refresh child widget states,
  /// than to rebuild the whole show.

  scroll_box_clear(&boxOuts);
  scroll_box_clear(&boxIns);
  scroll_box_clear(&boxParams);

  fill_outs(GTK_LIST_BOX(boxOuts.list));
  fill_ins(GTK_LIST_BOX(boxIns.list));
  fill_params(GTK_LIST_BOX(boxParams.list));
}

//==================================
//==== editing handlers

void ui_connect_in(void) {

  int t;
  if(outSelect < 0) { return; } 

  t = net_get_target(outSelect);
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
  int t;
  if(outSelect < 0) { return; } 
  t = net_get_target(outSelect);
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

/* void ui_preset_in(void) { */
/*   //... */
/* } */

/* void ui_preset_out(void) { */
/*   //... */
/* } */

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


// toggle preset inclusion for input
void ui_toggle_preset_input(int id) {
  printf("\r\n toggle preset input, id %d", id);
  net_toggle_in_preset(id);
  printf(" ; result: %d", preset_in_enabled(preset_get_select(), id));
}

// store input value in selected preset
void ui_store_preset_input(int id) {
  printf("\r\n store input value in preset, id %d", id);
  net_set_in_preset(id, 1);
  preset_store_in(preset_get_select(), id);
  if(id < net->numIns) {
    refresh_row_ins(id);
  } else {
    refresh_row_params(id - net->numIns);
  }
}

// toggle preset inclusion for output
void ui_toggle_preset_output(int id) {
  printf("\r\n toggle preset output, id %d", id);
  net_toggle_out_preset(id);
  printf(" ; result: %d", preset_out_enabled(preset_get_select(), id));
}

// store output target in selected preset
void ui_store_preset_output(int id) {
  printf("\r\n store output target in preset, id %d", id); 
  net_set_out_preset(id, 1);
  preset_store_out(preset_get_select(), id);
  refresh_row_outs(id);
}

// set module by name
void ui_set_module(const char* name) {
  printf("\r\n set module: %s", name);

  scroll_box_clear(&boxOuts);
  scroll_box_clear(&boxIns);
  scroll_box_clear(&boxParams);

  files_load_dsp_name(name);

  fill_outs(GTK_LIST_BOX(boxOuts.list));
  fill_ins(GTK_LIST_BOX(boxIns.list));
  fill_params(GTK_LIST_BOX(boxParams.list));
  
}
