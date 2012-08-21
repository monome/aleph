/* redraw.c
 * bees
 * aleph
 *
 * page-specific redraw functinos.
 */
#include <stdio.h>

#include "param.h"
#include "preset.h"
#include "redraw.h"
#include "scene.h"
#include "screen.h"
#include "types.h"

#define SNPRINTF

//------------------------
//----- static variables

// line buffer
static char lineBuf[SCREEN_W];

//---------------------------------
//---- static functions

//// line redraws
// op
static void draw_line_ops(s32 n, u16 num, u8 y, u8 statMod);
// in
static void draw_line_ins(s32 n, u16 num, u8 y, u8 statMod);
// out
static void draw_line_outs(s32 n, u16 num, u8 y, u8 statMod);
// presets
static void draw_line_presets(s32 n, u16 num, u8 y, u8 statMod);
// scenes
static void draw_line_scenes(s32 n, u16 num, u8 y, u8 statMod);

//==================================================
//==== redraw ops page
void redraw_ops(void) {
  u8 y = 0;                       // which line
  s32 n, nCenter;         // which list entry
  //  opStatus_t status = eUserOp;
  // total count of ops, including system-controlled
  const u16 num = net_num_ops();

  // draw the header
  screen_line(0, 0, "OPERATORS", 3);

  nCenter = page->selected;
  if (nCenter >= num) {
    nCenter = num;
  }

  n = nCenter;
  // print selection at center
  y = SCREEN_ROW_CENTER;
  draw_line_ops(n, num, y, 4);

  // print lower entries
  while (y > 1) {
    n--;
    y--;
    draw_line_ops(n, num, y, 1);
  }
  
  // re-center
  n = nCenter;
  y = SCREEN_ROW_CENTER;
  
  // print higher entries
  while (y < SCREEN_H_2) {
    n++;
    y++;
    draw_line_ops(n, num, y, 1);
  }
      
  // draw footer 
  // (new op type)
#ifdef SNPRINTF
  snprintf(lineBuf, SCREEN_W, " + %s",
	   op_registry[userOpTypes[newOpType]].name);
#else
#endif
  screen_line(0, kScreenLines[CHAR_ROWS - 2], lineBuf, 5);
  // (function labels)
  // don't allow deletion of system operators
  if (net_op_status(net_num_ops() - 1) == eUserOp) {
    screen_line(0, kScreenLines[CHAR_ROWS-1], "PARAM ROUTE CREATE DELETE", 3);
  } else  { 
    screen_line(0, kScreenLines[CHAR_ROWS-1], "PARAM ROUTE CREATE", 3);
  }
}

//==================================================
//==== redraw inputs page
void redraw_ins(void) {
  u8 y = 0;                       // which line
  s32 n, nCenter;         // which list entry
  //  s16 opIdx; // index of operator
  const u16 num = net_num_ins(); // how many 
    
  nCenter = page->selected;
  if (nCenter >= num) {
    nCenter = num;
  }
  n = nCenter;
  // print selection at center
  y = SCREEN_ROW_CENTER;
  draw_line_ins(n, num, y, 4);
  
  // print lower entries
  while (y > 1) {
    n--;
    y--;
    draw_line_ins(n, num, y, 1);
  }
  
  // re-center
  n = nCenter;
  y = SCREEN_ROW_CENTER;
  
  // print higher entries
  while (y < SCREEN_H_1) {
    n++;
    y++;
    draw_line_ins(n, num, y, 1);
  }    

  // draw the header
#ifdef SNPRINTF
  snprintf(lineBuf, SCREEN_W, "PARAMS ");
#else
#endif
  screen_line(0, 0, lineBuf, 3);

  // draw footer 
  // (function labels)
  screen_line(0, kScreenLines[CHAR_ROWS - 1], "GATHER DISCONNECT STORE  PRESET ", 3);
}

