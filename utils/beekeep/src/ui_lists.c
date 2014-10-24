// stdlib
#include <string.h>

// bees
#include "net_protected.h"
// beekeep
#include "ui_handlers.h"
#include "ui_lists.h"

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
  GtkWidget* entryValue;
} RowIns;

typedef struct _RowParams {
  GtkWidget* row;
  GtkWidget* grid;
  GtkWidget* labelConnected;
  GtkWidget* labelName;
  GtkWidget* entryValue;
  GtkWidget* labelValue;
} RowParams;


//------------------------
//---- variables

// kind unnecessary but whatever
RowOps rowOps[NET_OUTS_MAX];
RowOuts rowOuts[NET_OUTS_MAX];
RowIns rowIns[NET_INS_MAX];
RowParams rowParams[NET_INS_MAX];

//------------------
//--- callbacks

static void select_out_callback( GtkListBox *box, gpointer data ) {
  // gotta be a better way to get this
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

/* TODO
static void select_preset_callback( GtkListBox *box, gpointer data ) {
  int id;
  GtkListBoxRow* row;
  row = gtk_list_box_get_selected_row(box);
  id = gtk_list_box_row_get_index(row);
  ui_select_preset(id);
}
*/


//--------------------------------
//--- fill listboxes

void fill_ops(GtkListBox *list) {  
  GtkWidget *row;
  GtkWidget *label;
  char str[64];
  int i, n;
  
  g_signal_connect (list, "row-selected", G_CALLBACK(select_op_callback), NULL);

  n = net_num_ops();
 
  for(i=0; i<n; i++) {
    snprintf(str, 64, "%d.%s", i, net_op_name(i) );
    row = gtk_list_box_row_new();
    label = gtk_label_new(str);
    gtk_container_add(GTK_CONTAINER(row), label);
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

  char str[64];
  int i, n;
  int t;

  g_signal_connect (list, "row-selected", G_CALLBACK(select_out_callback), NULL);

  n = net->numOuts;

  for(i=0; i<n; i++) {
    snprintf(str, 64, "%s.%d.%s", 
	     net_op_name(net_out_op_idx(i)),
	     i, net_out_name(i) );

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
    gtk_label_set_max_width_chars(GTK_LABEL(label), 24);
    gtk_misc_set_alignment(GTK_MISC(label), 0.f, 0.f);
    gtk_grid_attach(GTK_GRID(grid), label, 1, 0, 1, 1);

    if(t > -1) {
      memset(str, '\0', 64);
      if(t >= net->numIns) { // target is param
	snprintf(str, 64, " -> %d.%s",
		 t - net->numIns, 
		 net_in_name(t) );
      } else {		// target is op input
	snprintf(str, 64, " -> %d.%s/%s", 
		 net_in_op_idx(t),
		 net_op_name(net_in_op_idx(t)),
		 net_in_name(t) );
      }
      gtk_label_set_text(GTK_LABEL(label), str);
    }
    gtk_container_add(GTK_CONTAINER(list), row);    
  }
  gtk_widget_show_all(list);
}

void fill_ins(GtkListBox *list) {
  GtkWidget *row;
  GtkWidget *label;
  GtkWidget *grid;
  char str[64];
  int i, n;

  g_signal_connect (list, "row-selected", G_CALLBACK(select_in_callback), NULL);
  n = net->numIns;

  for(i=0; i<n; i++) {
    snprintf(str, 64, "%s.%d.%s", 
	     net_op_name(net_in_op_idx(i)),
	     i, net_in_name(i) );
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
    gtk_grid_attach(GTK_GRID(grid), label, 1, 0, 1, 1);
    //    gtk_container_add(GTK_CONTAINER(row), label);
    gtk_container_add(GTK_CONTAINER(list), row);    
  }
  gtk_widget_show_all(list);
}

void fill_params(GtkListBox *list) {  
  GtkWidget *row;
  GtkWidget *label;
  char str[64];
  int i, n;

  g_signal_connect (list, "row-selected", G_CALLBACK(select_param_callback), NULL);
  n = net->numParams;

  for(i=0; i<n; i++) {
    snprintf(str, 64, "%d.%s", i, net_in_name(i + net->numIns) );
    row = gtk_list_box_row_new();
    label = gtk_label_new(str);
    rowParams[i].row = row;
    rowParams[i].labelName = label; 
    gtk_container_add(GTK_CONTAINER(row), label);
    gtk_container_add(GTK_CONTAINER(list), row);    
  }
  gtk_widget_show_all(list);
}


void fill_presets(GtkListBox *list) {  
  GtkWidget *row;
  GtkWidget *label;
  char str[64];
  int i, n;

  g_signal_connect (list, "row-selected", G_CALLBACK(select_param_callback), NULL);
  n = net->numParams;

  for(i=0; i<n; i++) {
    snprintf(str, 64, "%d.%s", i, net_in_name(i + net->numIns) );
    row = gtk_list_box_row_new();
    label = gtk_label_new(str);
    rowParams[i].row = row;
    rowParams[i].labelName = label; 
    gtk_container_add(GTK_CONTAINER(row), label);
    gtk_container_add(GTK_CONTAINER(list), row);    
  }
  gtk_widget_show_all(list);
}




//---- refresh/rebuild individual rows
