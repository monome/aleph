#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

#include "net_protected.h"
#include "ui.h"

//--------------------
//--- selections
static void select_out( GtkListBox *box, gpointer data ) {
  GtkListBoxRow* row = gtk_list_box_get_selected_row(box);
  g_print ("select output; ");
  //  g_print("row object: 0x%08x \n", (unsigned int)row);
  //... do more stuff
}

static void select_in( GtkListBox *box, gpointer data ) {
  GtkListBoxRow* row = gtk_list_box_get_selected_row(box);
  g_print ("select input; ");
  //  g_print("row object: 0x%08x \n", (unsigned int)row);
  //... do more stuff
}

static void select_op( GtkListBox *box, gpointer data ) {
  GtkListBoxRow* row = gtk_list_box_get_selected_row(box);
  g_print ("select operator; ");
  //  g_print("row object: 0x%08x \n", (unsigned int)row);
  //... do more stuff
}

//--------------------------------
//--- fill listboxes
static void fill_outs(GtkListBox *box) {
  
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
    gtk_list_box_insert(box, row, 0);    
  }
}

static void fill_ins(GtkListBox *box) {
  
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
    gtk_list_box_insert(box, row, 0);    
  }
}

static void fill_ops(GtkListBox *box) {  
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
    gtk_list_box_insert(box, row, 0);    
  }
}

//------------------------
//---- init, build, connect
void ui_init(void) {
  //  GtkBuilder *builder;
  //  GObject *window;
  //  GObject *list;

  GtkWidget *window;
  GtkWidget *scrolled;
  GtkWidget *layout;

  GtkWidget *outList;
  GtkWidget *inList;
  GtkWidget *opList;


  //---  window
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "bees editor");
  gtk_window_set_default_size(GTK_WINDOW(window), 300, 400);
  //  gtk_container_set_border_width (GTK_CONTAINER (window), 0);
  //  g_signal_connect (window, "delete-event", G_CALLBACK (delete_handler), NULL);
  g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

  //--- layout 
  layout = gtk_layout_new(NULL, NULL);
  gtk_container_add(GTK_CONTAINER(window), layout);

  // create scrolling window
  scrolled = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled),
				  GTK_POLICY_AUTOMATIC, 
				  GTK_POLICY_AUTOMATIC);

  gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scrolled), 280);
  gtk_scrolled_window_set_min_content_width(GTK_SCROLLED_WINDOW(scrolled), 280);
  gtk_container_add(GTK_CONTAINER(layout), scrolled);


  // create lists
  outList = gtk_list_box_new ();
  g_signal_connect (outList, "row-selected", G_CALLBACK(select_out), NULL);
  fill_outs(GTK_LIST_BOX(outList));
  gtk_container_add( GTK_CONTAINER(scrolled), outList );
  gtk_widget_show (outList);


  /// show everything
  gtk_widget_show_all(window);

  /* /\* Construct a GtkBuilder instance and load our UI description *\/ */
  /* builder = gtk_builder_new (); */
  /* gtk_builder_add_from_file (builder, "builder.ui", NULL); */

  /* /\* Connect signal handlers to the constructed widgets. *\/ */
  /* window = gtk_builder_get_object (builder, "window"); */
  /* g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL); */

  /* list = gtk_builder_get_object (builder, "outList"); */
  /* fill_outs(list); */
  /* g_signal_connect (list, "row-selected", G_CALLBACK (select_out), NULL); */

  /* list = gtk_builder_get_object (builder, "inList"); */
  /* fill_ins(list); */
  /* g_signal_connect (list, "row-selected", G_CALLBACK (select_in), NULL); */

  /* list = gtk_builder_get_object (builder, "opList"); */
  /* fill_ops(list); */
  /* g_signal_connect (list, "row-selected", G_CALLBACK (select_op), NULL); */
}
