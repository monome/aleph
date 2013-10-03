/* key_handler.c
 * bees
 * aleph
 *
 * page-specific key handling routines
 */

// asf
#include "delay.h"
#ifdef ARCH_AVR32
#include "print_funcs.h"
#endif
// common
#include "files.h"
// avr32
#include "app.h"
#include "bfin.h"
// bees
#include "op.h"
#include "op_math.h"
#include "menu_protected.h"
#include "param.h"
#include "preset.h"
#include "redraw.h"
#include "scene.h"
#include "key_handler.h"

#if 0
#define FAST_KNOB_LSHIFT 4

//--------------------
//----- static variables

// return param increment given encoder ticks
static fix16 scale_knob_value(const s32 v);

// scroll a single character in a string
#define MAX_EDIT_CHAR 120
#define MIN_EDIT_CHAR 32

// scroll character up
static void edit_string_inc_char(char* str, u8 pos) {
  u8 tmp = str[pos]; 
  if(tmp == 0) { tmp = MIN_EDIT_CHAR; }
  if ( tmp < MAX_EDIT_CHAR ) {
    tmp++;
  } else {
    tmp = MIN_EDIT_CHAR;
  }
  str[pos] = tmp;
  //return tmp;
}

// scroll character down
static void edit_string_dec_char(char* str, u8 pos) {
  u8 tmp = str[pos]; 
  if (tmp > MIN_EDIT_CHAR) {
    tmp--;
  } else {
    tmp = MAX_EDIT_CHAR;
  }
  str[pos] = tmp;
  //  return tmp;
}

//--------------------------
//--- static func def
// return param increment given encoder ticks
static fix16 scale_knob_value(const s32 v) {
  s32 vabs = BIT_ABS(v);
  //  print_dbg("\r\n knob acc, val: ");
  //  print_dbg_hex((u32)v);
  if(vabs < 4) {
    //    print_dbg("\r\n ");
    return v;
  } else if (vabs < 8) {
    //    print_dbg("\r\n knob acc 1");
    return v << 2;
  } else if (vabs < 12) {
    //    print_dbg("\r\n knob acc 2");
    return v << 4;
  } else if (vabs < 19) {
    //    print_dbg("\r\n knob acc 3");
    return v << 5;
  } else if (vabs < 25) {
    //    print_dbg("\r\n knob acc 4");
    return v << 6;  } 
  else if (vabs < 32) {
    //    print_dbg("\r\n knob acc 4");
    return v << 6;
  } else {
    //    print_dbg("\r\n knob acc max");
    return v << 12;
  }
}


//========================================
//====== key handlers
//--- OPS
void key_handler_ops(uiKey_t key, s16 val) {
  u16 n;
  switch(key) {
  case eKeyFnDownA: 
    // fnA go to selected operator's inputs on INS page
    pages[ePageIns].selected = net_op_in_idx(curPage->selected, 0);
    set_page(ePageIns);
    break;
  case eKeyFnDownB:
    // fnB : go to this operator's outputs on OUTS page
    pages[ePageOuts].selected = net_op_out_idx(curPage->selected, 0);
    set_page(ePageOuts);
    break;
  case eKeyFnDownC:
    // fnC : create new operator of specified type
    net_add_op(userOpTypes[newOpType]);
    // redraw_ops();
    break;
  case eKeyFnDownD:
    // fnD : delete
    // FIXME: need to add arbitrary op deletion.
    //// 
    // right now this will destroy the last created op
    /////// + added the function, need to try it out
    /// dont delete if this is a sysem-owned operator
    if(net_op_flag(net_num_ops()- 1, eOpFlagSys)) {
      return;
    }
    net_pop_op();
    n = net_num_ops() - 1;
    if (curPage->selected > n) {
      curPage->selected = n;
    }
    // redraw_ops();
    break;
    //// encoder A: scroll pages
  case eKeyEncUpA:
    scroll_page(1);
    break;
  case eKeyEncDownA:
    scroll_page(0);
    break;
    //// encoder B: scroll selection
  case eKeyEncUpB:
    scroll_select(1, net_num_ops() );
    break;
  case eKeyEncDownB:
    scroll_select(-1, net_num_ops() ); 
    break;
    //// encoder C: move up/down in order of execution
  case eKeyEncUpC:
    // TODO
    break;
  case eKeyEncDownC:
    // TODO
    break;
    //// encoder D: select new operator type for creation
  case eKeyEncUpD:
    newOpType++;
    if (newOpType >= NUM_USER_OP_TYPES) {
      newOpType = 0;
    }
    // redraw_ops();
    break;
  case eKeyEncDownD:
    newOpType--;
    if (newOpType >= NUM_USER_OP_TYPES) {
      newOpType = NUM_USER_OP_TYPES_1;
    }
    // redraw_ops();
    // nothing
    break;
  
  default:
    ;; // nothing
  }  
}

