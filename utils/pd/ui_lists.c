// stdlib
#include <string.h>

// bees
#include "net_protected.h"
#include "preset.h"
// beekeep
#include "ui_handlers.h"
#include "ui_lists.h"

// size of label text buffers
#define LABEL_BUF_SIZE 64

//-------------------------
//--- classes

// stuff needed by rows
typedef struct _RowOps {
  GtkWidget* row;
  GtkWidget* labelName;
} RowOps;

typedef struct _RowOuts {
  GtkWidget* row;
  GtkWidget* grid;
  GtkWidget* labelName;
  GtkWidget* labelTarget;
  GtkWidget* togglePreset;
  GtkWidget* storePreset;
} RowOuts;

typedef struct _RowIns {
  GtkWidget* row;
  GtkWidget* grid;
  GtkWidget* labelConnected;
  GtkWidget* labelName;
  GtkWidget* spinValue;
  GtkWidget* togglePreset;
  GtkWidget* storePreset;
} RowIns;

  GtkWidget* storePreset;
typedef struct _RowParams {
  GtkWidget* row;
  GtkWidget* grid;
  GtkWidget* labelConnected;
  GtkWidget* labelName;
  GtkWidget* spinValue;
  GtkWidget* labelValue;
  GtkWidget* togglePreset;
  GtkWidget* storePreset;
} RowParams;


typedef struct _RowPresets {
  GtkWidget* row;
  GtkWidget* grid;
  GtkWidget* entryName;
} RowPresets;



//------------------------
//---- variables

// kinda unnecessary but whatever
RowOps 	rowOps[NET_OUTS_MAX];
RowOuts rowOuts[NET_OUTS_MAX];
RowIns rowIns[NET_INS_MAX];
RowParams rowParams[NET_INS_MAX];
RowPresets rowPresets[NET_PRESETS_MAX];

//------------------
//--- callbacks

static void select_out_callback( GtkListBox *box, gpointer data ) {
  int id;
  GtkListBoxRow* row;
  row = gtk_list_box_get_selected_row(box);
  id = gtk_list_box_row_get_index(row);
  if(id < 0) { 
    printf("\r\n invalid output selection id");
    return;
  }
  ui_select_out(id);
}


static void select_in_callback( GtkListBox *box, gpointer data ) {
  int id;
  GtkListBoxRow* row;
  row = gtk_list_box_get_selected_row(box);
  id = gtk_list_box_row_get_index(row);
  if(id < 0) { 
    printf("\r\n invalid input selection id");
    return;
  }
  ui_select_in(id);
}

static void select_op_callback( GtkListBox *box, gpointer data ) {
  int id;
  GtkListBoxRow* row;
  row = gtk_list_box_get_selected_row(box);
  id = gtk_list_box_row_get_index(row);
  if(id < 0) { 
    printf("\r\n invalid op selection id");
    return;
  }
  ui_select_op(id);
}

static void select_param_callback( GtkListBox *box, gpointer data ) {
  int id;
  GtkListBoxRow* row;
  row = gtk_list_box_get_selected_row(box);
  id = gtk_list_box_row_get_index(row);
  if(id < 0) { 
    printf("\r\n invalid param selection id");
    return;
  }
  ui_select_param(id);
}

static void select_preset_callback( GtkListBox *box, gpointer data ) {
  int id;
  GtkListBoxRow* row;
  row = gtk_list_box_get_selected_row(box);
  id = gtk_list_box_row_get_index(row);
  if(id < 0) { 
    printf("\r\n invalid preset selection id");
    return;
  }
  ui_select_preset(id);
}

static void spin_in_callback( GtkSpinButton *but, gpointer data ) {
  int id = GPOINTER_TO_INT(data);
  int val = gtk_spin_button_get_value_as_int(but);
  printf("\r\n setting input node from spinbox; id: %d; val: 0x%08x", id, val);
  ui_set_input(id, val);
}


static void spin_param_callback( GtkSpinButton *but, gpointer data ) {
  int id = GPOINTER_TO_INT(data);
  int val = gtk_spin_button_get_value_as_int(but);
  printf("\r\n setting param from spinbox; id: %d; val: 0x%08x", id, val);
  ui_set_param(id, val);
}


static void preset_name_entry_callback( GtkEntry *entry, gpointer data) {
  int id = GPOINTER_TO_INT(data);
  const char* str;
  str = gtk_entry_get_text(entry);
  printf("\r\n setting preset name from widget; id: %d ; new name: %s", id, str);
  strncpy(preset_name(id), str, PRESET_NAME_LEN);
}

