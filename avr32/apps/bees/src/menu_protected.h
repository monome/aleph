/* menu_protected.h
 * bees
 * aleph
 * 
 * menu-related variables for restricted access
 */


#ifndef _MENU_PROTECTED_H_
#define _MENU_PROTECTED_H_

#include "op.h"
#include "net.h"
#include "ui.h"
#include "menu.h"
#include "pages.h"
#include "screen.h"

///////////////////////
///// variables

// pointer to current page
extern page_t* curPage;
// idx of current page
extern s8 pageIdx;

// array of onode pointers for gathering
extern u32(*gathered)[NET_OUTS_MAX];
// how many gathered
extern u32 numGathered;
// page structures
extern page_t pages[NUM_PAGES];

// const array of user-creatable operator type id's
#define NUM_USER_OP_TYPES 6
#define NUM_USER_OP_TYPES_1 5
extern const op_id_t userOpTypes[NUM_USER_OP_TYPES];
// selected new operator type
extern op_id_t newOpType;

////////////////////////////
///// functions

// set current page
extern void set_page(ePage n);
// scroll current page
extern void scroll_page(s8 dir);
// scroll selection in current page
extern void scroll_select(s8 dir, s32 max);
// scroll selection in current page
extern void scroll_select_clip(s8 dir, s32 max);
// scroll selection in current page
extern void scroll_select_wrap(s8 dir, s32 max);

#endif // header guard