//--- INS
void key_handler_ins(uiKey_t key, s16 val) {
  u32 i;
  switch(key) {
  case eKeyFnDownA:
    // fnA : gather
    numGathered = net_gather(curPage->selected, gathered);
    break;
  case eKeyFnDownB:
    // fnB : disconnect
    numGathered = net_gather(curPage->selected, gathered);
    for(i=0; i<numGathered; i++) {
      net_disconnect(*(gathered[i]));
    }
    break;
  case eKeyFnDownC:
    // fnC : overwrite preset value (and include)
    ///////
    //////////
    /// testing
    // store default scene
    //    scene_write_default();

    /////////
    ////////
    break;
  case eKeyFnDownD:
    // toggle preset inclusion
    net_toggle_in_preset(curPage->selected);
    break;
    //// encoder A: scroll pages
  case eKeyEncUpA:
    scroll_page(1);
    break;
  case eKeyEncDownA:
    scroll_page(0);
    break;
    //// encoder B: scroll selection
  case eKeyEncUpB:
    scroll_select(1, net_num_ins() );
    break;
  case eKeyEncDownB:
    scroll_select(-1, net_num_ins() );      
    break;
  case eKeyEncUpC:
    // encoder C : value slow
    net_inc_in_value(curPage->selected, scale_knob_value(val));
    break;
  case eKeyEncDownC:
    net_inc_in_value(curPage->selected, scale_knob_value(val));
    break;
  case eKeyEncUpD:
    net_inc_in_value(curPage->selected, scale_knob_value(val) << FAST_KNOB_LSHIFT);
    break;
  case eKeyEncDownD:
    net_inc_in_value(curPage->selected, scale_knob_value(val) << FAST_KNOB_LSHIFT);
    break;
  default:
    ;; // nothing
  }  
  (*(curPage->redraw))();
}

//--- OUTS
void key_handler_outs(uiKey_t key, s16 val) {
  s16 i;
  static s32 target;
  switch(key) {
  case eKeyFnDownA: 
    // follow
    i = net_get_target(curPage->selected);
    if (i == -1) { return; } 
    pages[ePageIns].selected = i;
    set_page(ePageIns);
    break;
  case eKeyFnDownB:
    // disconnect
    net_disconnect(curPage->selected);
    break;
  case eKeyFnDownC:
    // re-send? store?
    break;
  case eKeyFnDownD:
    // toggle preset (target)
    i = net_get_target(curPage->selected);
    if(i>=0) { net_toggle_in_preset(i); }
    break;
    //// encoder A: scroll pages
  case eKeyEncUpA:
    scroll_page(1);
    break;
  case eKeyEncDownA:
    scroll_page(0);
    break;
    //// encoder B: scroll selection
  case eKeyEncUpB:
    scroll_select(1, net_num_outs() );
    break;
  case eKeyEncDownB:
    scroll_select(-1, net_num_outs() );      
    break;
    //// encoder C: scroll target
  case eKeyEncUpC:
    target++;
    if (target == net_num_ins()) {
      target = -1;
    }
    net_connect(curPage->selected, target);
    break;
  case eKeyEncDownC:
    target--;
    if (target == -2) {
      target = net_num_ins() - 1;
    }
    net_connect(curPage->selected, target);
    break;
  case eKeyEncUpD:
    // nothing
    break;
  case eKeyEncDownD:
    // nothing
    break;
    default:
    ;; // nothing
  }  
  (*(curPage->redraw))();
}

//--- GATHERED
void key_handler_gathered(uiKey_t key, s16 val) {
  key_handler_outs(key, val);
}

