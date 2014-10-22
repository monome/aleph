#include <string.h>

#include "net_protected.h"
#include "ui_lists.h"

//--------------------------------
//--- fill listboxes

void fill_ops(GtkListBox *box) {  
  GtkWidget *row;
  GtkWidget *label;
  char str[64];
  int i, n;
  
  n = net_num_ops();

  for(i=0; i<n; i++) {
    snprintf(str, 64, "%d.%s", i, net_op_name(i) );
    row = gtk_list_box_row_new();
    label = gtk_label_new(str);
    gtk_container_add(GTK_CONTAINER(row), label);
    gtk_list_box_insert(box, row, -1);    
  }
}

void fill_outs(GtkListBox *box) {
  GtkWidget *row;
  GtkWidget *label;
  GtkWidget *grid;
  char str[64];
  int i, n;
  int t;

  n = net->numOuts;

  for(i=0; i<n; i++) {
    snprintf(str, 64, "%s.%d.%s", 
	     net_op_name(net_out_op_idx(i)),
	     i, net_out_name(i) );

    row = gtk_list_box_row_new();
    
    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(row), grid);

    label = gtk_label_new(str);
    gtk_label_set_width_chars(GTK_LABEL(label), 16);
    gtk_misc_set_alignment(GTK_MISC(label), 0.f, 0.f);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

    t = net_get_target(i);
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
      label = gtk_label_new(str);
      gtk_label_set_max_width_chars(GTK_LABEL(label), 24);
      gtk_misc_set_alignment(GTK_MISC(label), 0.f, 0.f);
      gtk_grid_attach(GTK_GRID(grid), label, 1, 0, 1, 1);
    }

    gtk_list_box_insert(box, row, -1);    
  }
}

void fill_ins(GtkListBox *box) {
  GtkWidget *row;
  GtkWidget *label;
  char str[64];
  int i, n;

  n = net->numIns;

  for(i=0; i<n; i++) {
    snprintf(str, 64, "%s.%d.%s", 
	     net_op_name(net_in_op_idx(i)),
	     i, net_in_name(i) );
    row = gtk_list_box_row_new();
    label = gtk_label_new(str);

    gtk_misc_set_alignment(GTK_MISC(label), 0.f, 0.f);
    gtk_container_add(GTK_CONTAINER(row), label);
    gtk_list_box_insert(box, row, -1);    
  }
}

void fill_params(GtkListBox *box) {  
  GtkWidget *row;
  GtkWidget *label;
  char str[64];
  int i, n;

  n = net->numParams;

  for(i=0; i<n; i++) {
    snprintf(str, 64, "%d.%s", i, net_in_name(i + net->numIns) );
    row = gtk_list_box_row_new();
    label = gtk_label_new(str);
    gtk_container_add(GTK_CONTAINER(row), label);
    gtk_list_box_insert(box, row, -1);    
  }
}
