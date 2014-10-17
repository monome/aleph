#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

#include "net_protected.h"
#include "ui.h"

//--------------------
//--- selections
static void select_out( GObject *box, gpointer data ) {
  //  GtkListBoxRow* row = gtk_list_box_get_selected_row(GTK_LIST_BOX(box));
  g_print ("select output; ");
  //  g_print("row object: 0x%08x \n", (unsigned int)row);
  //... do more stuff
}

static void select_in( GObject *box, gpointer data ) {
  //  GtkListBoxRow* row = gtk_list_box_get_selected_row(GTK_LIST_BOX(box));
  g_print ("select input; ");
  //  g_print("row object: 0x%08x \n", (unsigned int)row);
  //... do more stuff
}

static void select_op( GObject *box, gpointer data ) {
  //  GtkListBoxRow* row = gtk_list_box_get_selected_row(GTK_LIST_BOX(box));
  g_print ("select operator; ");
  //  g_print("row object: 0x%08x \n", (unsigned int)row);
  //... do more stuff
}

//--------------------------------
//--- fill listboxes
static void fill_outs(GObject *box) {
  
  GtkWidget *row;
  GtkWidget *label;
  char str[64];
  int i, n;
  
  n = net->numOuts;
  for(i=0; i<n; i++) {
    snprintf(str, 64, "%s.%d.%s", 
	     net_op_name(net_out_op_idx(i)),
	     i, net_out_name(i) );
    row = gtk_list_box_row_new();
    label = gtk_label_new(str);
    gtk_container_add(GTK_CONTAINER(row), label);
    gtk_list_box_insert(GTK_LIST_BOX(box), row, 0);    
  }
}

static void fill_ins(GObject *box) {
  
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
    gtk_list_box_insert(GTK_LIST_BOX(box), row, 0);    
  }
}

static void fill_ops(GObject *box) {  
  GtkWidget *row;
  GtkWidget *label;
  char str[64];
  int i, n;
  
  n = net->numOps;
  for(i=0; i<n; i++) {
    snprintf(str, 64, "%s", net_op_name(i) );
    row = gtk_list_box_row_new();
    label = gtk_label_new(str);
    gtk_container_add(GTK_CONTAINER(row), label);
    gtk_list_box_insert(GTK_LIST_BOX(box), row, 0);    
  }
}

//------------------------
//---- init, build, connect
void ui_init(void) {
  GtkBuilder *builder;
  GObject *window;
  GObject *list;
  GObject *scroll;

  /* Construct a GtkBuilder instance and load our UI description */
  builder = gtk_builder_new ();
  gtk_builder_add_from_file (builder, "builder_glade.ui", NULL);

  /* Connect signal handlers to the constructed widgets. */
  window = gtk_builder_get_object (builder, "window");
  g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

  list = gtk_builder_get_object (builder, "outListBox");
  //  printf("\r\n list pointer: 0x%08x", (int)list);
  fill_outs(list);
  g_signal_connect (list, "row-selected", G_CALLBACK (select_out), NULL);
  scroll = gtk_builder_get_object (builder, "outScroll");
  gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll), 280);
  gtk_scrolled_window_set_min_content_width(GTK_SCROLLED_WINDOW(scroll), 280);

  /*
  list = gtk_builder_get_object (builder, "inListBox");
  //  printf("\r\n list pointer: 0x%08x", (int)list);
  fill_ins(list);
  g_signal_connect (list, "row-selected", G_CALLBACK (select_in), NULL);
  scroll = gtk_builder_get_object (builder, "inScroll");
  gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll), 280);
  gtk_scrolled_window_set_min_content_width(GTK_SCROLLED_WINDOW(scroll), 280);

  list = gtk_builder_get_object (builder, "opListBox");
  //  printf("\r\n list pointer: 0x%08x", (int)list);
  fill_ops(list);
  g_signal_connect (list, "row-selected", G_CALLBACK (select_op), NULL);
  scroll = gtk_builder_get_object (builder, "opScroll");
  gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll), 280);
  gtk_scrolled_window_set_min_content_width(GTK_SCROLLED_WINDOW(scroll), 280);
  */

}