//--- PLAY
void key_handler_play(uiKey_t key, s16 v) {
  s32 val;
  s16 inIdx = -1;
  //activate the 8 system operators from net.c : add_sys_ops()
  switch(key) {
    ///// keys
  case eKeyFnUpA:
    inIdx = net_op_in_idx(4, 0); // op 4: switch, in 0: val
    val = 0;
    break;
  case eKeyFnDownA:
    inIdx = net_op_in_idx(4, 0); // op 4: switch, in 0: val
    val = 1;
    break;
  case eKeyFnUpB:
    inIdx = net_op_in_idx(5, 0); // op 5: switch, in 0: val
    val = 0;
    break;
  case eKeyFnDownB:
    inIdx = net_op_in_idx(5, 0); // op 5: switch, in 0: val
    val = 1;
    break;
  case eKeyFnUpC:
    inIdx = net_op_in_idx(6, 0); // op 6: switch, in 0: val
    val = 0;
    break;
  case eKeyFnDownC:
    inIdx = net_op_in_idx(6, 0); // op 6: switch, in 0: val
    val = 1;
    break;
  case eKeyFnUpD:
    inIdx = net_op_in_idx(7, 0); // op 7: switch, in 0: val
    val = 0;
    break;
  case eKeyFnDownD:
    inIdx = net_op_in_idx(7, 0); // op 7: switch, in 0: val
    val = 1;
    break;
    ///// encoders
   case eKeyEncUpA:
    inIdx = net_op_in_idx(0, 0); // op 0: enc, in 0: move
    val = 1;
    break;
   case eKeyEncDownA:
    inIdx = net_op_in_idx(0, 0); // op 0: enc, in 0: move
    val = -1;
    break;
   case eKeyEncUpB:
    inIdx = net_op_in_idx(1, 0); // op 1: enc, in 0: move
    val = 1;
    break;
   case eKeyEncDownB:
    inIdx = net_op_in_idx(1, 0); // op 1: enc, in 0: move
    val = -1;
    break;
   case eKeyEncUpC:
    inIdx = net_op_in_idx(2, 0); // op 2: enc, in 0: move
    val = 1;
    break;
   case eKeyEncDownC:
    inIdx = net_op_in_idx(2, 0); // op 2: enc, in 0: move
    val = -1;
    break;
   case eKeyEncUpD:
    inIdx = net_op_in_idx(3, 0); // op 3: enc, in 0: move
    val = 1;
    break;
   case eKeyEncDownD:
    inIdx = net_op_in_idx(3, 0); // op 3: enc, in 0: move
    val = -1;
    break;
   default:
    ;;
    break;
  }
  //  print_dbg("\r\n activating index from play mode: ");
  //  print_dbg_ulong(inIdx);
  if(inIdx >= 0) {
    //    print_dbg("\r\n key_handler_play");
    net_activate(inIdx, val, NULL);
    // redraw_play();
  }
}

//--- PRESETS
extern void key_handler_presets(uiKey_t key, s16 val) {
  //  s16 i;
  switch(key) {
  case eKeyFnDownA: // clear
    switch(curPage->mode) {
    case eModeNone:
      curPage->mode = eModeClear;
      break;
    case eModeClear:
      //preset_clear(curPage->selected);
      curPage->mode = eModeNone;
      break;
    default:
      curPage->mode = eModeNone;
    }
    break;
  case eKeyFnDownB: // copy
    switch(curPage->mode) {
    case eModeNone:
      curPage->mode = eModeCopy;
      break;
    case eModeCopy:
      //preset_copy(curPage->selected);
      curPage->mode = eModeNone;
      break;
    default:
      curPage->mode = eModeNone;
    }
    break;
  case eKeyFnDownC: // write
    switch(curPage->mode) {
    case eModeNone:
      curPage->mode = eModeWrite;
      break;
    case eModeWrite:
      preset_store(curPage->selected);
      curPage->mode = eModeNone;
      break;
    default:
      curPage->mode = eModeNone;
    }
    break;
  case eKeyFnDownD: // read
    switch(curPage->mode) {
    case eModeNone:
      curPage->mode = eModeRead;
      break;
    case eModeRead:
      preset_recall(curPage->selected);
      curPage->mode = eModeNone;
      break;
    default:
      curPage->mode = eModeNone;
    }
    break;
    //// encoder A: scroll pages
  case eKeyEncUpA:
    scroll_page(1);
    break;
  case eKeyEncDownA:
    scroll_page(0);
    break;
    //// encoder B: scroll selection
  case eKeyEncUpB:
    scroll_select(1, NET_PRESETS_MAX );
    break;
  case eKeyEncDownB:
    scroll_select(-1, NET_PRESETS_MAX ); 
    break;
    //// encoder C: scroll name pos
  case eKeyEncUpC:
    curPage->cursor++;
    if (curPage->cursor >= PRESET_NAME_LEN) {
      curPage->cursor = 0;
    } 
    break;
  case eKeyEncDownC:
      curPage->cursor--;
    if (curPage->cursor < 0) {
      curPage->cursor = PRESET_NAME_LEN - 1;
    } 
    break;
  case eKeyEncUpD:
    // scroll name char
    edit_string_inc_char(preset_name(curPage->selected), curPage->cursor);
    break;
  case eKeyEncDownD:
    // scroll name char
    edit_string_dec_char(preset_name(curPage->selected), curPage->cursor);
    break;
    default:
    ;; // nothing
  }  
  (*(curPage->redraw))();
}

