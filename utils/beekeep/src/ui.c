// stdlib
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// gtk
#include <gtk/gtk.h>

/// bees
#include "net_protected.h"
#include "scene.h"

// beekeep
#include "ui.h"
#include "ui_files.h"
#include "ui_handlers.h"
#include "ui_lists.h"
#include "ui_op_menu.h"

//----------------------------------
//--- extern variables

// scrollboxes
ScrollBox boxOuts;
ScrollBox boxIns;
ScrollBox boxOps;
ScrollBox boxParams;
ScrollBox boxPresets;

// new-op label
GtkWidget* newOpLabel;
// connect/disconect input button
GtkWidget* connectInputBut;
// connect/disconnect param button
GtkWidget* connectParamBut;
// selections
int opSelect = -1;
int outSelect = -1;
int inSelect = -1;
int paramSelect = -1;
int presetSelect = -1;
op_id_t newOpSelect = -1;

//----------------------------
//--- static functions

// make a scrollable box
static void scroll_box_new( ScrollBox* scrollbox, 
			    GtkWidget* parent, // must be GtkGrid*
			    list_fill_fn fill,
			    //			    list_select_fn select,
			    int x, int y, int w, int h)
{
  
  GtkWidget* scroll;
  GtkWidget* list;

  scroll = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW(scroll),
				  GTK_POLICY_AUTOMATIC,
				  GTK_POLICY_AUTOMATIC);

  //  gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll), 666);
  //  gtk_scrolled_window_set_min_content_width(GTK_SCROLLED_WINDOW(scroll), 255);

  gtk_grid_attach(GTK_GRID(parent), scroll, x, y, w, h);

  list = gtk_list_box_new ();

  (*fill)(GTK_LIST_BOX(list));

  gtk_container_add( GTK_CONTAINER(scroll), list );
  gtk_widget_show (list);

  scrollbox->scroll = GTK_SCROLLED_WINDOW(scroll);
  scrollbox->list = GTK_LIST_BOX(list);
}

// destroyer-iterator
static void destroy_iter(GtkWidget* wgt, gpointer data) {
  gtk_widget_destroy(wgt);
}

// remove all list elements
void scroll_box_clear( ScrollBox* scrollbox ) {
  gtk_list_box_select_row(scrollbox->list, NULL);
  gtk_container_foreach(GTK_CONTAINER(scrollbox->list), &(destroy_iter), NULL);
}

//--------------------
//--- callbacks

static void scene_name_entry_callback( GtkEntry *entry, gpointer data) {
  scene_set_name(gtk_entry_get_text(entry));
}

static void create_op_but_callback( GtkWidget* but, gpointer data) {
  ui_create_op();
}

static void delete_op_but_callback( GtkWidget* but, gpointer data) {
  ui_delete_op();
}

static void connect_in_but_callback( GtkWidget* but, gpointer data) {
  ui_connect_in();
}

static void connect_param_but_callback( GtkWidget* but, gpointer data) {
  ui_connect_param();
}

static void write_scn_but_callback( GtkWidget* but, gpointer data) {
  write_scn();
}

static void write_json_but_callback( GtkWidget* but, gpointer data) {
  write_json();
}

/* not really useful yet
static void write_gv_but_callback( GtkWidget* but, gpointer data) {
  write_gv();
}
*/


