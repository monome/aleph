#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

#include "net_protected.h"
#include "ui.h"

//----------------------------
//---- "classes"

// just a ScrolledWindow with a ListBox in it
typedef struct _ScrollBox { 
  GtkScrolledWindow* scroll;
  GtkListBox* list;
} ScrollBox;

// fill function type
typedef void (*list_fill_fn) (GtkListBox* list);

// selection handler type
typedef void (*list_select_fn)( GtkListBox *box, gpointer data );

//----------------------------
//--- "methods"

// make a scrollable box
static void scroll_box_new( ScrollBox* scrollbox, 
			    GtkWidget* parent, 
			    list_fill_fn fill,
			    list_select_fn select,
			    int x, int y, int w, int h)
{
  
  GtkWidget* scroll;
  GtkWidget* list;
  scroll = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW(scroll),
				   GTK_POLICY_AUTOMATIC, 
				  GTK_POLICY_AUTOMATIC);

  gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll), 280);
  gtk_scrolled_window_set_min_content_width(GTK_SCROLLED_WINDOW(scroll), 280);
  //  gtk_container_add(GTK_CONTAINER(parent), scroll);
  // ech, assume parent is a grid
  gtk_grid_attach(GTK_GRID(parent), scroll, x, y, w, h);

  list = gtk_list_box_new ();
  g_signal_connect (list, "row-selected", G_CALLBACK(select), NULL);

  (*fill)(GTK_LIST_BOX(list));

  gtk_container_add( GTK_CONTAINER(scroll), list );
  gtk_widget_show (list);

  scrollbox->scroll = GTK_SCROLLED_WINDOW(scroll);
  scrollbox->list = GTK_LIST_BOX(list);

}

//--------------------
//--- selections
static void select_out( GtkListBox *box, gpointer data ) {
  // gotta be a better way to get this
  printf("\r\n selected output; row index: %d", 
	 gtk_list_box_row_get_index(gtk_list_box_get_selected_row(box)) );
  //... 
}

static void select_in( GtkListBox *box, gpointer data ) {
  //... 
}

static void select_op( GtkListBox *box, gpointer data ) {
  //... 
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

  GtkWidget *window;
  GtkWidget *grid;

  ScrollBox boxOuts;
  ScrollBox boxIns;
  ScrollBox boxOps;

  //---  window
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "bees editor");
  gtk_window_set_default_size(GTK_WINDOW(window), 300, 400);
  /// need this?
  //  g_signal_connect (window, "delete-event", G_CALLBACK (delete_handler), NULL);
  g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

  /* //--- layout  */
  /* layout = gtk_layout_new(NULL, NULL); */
  /* gtk_container_add(GTK_CONTAINER(window), layout); */

  //--- grid 
  grid = gtk_grid_new();
  gtk_container_add(GTK_CONTAINER(window), grid);

  //--- scrolling list things 
  scroll_box_new( &boxOuts, grid, &fill_outs, &select_out, 0, 0, 1, 3);
  scroll_box_new( &boxIns, grid, &fill_ins, &select_in, 1, 0, 1, 3 );
  scroll_box_new( &boxOps, grid, &fill_ops, &select_op, 2, 0, 1, 3 );

  /// show everything
  gtk_widget_show_all(window);

}