//--- SCENES
extern void key_handler_scenes(uiKey_t key, s16 val) {
  switch(key) {


  case eKeyFnDownA: // write
    switch(curPage->mode) {
    case eModeNone:
      curPage->mode = eModeWrite;
      break;
    case eModeWrite:
      files_store_scene_name(sceneData->desc.sceneName);
      print_dbg("\r\n wrote current scene to sdcard.");
      curPage->mode = eModeNone;
      break;
    default:
      curPage->mode = eModeNone;
    }
    break;

  /* case eKeyFnDownA: // clear */
  /*   switch(curPage->mode) { */
  /*   case eModeNone: */
  /*     curPage->mode = eModeClear; */
  /*     break; */
  /*   case eModeClear: */
  /*     //scene_clear(curPage->selected); */
  /*     curPage->mode = eModeNone; */
  /*     break; */
  /*   default: */
  /*     curPage->mode = eModeNone; */
  /*   } */
  /*   break; */

  case eKeyFnDownB: // read
    switch(curPage->mode) {
    case eModeNone:
      curPage->mode = eModeRead;
      break;
    case eModeRead:
      //      app_notify("\r\n reading scene... ");
      files_load_scene(curPage->selected);
      curPage->mode = eModeNone;
      break;
    default:
      curPage->mode = eModeNone;
    }
    break;

  case eKeyFnDownC: // copy
    switch(curPage->mode) {
    case eModeNone:
      curPage->mode = eModeCopy;
      break;
    case eModeCopy:
      //scene_copy(curPage->selected);
      curPage->mode = eModeNone;
      break;
    default:
      curPage->mode = eModeNone;
    }
    break;

 
 case eKeyFnDownD: // default
    switch(curPage->mode) {
    case eModeNone:
      curPage->mode = eModeDefault;
      break;
    case eModeDefault:
      //      app_notify("writing default scene...");
      scene_write_default();
      curPage->mode = eModeNone;
      break;
    default:
      curPage->mode = eModeNone;
    }
    break;

    //// encoder A: scroll pages
  case eKeyEncUpA:
    scroll_page(1);
    break;
  case eKeyEncDownA:
    scroll_page(0);
    break;
    //// encoder B: scroll selection
  case eKeyEncUpB:
    scroll_select(1, files_get_scene_count() );
    break;
  case eKeyEncDownB:
    //
    scroll_select(-1, files_get_scene_count() );
    break;
  case eKeyEncUpC: // cursor: position in name
    curPage->cursor++;
    if (curPage->cursor >= SCENE_NAME_LEN) {
      curPage->cursor = 0;
    } 
    break;
  case eKeyEncDownC:  // cursor: position in name
    curPage->cursor--;
    if (curPage->cursor < 0) {
      curPage->cursor = SCENE_NAME_LEN - 1;
    } 
    break;
  case eKeyEncUpD:     // scroll name char at pos
    edit_string_inc_char(sceneData->desc.sceneName, curPage->cursor);
    break;
  case eKeyEncDownD:     // scroll name char at pos
    edit_string_dec_char(sceneData->desc.sceneName, curPage->cursor);
    break;
  default:
    ;; // nothing
  }  
  (*(curPage->redraw))();
}

//--- DSP
extern void key_handler_dsp(uiKey_t key, s16 val) {
  // print_dbg("\r\n key_handler_dsp");
  switch(key) {
  case eKeyFnDownA:
    //    app_notify("loading DSP...");
    // load DSP
    files_load_dsp(curPage->selected);
    delay_ms(100);
    // update param list
    report_params();
    delay_ms(100);
    // start audio
    bfin_enable();
    break;
  case eKeyFnDownB:
    // write default
    //    app_notify("writing default DSP...");
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
#endif