static void preset_toggle_input_callback( GtkWidget *tog, gpointer data) {
  int id = GPOINTER_TO_INT(data);
  ui_toggle_preset_input(id);
}

static void preset_store_input_callback( GtkWidget *but, gpointer data) {
  int id = GPOINTER_TO_INT(data);
  ui_store_preset_input(id);
}

static void preset_toggle_output_callback( GtkWidget *tog, gpointer data) {
  int id = GPOINTER_TO_INT(data);
  ui_toggle_preset_output(id);
}

static void preset_store_output_callback( GtkWidget *but, gpointer data) {
  int id = GPOINTER_TO_INT(data);
  ui_store_preset_output(id);
}


//----------------------------------
//--- helpers

GtkWidget* create_spin_button(int val) {
  GtkWidget *button;
  GtkAdjustment *adjustment;
  adjustment = gtk_adjustment_new ((double)val, -32768, 32767, 1.0, 5.0, 0.0);
  button = gtk_spin_button_new (adjustment, 1.0, 0);
  return button;
}

// set state of toggle with callback as argument (.... :S )
typedef void (*toggle_callback)( GtkWidget *tog, gpointer data);
static void set_toggle_state(GtkToggleButton* tog, 
			     gboolean state, 
			     toggle_callback fn) 
{
  g_signal_handlers_block_by_func( tog, G_CALLBACK(fn), NULL);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tog), state);
  g_signal_handlers_unblock_by_func( tog, G_CALLBACK(fn), NULL);
}

//--------------------------------
//--- fill listboxes

void fill_ops(GtkListBox *list) {  
  GtkWidget *row;
  GtkWidget *label;
  GtkWidget *grid;
  char str[LABEL_BUF_SIZE];
  int i, n;
  
  g_signal_connect (list, "row-selected", G_CALLBACK(select_op_callback), NULL);

  n = net_num_ops();
 
  for(i=0; i<n; i++) {
    snprintf(str, LABEL_BUF_SIZE, "%d.%s", i, net_op_name(i) );
    row = gtk_list_box_row_new();
    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(row), grid);
    label = gtk_label_new(str);
    gtk_grid_attach_next_to(GTK_GRID(grid), label, NULL, GTK_POS_RIGHT, 1, 1);
    gtk_container_add(GTK_CONTAINER(list), row);    
    rowOps[i].row = row;
    rowOps[i].labelName = label;
    gtk_widget_show_all(row);
  }
}

void fill_outs(GtkListBox *list) {
  GtkWidget *row;
  GtkWidget *label;
  GtkWidget* grid;
  GtkWidget* tog;
  GtkWidget* but;
  char str[LABEL_BUF_SIZE];
  int i, n;
  int t;

  g_signal_connect (list, "row-selected", G_CALLBACK(select_out_callback), NULL);

  n = net->numOuts;

  for(i=0; i<n; i++) {
    snprintf(str, LABEL_BUF_SIZE, "%d.%s.%s", 
	     net_out_op_idx(i), 
	     net_op_name(net_out_op_idx(i)),
	     net_out_name(i) );

    row = gtk_list_box_row_new();
    rowOuts[i].row = row;
    
    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(row), grid);
    rowOuts[i].grid = grid;

    label = gtk_label_new(str);
    rowOuts[i].labelName = label;
    gtk_label_set_width_chars(GTK_LABEL(label), 16);
    gtk_misc_set_alignment(GTK_MISC(label), 0.f, 0.f);
    gtk_grid_attach_next_to(GTK_GRID(grid), label, NULL, GTK_POS_RIGHT, 1, 1);

    ///  preset_inclusion toggle
    tog = gtk_toggle_button_new_with_label("I");
    g_signal_connect(tog, "clicked", 
		     G_CALLBACK(preset_toggle_output_callback), 
		     // use the same callback for op ins and params.
		     // so data passed to callback should be flat inode index.
		     GINT_TO_POINTER(i + net->numIns));
    gtk_grid_attach_next_to( GTK_GRID(grid), tog, NULL, GTK_POS_RIGHT, 1, 1 );
    rowOuts[i].togglePreset = tog;

    // preset include state
    set_toggle_state( GTK_TOGGLE_BUTTON(rowOuts[i].togglePreset) ,
		     preset_out_enabled(preset_get_select(), i),
		     &preset_toggle_output_callback);

    // store-preset button
    but = gtk_button_new_with_label("S");
    g_signal_connect(but, "clicked", 
		     G_CALLBACK(preset_store_output_callback), 
		     // use the same callback for op ins and params.
		     // so data passed to callback should be flat inode index.
		     GINT_TO_POINTER(i + net->numIns));
    gtk_grid_attach_next_to( GTK_GRID(grid), but, NULL, GTK_POS_RIGHT, 1, 1 );
    rowOuts[i].storePreset = but;

    // target label
    
    t = net_get_target(i);
    label = gtk_label_new("");
    rowOuts[i].labelTarget = label;
    gtk_label_set_max_width_chars(GTK_LABEL(label), 16);
    gtk_misc_set_alignment(GTK_MISC(label), 0.f, 0.f);
    gtk_grid_attach_next_to(GTK_GRID(grid), label, NULL, GTK_POS_RIGHT, 1, 1);

    if(t > -1) {
      memset(str, '\0', LABEL_BUF_SIZE);
      if(t >= net->numIns) { // target is param
	snprintf(str, LABEL_BUF_SIZE, " -> %d.%s",
		 t - net->numIns, 
		 net_in_name(t) );
      } else {		// target is op input
	snprintf(str, LABEL_BUF_SIZE, " -> %d.%s/%s", 
		 net_in_op_idx(t),
		 net_op_name(net_in_op_idx(t)),
		 net_in_name(t) );
      }
      gtk_label_set_text(GTK_LABEL(label), str);
    }

    gtk_container_add(GTK_CONTAINER(list), row);    
  }
  gtk_widget_show_all(GTK_WIDGET(list));
}

