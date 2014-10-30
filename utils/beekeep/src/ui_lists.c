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
} RowOuts;

typedef struct _RowIns {
  GtkWidget* row;
  GtkWidget* grid;
  GtkWidget* labelConnected;
  GtkWidget* labelName;
  GtkWidget* spinValue;
  GtkWidget* togglePreset;
} RowIns;

typedef struct _RowParams {
  GtkWidget* row;
  GtkWidget* grid;
  GtkWidget* labelConnected;
  GtkWidget* labelName;
  GtkWidget* spinValue;
  GtkWidget* labelValue;
  GtkWidget* togglePreset;
} RowParams;


//------------------------
//---- variables

// kinda unnecessary but whatever
RowOps 	rowOps[NET_OUTS_MAX];
RowOuts rowOuts[NET_OUTS_MAX];
RowIns rowIns[NET_INS_MAX];
RowParams rowParams[NET_INS_MAX];

//------------------
//--- callbacks

static void select_out_callback( GtkListBox *box, gpointer data ) {
  int id;
  GtkListBoxRow* row;
  row = gtk_list_box_get_selected_row(box);
  id = gtk_list_box_row_get_index(row);
  ui_select_out(id);
}


static void select_in_callback( GtkListBox *box, gpointer data ) {
  int id;
  GtkListBoxRow* row;
  row = gtk_list_box_get_selected_row(box);
  id = gtk_list_box_row_get_index(row);
  ui_select_in(id);
}

static void select_op_callback( GtkListBox *box, gpointer data ) {
  int id;
  GtkListBoxRow* row;
  row = gtk_list_box_get_selected_row(box);
  id = gtk_list_box_row_get_index(row);
  ui_select_op(id);
}

static void select_param_callback( GtkListBox *box, gpointer data ) {
  int id;
  GtkListBoxRow* row;
  row = gtk_list_box_get_selected_row(box);
  id = gtk_list_box_row_get_index(row);
  ui_select_param(id);
}

static void select_preset_callback( GtkListBox *box, gpointer data ) {
  int id;
  GtkListBoxRow* row;
  row = gtk_list_box_get_selected_row(box);
  id = gtk_list_box_row_get_index(row);
  ui_select_preset(id);
}

static void spin_in_callback( GtkSpinButton *but, gpointer data ) {
  //  int id = *((int*)data);
  int id = GPOINTER_TO_INT(data);
  int val = gtk_spin_button_get_value_as_int(but);
  printf("\r\n setting input node from spinbox; id: %d; val: 0x%08x", id, val);
  ui_set_input(id, val);
}


static void spin_param_callback( GtkSpinButton *but, gpointer data ) {
  //  int id = *((int*)data);
  int id = GPOINTER_TO_INT(data);
  int val = gtk_spin_button_get_value_as_int(but);
  printf("\r\n setting param from spinbox; id: %d; val: 0x%08x", id, val);
  ui_set_param(id, val);
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
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);
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
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);
    
    t = net_get_target(i);
    label = gtk_label_new("");
    rowOuts[i].labelTarget = label;
    gtk_label_set_max_width_chars(GTK_LABEL(label), 16);
    gtk_misc_set_alignment(GTK_MISC(label), 0.f, 0.f);
    gtk_grid_attach(GTK_GRID(grid), label, 1, 0, 1, 1);

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
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

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
    gtk_grid_attach(GTK_GRID(grid), label, 1, 0, 1, 1);

    // spinbutton for value entry
    spin = create_spin_button(net_get_in_value(i));
    rowIns[i].spinValue = spin;
    gtk_grid_attach(GTK_GRID(grid), spin, 4, 0, 1, 1);
    g_signal_connect (spin, "value_changed", G_CALLBACK(spin_in_callback), GINT_TO_POINTER(i));
    
    gtk_container_add(GTK_CONTAINER(list), row);    
  }
  gtk_widget_show_all(GTK_WIDGET(list));
}

void fill_params(GtkListBox *list) {  
  GtkWidget *row;
  GtkWidget *grid;
  GtkWidget *label;
  GtkWidget *spin;
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
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);
    
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
    gtk_grid_attach(GTK_GRID(grid), label, 1, 0, 1, 1);

    // spinbutton for value entry
    spin = create_spin_button(net_get_in_value(i));
    rowParams[i].spinValue = spin;
    gtk_grid_attach(GTK_GRID(grid), spin, 4, 0, 1, 1);
    g_signal_connect (spin, "value_changed", G_CALLBACK(spin_param_callback), GINT_TO_POINTER(i));
    
    /// TODO: param representation label

    /// TODO: preset inclusion toggle

    
    gtk_container_add(GTK_CONTAINER(list), row);    
  }
  gtk_widget_show_all(GTK_WIDGET(list));
}


void fill_presets(GtkListBox *list) {  
  GtkWidget *row;
  GtkWidget *label;
  char str[LABEL_BUF_SIZE];
  int i, n;

  g_signal_connect (list, "row-selected", G_CALLBACK(select_param_callback), NULL);
  n = NET_PRESETS_MAX;

  for(i=0; i<n; i++) {
    //    snprintf(str, LABEL_BUF_SIZE, "%d.%s", i, net_in_name(i + net->numIns) );
    snprintf(str, LABEL_BUF_SIZE, "%s", preset_name(i));
    row = gtk_list_box_row_new();
    label = gtk_label_new(str);
    rowParams[i].row = row;
    rowParams[i].labelName = label; 
    gtk_container_add(GTK_CONTAINER(row), label);
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
  //#endif
  ///////////////
  /// block /// don't even need it? amazing (and rather inconsistent)
  //  g_signal_handlers_block_by_func(spin, G_CALLBACK(spin_in_callback), NULL);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin), (gdouble)val);
  // unblock
  ///  g_signal_handlers_unblock_by_func(spin, G_CALLBACK(spin_in_callback), NULL);
  //////////////
  gtk_widget_show_all(row);
}

void refresh_row_params(int id) {
  GtkWidget *row;
  GtkWidget *label;
  GtkWidget *spin;
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

  //// TODO: param rep label
  // val = net_get_in_value(id);
  //  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin), (gdouble)val);
  //  gtk_widget_show_all(row);
}
