#ifndef _BEEKEEP_UI_H_
#define _BEEKEEP_UI_H_

#include <gtk/gtk.h>

#include "op.h"

//----------------------------
//--- types

// just a ScrolledWindow with a ListBox in it
typedef struct _ScrollBox { 
  //  GtkScrolledWindow* scroll;
  GtkWidget* scroll;
  GtkListBox* list;
} ScrollBox;

// fill function type
typedef void (*list_fill_fn) (GtkListBox* list);

// selection handler type
typedef void (*list_select_fn)( GtkListBox *box, gpointer data );

//-----------------------------
//---- variables
// scroll boxes
extern ScrollBox boxOuts;
extern ScrollBox boxIns;
extern ScrollBox boxOps;
extern ScrollBox boxParams;
extern ScrollBox boxPresets;
// new-op label
extern GtkWidget* newOpLabel;
// connect/disconect input button
extern GtkWidget* connectInputBut;
// connect/disconnect param button
extern GtkWidget* connectParamBut;

// selections
// negative == no selection
extern int opSelect;
extern int outSelect;
extern int inSelect;
extern int paramSelect;
extern int presetSelect;
extern op_id_t newOpSelect;

//--------------------------------
//---- functions

// clear a scrollbox
extern void scroll_box_clear( ScrollBox* scrollbox );

// refresh toggle buttons
extern void refresh_connect_input_but(void);
extern void refresh_connect_param_but(void);

// intialize
extern void ui_init(void);

#endif
