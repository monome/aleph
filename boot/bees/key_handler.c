/* key_handler.c
 * bees
 * aleph
 *
 * page-specific key handling routines
 */

// asf
#ifdef ARCH_AVR32
#include "print_funcs.h"
#endif
// common
#include "files.h"
// bees
#include "op.h"
#include "op_math.h"
#include "menu_protected.h"
#include "param.h"
#include "preset.h"
#include "redraw.h"
#include "scene.h"
#include "key_handler.h"

#define FAST_KNOB_LSHIFT 4

//--------------------
//----- static variables

// character selection table
static const char kLabelChars[] = "abcdefghijklmnopqrstuvwxyz_012345789";
#define NUM_LABEL_CHARS 47
// index in the selection table 
//static s8 selectedLabelChar = 0;

// return param increment given encoder ticks
// static fix16 scale_knob_value(const s32 v);

//--------------------------
//--- static func def
// return param increment given encoder ticks
/* static fix16 scale_knob_value(const s32 v) { */
/*   s32 vabs = BIT_ABS(v); */
/*   if(vabs < 6) { */
/*     //    print_dbg("\r\n slow"); */
/*     return v; */
/*   } else if (vabs < 12) { */
/*     //    print_dbg("\r\n fast"); */
/*     return v * 8; */
/*   } else { */
/*     //    print_dbg("\r\n fastest"); */
/*     return v * 32; */
/*   } */
/* } */

//========================================
//====== key handlers

/// DSP
extern void key_handler_dsp(uiKey_t key, s16 val) {
  switch(key) {
  case eKeyFnDownA:
    // load DSP (and update the parameter list)
    files_load_dsp(curPage->selected);
    break;
  case eKeyFnDownB:
    //// set as default
    files_store_default_dsp(curPage->selected);
    break;
  case eKeyFnDownC:
    break;
  case eKeyFnDownD:
    break;
    //// encoder A: scroll pages
  case eKeyEncUpA:
    scroll_page(1);
    break;
  case eKeyEncDownA:
    scroll_page (-1);
    break;
    //// encoder B: scroll selection
  case eKeyEncUpB:
    scroll_select_clip(1,  files_get_dsp_count() );
    break;
  case eKeyEncDownB:
    scroll_select_clip(-1, files_get_dsp_count() );
    break;
  case eKeyEncUpC:
    break;
  case eKeyEncDownC:
    break;
  case eKeyEncUpD:     
    break;
  case eKeyEncDownD:     
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
  case eKeyFnDownB:
    break;
  case eKeyFnDownC:
    break;
  case eKeyFnDownD:
    break;
    //// encoder A: scroll pages
  case eKeyEncUpA:
    scroll_page(1);
    break;
  case eKeyEncDownA:
    scroll_page (-1);
    break;
    //// encoder B: scroll selection
  case eKeyEncUpB:
    scroll_select_clip(1,  files_get_dsp_count() );
    break;
  case eKeyEncDownB:
    scroll_select_clip(-1, files_get_dsp_count() );
    break;
  case eKeyEncUpC:
    break;
  case eKeyEncDownC:
    break;
  case eKeyEncUpD:     
    break;
  case eKeyEncDownD:     
    break;
  default:
    ;; // nothing
  }  
  (*(curPage->redraw))();
}