//------------------------
//---- init, build, connect
void ui_init(void) {

  GtkWidget *window;
  GtkWidget *grid;
  GtkWidget *label;
  GtkWidget *opMenu;
  GtkWidget *wgt; // temp
  //  GtkWidget *child; // temp
  
  //---  window
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "bees editor");
  //  gtk_window_set_default_size(GTK_WINDOW(window), 300, 400);
  //////////////////
  /// FIXME: need to set a delete handler and do some cleanup.
  /// as it stands, getting core dump from null pointers on window close.
  //  g_signal_connect (window, "delete-event", G_CALLBACK (delete_handler), NULL);
  ///////////////////////
  g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

  // grid layout
  grid = gtk_grid_new();
  gtk_grid_set_column_spacing (GTK_GRID(grid), 40);
  gtk_grid_set_row_spacing (GTK_GRID(grid), 20);
  gtk_container_add(GTK_CONTAINER(window), grid);

  // scene name label
  wgt = gtk_label_new("SCENE:");
  gtk_grid_attach( GTK_GRID(grid), wgt, 0, 0, 1, 1 );

  // scene name
  wgt = gtk_entry_new();
  gtk_entry_set_text( GTK_ENTRY(wgt), scene_get_name() );
  gtk_grid_attach( GTK_GRID(grid), wgt, 1, 0, 4, 1 );
  g_signal_connect( wgt, "activate", G_CALLBACK(scene_name_entry_callback), NULL);

  // export .scn button
  wgt = gtk_button_new_with_label("write .scn");
  gtk_grid_attach( GTK_GRID(grid), wgt, 5, 0, 1, 1 );
  g_signal_connect( wgt, "clicked", G_CALLBACK(write_scn_but_callback), NULL);

  // export .json button
  wgt = gtk_button_new_with_label("write .json");
  gtk_grid_attach( GTK_GRID(grid), wgt, 6, 0, 1, 1 );
  g_signal_connect( wgt, "clicked", G_CALLBACK(write_json_but_callback), NULL);

  // export .gv button
  /* not really useful yet
  wgt = gtk_button_new_with_label("write .gv");
  gtk_grid_attach( GTK_GRID(grid), wgt, 7, 0, 1, 1 );
  g_signal_connect( wgt, "clicked", G_CALLBACK(write_gv_but_callback), NULL);
  */

  // clear button

  // select module button (file dialog?)

  //  list labels
  label = gtk_label_new("OPERATORS");
  gtk_grid_attach(GTK_GRID(grid), label, 0, 4, 2, 1);
  label = gtk_label_new("OUTPUTS");
  gtk_grid_attach(GTK_GRID(grid), label, 2, 4, 4, 1);
  label = gtk_label_new("INPUTS");
  gtk_grid_attach(GTK_GRID(grid), label, 6, 4, 5, 1);
  label = gtk_label_new("PARAMETERS");
  gtk_grid_attach(GTK_GRID(grid), label, 11, 4, 4, 1);
  label = gtk_label_new("PRESETS");
  gtk_grid_attach(GTK_GRID(grid), label, 15, 4, 4, 1);

  //--- create scrolling list things 
  scroll_box_new( &boxOps, grid, &fill_ops,		0, 8, 2, 24 );
  scroll_box_new( &boxOuts, grid, &fill_outs, 		2, 8, 4, 24 );
  scroll_box_new( &boxIns, grid, &fill_ins,  		6, 8, 5, 24 );
  scroll_box_new( &boxParams, grid, &fill_params,	11, 8, 4, 24 );
  scroll_box_new( &boxPresets, grid, &fill_presets, 	15, 8, 4, 24 );

  // new op label
  newOpLabel = gtk_label_new("    ");
  gtk_grid_attach( GTK_GRID(grid), newOpLabel, 0, 32, 1, 1 );

  // new op menu
  opMenu = create_op_menu();
  wgt = gtk_menu_button_new();
  gtk_menu_button_set_popup( GTK_MENU_BUTTON(wgt), opMenu );
  gtk_grid_attach( GTK_GRID(grid), wgt, 1, 32, 1, 1 );

  // create op button
  wgt = gtk_button_new_with_label("CREATE");
  g_signal_connect(wgt, "clicked", G_CALLBACK(create_op_but_callback), NULL);
  gtk_grid_attach( GTK_GRID(grid), wgt, 2, 32, 1, 1 );
  
  // delete op button
  wgt = gtk_button_new_with_label("DELETE");
  g_signal_connect(wgt, "clicked", G_CALLBACK(delete_op_but_callback), NULL);
  gtk_grid_attach( GTK_GRID(grid), wgt, 3, 32, 1, 1 );

  // toggle-connect-to-input button
  //  connectInputBut = gtk_toggle_button_new_with_label("CONNECT");
    connectInputBut = gtk_button_new_with_label("CONNECT");
  g_signal_connect(connectInputBut, "clicked", 
		   G_CALLBACK(connect_in_but_callback), NULL);
  gtk_grid_attach( GTK_GRID(grid), connectInputBut, 6, 32, 1, 1 );
  
  // toggle-connect-to-param button
  //  connectParamBut = gtk_toggle_button_new_with_label("CONNECT");
  connectParamBut = gtk_button_new_with_label("CONNECT");
  g_signal_connect(connectParamBut, "clicked", 
		   G_CALLBACK(connect_param_but_callback), NULL);
  gtk_grid_attach( GTK_GRID(grid), connectParamBut, 10, 32, 1, 1 );

  // store-output-in-preset button

  // store-input-in-preset button


  /// show everything
  gtk_widget_show_all(window);
}


void refresh_connect_input_but(void) {
#if 0
  gboolean c = (net_get_target(outSelect) == (paramSelect + net->numIns));
  GValue v;
  g_value_init(&v,  G_TYPE_BOOLEAN );
  g_value_set_boolean(&v, c);
  //// it would be nice if we could do this without emitting a signal...
  //  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(connectParamBut), c);
  ///.. and this doesn't seem to work:
  g_object_set_property((GObject*)connectInputBut, 
			"active", 
			(const GValue*)&v);
  gtk_widget_show(connectInputBut);
#endif
}


void refresh_connect_param_but(void) {
#if 0
  gboolean c = (net_get_target(outSelect) == (paramSelect + net->numIns));
  GValue v;
  g_value_init(&v,  G_TYPE_BOOLEAN );
  g_value_set_boolean(&v, c);
  //  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(connectParamBut), c);
  g_object_set_property((GObject*)connectParamBut, 
			"active", 
			(const GValue*)&v);

  gtk_widget_show(connectParamBut);
#endif
}
