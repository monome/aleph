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
  char str[64];
  int i, n;
  int t;

  n = net->numOuts;

  for(i=0; i<n; i++) {
    snprintf(str, 64, "%s.%d.%s", 
	     net_op_name(net_out_op_idx(i)),
	     i, net_out_name(i) );
    t = net_get_target(i);
    if(t > -1) {
      strcat(str, " -> ");
      strcat(str, net_in_name(t));
    }
    row = gtk_list_box_row_new();
    label = gtk_label_new(str);
    gtk_container_add(GTK_CONTAINER(row), label);
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