//==================================================
//==== redraw outputs page
void redraw_outs(void) {
  u8 y = 0;                       // which line
  s32 n, nCenter;         // which list entry
  s16 target;
  const u16 num = net_num_outs(); // how many ops
  
  nCenter = page->selected;
  if (nCenter >= num) {
    nCenter = num;
  }
  n = nCenter;
  // print selection at center
  y = SCREEN_ROW_CENTER;
  draw_line_outs(n, num, y, 4);  
  // print lower entries
  while (y > 1) {
    n--;
    y--;
    draw_line_outs(n, num, y, 1);
  }
  
  // re-center
  n = nCenter;
  y = SCREEN_ROW_CENTER;
  
  // print higher entries
  while (y < SCREEN_H_2) {
    n++;
    y++;
    draw_line_outs(n, num, y, 1);
  }

  // draw footer 
  // (target)
  target = net_get_target(nCenter);
  if(target == -1) {
    screen_line(0, kScreenLines[CHAR_ROWS - 2], "NO TARGET", 1);
  } else {
#ifdef SNPRINTF
    snprintf(lineBuf, SCREEN_W, " -> %s/%s",
	     net_op_name(net_in_op_idx(target)), net_in_name(target));
#else
#endif
    screen_line(0, kScreenLines[CHAR_ROWS - 2], lineBuf, 1);
  }


  // draw the header
#ifdef SNPRINTF
  snprintf(lineBuf, SCREEN_W, "ROUTING");
#else
#endif
  screen_line(0, 0, lineBuf, 3);

  // (function labels)
  screen_line(0, kScreenLines[CHAR_ROWS - 1], "FOLLOW DISCONNECT STORE PRESET", 3);
}

/// redraw gathered outputs
void redraw_gathered(void) {
}

//==================================================
//==== redraw presets page
void redraw_presets(void) {
  u8 y = 0;                       // which line
  s32 n, nCenter;         // which list entry
  const u16 num = NET_PRESETS_MAX; // how many 

  nCenter = page->selected;
  if (nCenter >= num) {
    nCenter = num;
  }
  n = nCenter;
 
 // print selection at center
  y = SCREEN_ROW_CENTER;

  switch(page->mode) { 
  case eModeClear:
    screen_line(0, y, "CLEAR?", 6);
    break;
  case eModeCopy:
    screen_line(0, y, "COPY?", 6);
    break;
  case eModeStore:
    screen_line(0, y, "STORE?", 6);
    break;
  case eModeRecall:
    screen_line(0, y, "RECALL?", 6);
    break;
  case eModeNone:
  default:
    draw_line_presets(n, num, y, 4);  
    break;
  }
  // print lower entries
  while (y > 1) {
    n--;
    y--;
    draw_line_presets(n, num, y, 1);
  }
  
  // re-center
  n = nCenter;
  y = SCREEN_ROW_CENTER;
  
  // print higher entries
  while (y < SCREEN_H_2) {
    n++;
    y++;
    draw_line_presets(n, num, y, 1);
  }

  // draw footer 
  // (function labels)
  screen_line(0, kScreenLines[CHAR_ROWS - 1], "CLEAR COPY STORE RECALL", 3);
  
  // draw the header
#ifdef SNPRINTF
  snprintf(lineBuf, SCREEN_W, "PRESETS");
#else
#endif
  screen_line(0, 0, lineBuf, 3);

}

//==================================================
//==== redraw scenes page
void redraw_scenes(void) {
  u8 y = 0;                       // which line
  s32 n, nCenter;         // which list entry
  const u16 num = SCENE_COUNT; // how many 

  nCenter = page->selected;
  if (nCenter >= num) {
    nCenter = num;
  }
  n = nCenter;
  // print selection at center
  y = SCREEN_ROW_CENTER;
  switch(page->mode) { 
  case eModeClear:
    screen_line(0, y, "CLEAR?", 6);
    break;
  case eModeCopy:
    screen_line(0, y, "COPY?", 6);
    break;
  case eModeStore:
    screen_line(0, y, "STORE?", 6);
    break;
  case eModeRecall:
    screen_line(0, y, "RECALL?", 6);
    break;
  case eModeNone:
  default:
    draw_line_scenes(n, num, y, 4);  
    break;
  }
  // print lower entries
  while (y > 1) {
    n--;
    y--;
    draw_line_scenes(n, num, y, 1);
  }
  
  // re-center
  n = nCenter;
  y = SCREEN_ROW_CENTER;
  
  // print higher entries
  while (y < SCREEN_H_2) {
    n++;
    y++;
    draw_line_scenes(n, num, y, 1);
  }

  
  // draw the header
#ifdef SNPRINTF
  snprintf(lineBuf, SCREEN_W, "SCENES");
#else
#endif
  screen_line(0, 0, lineBuf, 3);

  // draw footer 
  // (function labels)
  screen_line(0, kScreenLines[CHAR_ROWS - 1], "CLEAR COPY STORE RECALL", 3);
}

//==================================================
//==== redraw play page
void redraw_play(void) {
  u8 y, n;
  n = SCREEN_H_1;
  for(y = 1; y < SCREEN_H; y++ ) {
#ifdef SNPRINTF
    snprintf(lineBuf, SCREEN_W, "p%d : %f",
	     touchedParams[n].idx,
	     touchedParams[n].val );
#else
#endif
    screen_line(0, kScreenLines[y], lineBuf, 1);
    n--;
  }
// draw the header
#ifdef SNPRINTF
  snprintf(lineBuf, SCREEN_W, "PLAY");
#else
#endif
  screen_line(0, 0, lineBuf, 6);

}


