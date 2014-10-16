#ifndef _BEEKEEP_UI_H_
#define _BEEKEEP_UI_H_

// class for entry in inputs list
typedef struct _InItem { 
  // ListBoxRow container
  GtkWidget *row;
  // text labell
  GtkWidget *label;
  // button for preset invlusion
  GtkWidget *button;
} InItem;


// class for entry in outputs list
typedef struct _OutItem { 
  // ListBoxRow container
  GtkWidget *row;
  // text labell
  GtkWidget *label;
  // button for preset invlusion
  GtkWidget *button;
} OutItem;

// class for entry in ops list
typedef struct _OpItem { 
  // top-level ListBoxRow container
  GtkWidget *row;
  // text label
  GtkWidget *label;
  // button for preset inclusion
  GtkWidget *button;
} OpItem;


// methods
extern InItem* 	 input_item_new	( GtkContainer* parent );
extern OutItem*  output_item_ne	( GtkContainer* parent );
extern OpItem* 	 op_item_new	( GtkContainer* parent );

// intialize
extern void ui_init(void);

#endif
