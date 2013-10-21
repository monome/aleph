/* pages_protected.h
 * bees
 * aleph
 * 
 * for direct but restricted access to global pages stuff
 */


#ifndef _PAGES_PROTECTED_H_
#define _PAGES_PROTECTED_H_

//#include "op.h"
#include "net.h"
#include "ui.h"
#include "pages.h"
#include "screen.h"

// page structures
extern page_t pages[NUM_PAGES];

// pointer to current page
extern page_t* curPage;
// idx of current page
extern s8 pageIdx;

extern void set_page(ePage n);

// lass pressed key
u8 keyPressed;


#endif // h guard