void fill_ins(GtkListBox *list) {
  GtkWidget *row;
  GtkWidget *label;
  GtkWidget *grid;
  GtkWidget *spin;
  GtkWidget *tog;
  GtkWidget *but;
  char str[LABEL_BUF_SIZE];
  int i, n;

  g_signal_connect (list, "row-selected", G_CALLBACK(select_in_callback), NULL);
  n = net->numIns;

  for(i=0; i<n; i++) {
    snprintf(str, LABEL_BUF_SIZE, "%d.%s.%s", 
	     net_in_op_idx(i),
	     net_op_name(net_in_op_idx(i)),
	     net_in_name(i) );

    row = gtk_list_box_row_new();
    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(row), grid);
    rowIns[i].row = row;
    rowIns[i].grid = grid;

    // label for connection arrow
    label = gtk_label_new("");
    rowIns[i].labelConnected = label;
    gtk_label_set_width_chars(GTK_LABEL(label), 4);
    gtk_misc_set_alignment(GTK_MISC(label), 0.f, 0.f);
    gtk_grid_attach_next_to(GTK_GRID(grid), label, NULL, GTK_POS_RIGHT, 1, 1);

    if(outSelect != -1) {
      if(net_get_target(outSelect) == i) {
	gtk_label_set_text(GTK_LABEL(label), " -> ");
      }
    }

    // label for input string
    label = gtk_label_new(str);
    rowIns[i].labelName = label;
    gtk_misc_set_alignment(GTK_MISC(label), 0.f, 0.f);
    gtk_label_set_width_chars(GTK_LABEL(label), 18);
    gtk_grid_attach_next_to(GTK_GRID(grid), label, NULL, GTK_POS_RIGHT, 1, 1);

    // spinbutton for value entry
    spin = create_spin_button(net_get_in_value(i));
    rowIns[i].spinValue = spin;
    gtk_grid_attach_next_to(GTK_GRID(grid), spin, NULL, GTK_POS_RIGHT, 1, 1);
    g_signal_connect (spin, "value_changed", G_CALLBACK(spin_in_callback), GINT_TO_POINTER(i));


    ///  preset_inclusion toggle
    tog = gtk_toggle_button_new_with_label("I");
    g_signal_connect(tog, "clicked", 
		     G_CALLBACK(preset_toggle_input_callback), 
		     GINT_TO_POINTER(i));
    gtk_grid_attach_next_to( GTK_GRID(grid), tog, NULL, GTK_POS_RIGHT, 1, 1 );
    rowIns[i].togglePreset = tog;
    // toggle state
    set_toggle_state(GTK_TOGGLE_BUTTON(rowIns[i].togglePreset),
		     preset_in_enabled(preset_get_select(), i),
		     &preset_toggle_input_callback);


    // store-preset button
    but = gtk_button_new_with_label("S");
    g_signal_connect(but, "clicked", 
		     G_CALLBACK(preset_store_input_callback), 
		     GINT_TO_POINTER(i));
    gtk_grid_attach_next_to( GTK_GRID(grid), but, NULL, GTK_POS_RIGHT, 1, 1 );
    rowIns[i].storePreset = but;
    
    gtk_container_add(GTK_CONTAINER(list), row);    
  }
  gtk_widget_show_all(GTK_WIDGET(list));
}

