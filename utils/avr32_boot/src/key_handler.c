/* key_handler.c
 * bees
 * aleph
 *
 * page-specific key handling routines
 */

// asf
#ifdef ARCH_AVR32
// #include "print_funcs.h"
#endif
// common
#include "files.h"
// bees
#include "menu_protected.h"
#include "redraw.h"
#include "key_handler.h"


//========================================
//====== key handlers

/// DSP
extern void key_handler_dsp(uiKey_t key, s16 val) {
  switch(key) {
  case eKeyFnDownA:
    // load DSP (and update the parameter list)
    files_load_dsp(curPage->selected);
    break;
  /* case eKeyFnDownB: */
  /*   //// set as default */
  /*   files_store_default_dsp(curPage->selected); */
  /*   break; */
    //// encoder A: scroll pages
  case eKeyEncUpB:
    scroll_page(1);
    break;
  case eKeyEncDownB:
    scroll_page (-1);
    break;
    //// encoder B: scroll selection
  case eKeyEncUpA:
    scroll_select_clip(1,  files_get_dsp_count() );
    break;
  case eKeyEncDownA:
    scroll_select_clip(-1, files_get_dsp_count() );
    break;
  default:
    ;; // nothing
  }  
  (*(curPage->redraw))();
}




/// FIRMWARE
extern void key_handler_fw(uiKey_t key, s16 val) {
  switch(key) {
  case eKeyFnDownA:
    // write firmware to internal flash
    files_write_firmware(curPage->selected);
    break;
    //// encoder A: scroll pages
  case eKeyEncUpB:
    scroll_page(1);
    break;
  case eKeyEncDownB:
    scroll_page (-1);
    break;
    //// encoder B: scroll selection
  case eKeyEncUpA:
    scroll_select_clip(1,  files_get_firmware_count() );
    break;
  case eKeyEncDownA:
    scroll_select_clip(-1, files_get_firmware_count() );
    break;
  default:
    ;; // nothing
  }  
  (*(curPage->redraw))();
}