/////---------------------------
/////  line redraws

// draw line of ops page
static void draw_line_ops(s32 n, u16 num, u8 y, u8 hl) {

  // wrap
  if (n < 0) {
    n += num;
  } else if (n >= num) {
    n -= num;
  } 

  //  if ( (n < num) && (n >= 0) ) { 
#ifdef SNPRINTF
  snprintf(lineBuf, SCREEN_W, "%d.%s",
             (int)n, net_op_name(n));
#else
#endif
    //    screen_line(0, kScreenLines[y], lineBuf, hl + net_op_status(n));
    screen_line(0, kScreenLines[y], lineBuf, hl);
    /*  } else {
    // no selection
#ifdef SNPRINTF
snprintf(lineBuf, SCREEN_W, "   .");
    screen_line(0, kScreenLines[y], lineBuf, 0);
  }
    */
  
}

// draw line of inputs page
static void draw_line_ins(s32 n, u16 num, u8 y, u8 hl) {
  s16 opIdx;
  char pch;
  // wrap
  if (n < 0) {
    n += num;
  } else if (n >= num) {
    n -= num;
  } 
  //  if ( (n < num) && (n >= 0) ) { 
    opIdx = net_in_op_idx(n);
    if (net_get_in_preset(n)) { pch = '*'; } else { pch = '.'; }
    if (opIdx >=0 ) {
#ifdef SNPRINTF
      snprintf(lineBuf, SCREEN_W, "%d%c%s/%s_%f",
	       //	       (int)n,
	       opIdx, 
	       pch,
	       net_op_name(net_in_op_idx(n)), 
	       net_in_name(n), 
	       net_get_in_value(n) );
#else
#endif
    } else {
      /// parameter
#ifdef SNPRINTF
      snprintf(lineBuf, SCREEN_W, "p%d%c%s_%f",
	       //	       (int)n,
	       (int)net_param_idx(n),
	       pch,
	       net_in_name(n),
	       ////// FIXME (?)
	       get_param_value(net_param_idx(n)) );
#else
#endif
    }
    screen_line(0, kScreenLines[y], lineBuf, hl);

    //} else {
    // no selection
    //#ifdef SNPRINTF
    //snprintf(lineBuf, SCREEN_W, ".");
    //screen_line(0, kScreenLines[y], lineBuf, 0);
    // }

}

// draw line of outputs page
static void draw_line_outs(s32 n, u16 num, u8 y, u8 hl) {
  s16 target;
  char pch;
  //  u8 status;
  // wrap
  if (n < 0) {
    n += num;
  } else if (n >= num) {
    n -= num;
  } 
  target = net_get_target(n);
  //  status = net_op_status(net_out_op_idx(n));    // no selection
  if (net_get_out_preset(n)) { pch = '*'; } else { pch = '.'; }
  if (target >= 0) {
#ifdef SNPRINTF
    snprintf(lineBuf, SCREEN_W, "%d%c%s/%s->%d.%s/%s",
	     net_out_op_idx(n),
	     pch,
	     net_op_name(net_out_op_idx(n)),
	     net_out_name(n), 
	     net_in_op_idx(target),
	     net_op_name(net_in_op_idx(target)),
	     net_in_name(target) );
#else
#endif
  } else {
#ifdef SNPRINTF
    snprintf(lineBuf, SCREEN_W, "%d%c%s/%s",
	     net_out_op_idx(n),
	     pch,
	     net_op_name(net_out_op_idx(n)),
	     net_out_name(n) );
#else
#endif
  }
  screen_line(0, kScreenLines[y], lineBuf, hl);// + status);
  
}

// draw line of presets page
void draw_line_presets(s32 n, u16 num, u8 y, u8 hl) {
  // wrap
  if (n < 0) {
    n += num;
  } else if (n >= num) {
    n -= num;
  } 
  
#ifdef SNPRINTF
  snprintf(lineBuf, SCREEN_W, "%d.%s ", (int)n, preset_name(n));
#else
#endif
  screen_line(0, kScreenLines[y], lineBuf, hl);
}

// draw line of scenes page
void draw_line_scenes(s32 n, u16 num, u8 y, u8 hl) {
  // wrap
  if (n < 0) {
    n += num;
  } else if (n >= num) {
    n -= num;
  } 
#ifdef SNPRINTF
  snprintf(lineBuf, SCREEN_W, "%d.%s ", (int)n, scene_name(n));
#else
#endif
  screen_line(0, kScreenLines[y], lineBuf, hl);
}