void fill_params(GtkListBox *list) {  
  GtkWidget *row;
  GtkWidget *grid;
  GtkWidget *label;
  GtkWidget *spin;
  GtkWidget* tog;
  GtkWidget* but;
  char str[LABEL_BUF_SIZE];
  int i, n;

  g_signal_connect (list, "row-selected", G_CALLBACK(select_param_callback), NULL);
  n = net->numParams;

  for(i=0; i<n; i++) {
    // build name string
    snprintf(str, LABEL_BUF_SIZE, "%d.%s", i, net_in_name(i + net->numIns) );

    // row and grid containers
    row = gtk_list_box_row_new();
    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(row), grid);
    rowParams[i].row = row;
    rowParams[i].grid = grid;

    // label for connection arrow
    label = gtk_label_new("");
    rowParams[i].labelConnected = label;
    gtk_label_set_width_chars(GTK_LABEL(label), 4);
    gtk_misc_set_alignment(GTK_MISC(label), 0.f, 0.f);
    gtk_grid_attach_next_to(GTK_GRID(grid), label, NULL, GTK_POS_RIGHT, 1, 1);
    
    if(outSelect != -1) {
      if(net_get_target(outSelect) == (i + net->numIns)) {
	gtk_label_set_text(GTK_LABEL(label), " -> ");
      }
    }

    // label for name
    label = gtk_label_new(str);
    rowParams[i].labelName = label; 
    gtk_misc_set_alignment(GTK_MISC(label), 0.f, 0.f);
    gtk_label_set_width_chars(GTK_LABEL(label), 18);
    gtk_grid_attach_next_to(GTK_GRID(grid), label, NULL, GTK_POS_RIGHT, 1, 1);

    // spinbutton for value entry
    spin = create_spin_button(net_get_in_value(i + net->numIns));
    rowParams[i].spinValue = spin;
    gtk_grid_attach_next_to(GTK_GRID(grid), spin, NULL, GTK_POS_RIGHT, 1, 1);
    g_signal_connect (spin, "value_changed", G_CALLBACK(spin_param_callback), GINT_TO_POINTER(i));
    
    // label for name
    //    param_get_string(str, i + net->numIns);
    //// FIXME: again, this is terribly confusing. 
    //// some functions take the param index and some the input node index.
    /// this one is the latter.
    memset(str, '\0', LABEL_BUF_SIZE);
    net_get_param_value_string(str, i + net->numIns);
    //    printf("\r\n param value label: %s", str);
    label = gtk_label_new(str);
    rowParams[i].labelValue = label; 
    gtk_misc_set_alignment(GTK_MISC(label), 0.f, 0.f);
    gtk_label_set_width_chars(GTK_LABEL(label), 12);
    gtk_grid_attach_next_to(GTK_GRID(grid), label, NULL, GTK_POS_RIGHT, 1, 1);
    
    ///  preset_inclusion toggle
    tog = gtk_toggle_button_new_with_label("I");
    g_signal_connect(tog, "clicked", 
		     G_CALLBACK(preset_toggle_input_callback), 
		     // use the same callback for op ins and params.
		     // so data passed to callback should be flat inode index.
		     GINT_TO_POINTER(i + net->numIns));
    gtk_grid_attach_next_to( GTK_GRID(grid), tog, NULL, GTK_POS_RIGHT, 1, 1 );
    rowParams[i].togglePreset = tog;

    // toggle state
    set_toggle_state(GTK_TOGGLE_BUTTON(rowParams[i].togglePreset),
		     preset_in_enabled(preset_get_select(), i + net->numIns),
		     &preset_toggle_input_callback);

    // store-preset button
    but = gtk_button_new_with_label("S");
    g_signal_connect(but, "clicked", 
		     G_CALLBACK(preset_store_input_callback), 
		     // use the same callback for op ins and params.
		     // so data passed to callback should be flat inode index.
		     GINT_TO_POINTER(i + net->numIns));
    gtk_grid_attach_next_to( GTK_GRID(grid), but, NULL, GTK_POS_RIGHT, 1, 1 );
    rowParams[i].storePreset = but;

    gtk_container_add(GTK_CONTAINER(list), row);    
  }
  gtk_widget_show_all(GTK_WIDGET(list));
}


