#include "op.h"
#include "ui_handlers.h"
#include "ui_op_menu.h"

static void op_menu_select( GtkWidget* widget, gpointer data ) {
  op_id_t* pOpId = (op_id_t*)data;
  ui_select_new_op(*pOpId);
  printf("\r\n op selection: %s", op_registry[*pOpId].name);
}

//GtkWidget* create_op_menu(GtkContainer* parent) {
GtkWidget* create_op_menu(void) {
  GtkWidget* menu;
  GtkWidget* child;
  int i;
  op_id_t id;





  const char* name;

  menu = gtk_menu_new();

  for(i=0; i<NUM_USER_OP_TYPES; i++) {
    id = userOpTypes[i];
    name = op_registry[id].name;
    child = gtk_menu_item_new_with_label(name);
    gtk_menu_attach(GTK_MENU(menu), child, 0, 1, i+1, i+2);
    //////////////////
    // huh, have to do this here.
    gtk_widget_show(child);
    //////////////////
    g_signal_connect( child, 
		      "activate", 
		      G_CALLBACK(op_menu_select), 
		      (gpointer)&(userOpTypes[i]) );
  }
  return menu;
}
