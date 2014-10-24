#ifndef _BEEKEEP_UI_H_
#define _BEEKEEP_UI_H_

#include <gtk/gtk.h>

#include "op.h"

//----------------------------
//--- types

// just a ScrolledWindow with a ListBox in it
typedef struct _ScrollBox { 
  GtkScrolledWindow* scroll;
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


/// need this later, maybe...?
/* // class for entry in inputs list */
/* typedef struct _ItemIn {  */
/*   // ListBoxRow container */
/*   GtkWidget *row; */
/*   // text labell */
/*   GtkWidget *label; */
/*   // button for preset invlusion */
/*   GtkWidget *button; */
/* } ItemIn; */


/* // class for entry in outputs list */
/* typedef struct _ItemOut {  */
/*   // ListBoxRow container */
/*   GtkWidget *row; */
/*   // text labell */
/*   GtkWidget *label; */
/*   // button for preset invlusion */
/*   GtkWidget *button; */
/* } ItemOut; */

/* // class for entry in ops list */
/* typedef struct _ItemOp {  */
/*   // top-level ListBoxRow container */
/*   GtkWidget *row; */
/*   // text label */
/*   GtkWidget *label; */
/*   // button for preset inclusion */
/*   GtkWidget *button; */
/* } ItemOp; */


// methods
/* extern itemOut* input_item_new	 ( GtkContainer* parent ); */
/* extern itemIn*  output_item_new ( GtkContainer* parent ); */
/* extern itemOp* 	 op_item_new	 ( GtkContainer* parent ); */

// intialize
extern void ui_init(void);

#endif
