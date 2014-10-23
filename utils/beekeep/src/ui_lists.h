#ifndef _BEEKEEP_UI_LISTS_H_
#define _BEEKEEP_UI_LISTS_H_

#include "ui.h"

//--------------------------------
//--- extern functions

// fill listboxes
extern void fill_ops(GtkListBox *box);
extern void fill_outs(GtkListBox *box);
extern void fill_ins(GtkListBox *box);
extern void fill_params(GtkListBox *box); 
// TODO
// extern void fill_presets(GtkListBox *box); 


// eh...
/* // refresh listboxes */
/* extern void refresh_list_ops(void); */
/* extern void refresh_list_outs(void); */
/* extern void refresh_list_ins(void); */
/* extern void refresh_list_params(void); */
/* // TODO */
/* // extern void refresh_list_presets(void); */




#endif