void fill_presets(GtkListBox *list) {  
  GtkWidget *row;
  GtkWidget *entry;
  GtkWidget* grid;

  char* str;
  int i, n;

  g_signal_connect (list, "row-selected", G_CALLBACK(select_preset_callback), NULL);

  n = NET_PRESETS_MAX;

  for(i=0; i<n; i++) {
    row = gtk_list_box_row_new();
    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(row), grid);
    rowPresets[i].grid = grid;

    str = preset_name(i);

    entry = gtk_entry_new();
    gtk_entry_set_text( GTK_ENTRY(entry), str );
    gtk_grid_attach_next_to(GTK_GRID(grid), entry, NULL, GTK_POS_RIGHT, 4, 1);
    g_signal_connect( entry, "activate", 
		      G_CALLBACK(preset_name_entry_callback), GINT_TO_POINTER(i));

    rowPresets[i].row = row;
    rowPresets[i].grid = grid;
    rowPresets[i].entryName = entry; 
    gtk_container_add(GTK_CONTAINER(list), row);    
  }
  gtk_widget_show_all(GTK_WIDGET(list));
}

//---- refresh/rebuild individual rows

// nothing to do?
/* void refresh_row_ops(int id) { */
/* } */

void refresh_row_outs(int id) {
  GtkWidget *row;
  GtkWidget *label;
  char str[LABEL_BUF_SIZE];
  int t;

  if(id < 0 || id >= net->numOuts) { return; }

  row = rowOuts[id].row;
  label = rowOuts[id].labelTarget;
  
  // refresh the target label
  memset(str, '\0', LABEL_BUF_SIZE);
  t = net_get_target(id);
  if(t > -1) {
    if(t >= net->numIns) { // target is param
      snprintf(str, LABEL_BUF_SIZE, " -> %d.%s",
	       t - net->numIns, 
	       net_in_name(t) );
    } else {		// target is op input
      snprintf(str, LABEL_BUF_SIZE, " -> %d.%s/%s", 
	       net_in_op_idx(t),
	       net_op_name(net_in_op_idx(t)),
	       net_in_name(t) );
    }
  }
  gtk_label_set_text(GTK_LABEL(label), str);

  // refresh preset include toggle
  set_toggle_state(GTK_TOGGLE_BUTTON(rowOuts[id].togglePreset),
		   preset_out_enabled(preset_get_select(), id),
		   &preset_toggle_output_callback);

  gtk_widget_show_all(row);
}

void refresh_row_ins(int id) {
  GtkWidget *row;
  GtkWidget *label;
  GtkWidget *spin;
  int t;
  int val;

  if(id < 0 || id >= net->numIns) { return; }
  
  row = rowIns[id].row;
  label = rowIns[id].labelConnected;
  spin = rowIns[id].spinValue;

  // refresh the connection label  
  if(outSelect >= 0) {
    t = net_get_target(outSelect);
    if(t == id) {
      gtk_label_set_text(GTK_LABEL(label), " -> ");
    } else { 
      gtk_label_set_text(GTK_LABEL(label), "    ");
    }
  }

  // set value...
  val = net_get_in_value(id);
  /// this doesn't trigger a signal, strangely...
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin), (gdouble)val);

  // refresh preset include toggle
  set_toggle_state(GTK_TOGGLE_BUTTON(rowIns[id].togglePreset),
		   preset_in_enabled(preset_get_select(), id),
		   &preset_toggle_input_callback);


  gtk_widget_show_all(row);
}

void refresh_row_params(int id) {
  GtkWidget *row;
  GtkWidget *label;
  GtkWidget *spin;
  char str[LABEL_BUF_SIZE];
  int t;
  int val;
  if(id < 0 || id >= net->numParams) { return; }
  
  row = rowParams[id].row;
  label = rowParams[id].labelConnected;
  spin = rowParams[id].spinValue;

  // refresh the connection label  
  if(outSelect >= 0) {
    t = net_get_target(outSelect) - net->numIns;
    if(t == id) {
      gtk_label_set_text(GTK_LABEL(label), " -> ");
    } else { 
      gtk_label_set_text(GTK_LABEL(label), "    ");
    }
  }

  // refresh value label
  label = rowParams[id].labelValue;
  net_get_param_value_string(str, id + net->numIns);
  gtk_label_set_text(GTK_LABEL(label), str);

  gtk_widget_show_all(row);

  // refresh preset include toggle
  set_toggle_state(GTK_TOGGLE_BUTTON(rowParams[id].togglePreset),
		   preset_in_enabled(preset_get_select(), id + net->numIns),
		   &preset_toggle_input_callback);

  //...??
  //  val = net_get_in_value(id);
  //  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin), (gdouble)val);
  //  gtk_widget_show_all(row);
}
