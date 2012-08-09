/* redraw.c
 * bees
 * aleph
 *
 * page-specific redraw functinos.
 */
#include <stdio.h>

#include "param.h"
#include "redraw.h"
#include "scene.h"
#include "screen.h"
#include "types.h"
//#include "ui.h"

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
  snprintf(lineBuf, SCREEN_W, "[ +++ %s",
	   op_registry[userOpTypes[newOpType]].name);
  screen_line(0, SCREEN_H_2, lineBuf, 5);
  // (function labels)
  // don't allow deletion of system operators
  if (net_op_status(net_num_ops() - 1) == eUserOp) {
    screen_line(0, SCREEN_H_1, "PARAM ROUTE CREATE DELETE", 3);
  } else  { 
    screen_line(0, SCREEN_H_1, "PARAM ROUTE CREATE", 3);
  }
}

//==================================================
//==== redraw inputs page
void redraw_ins(void) {
  u8 y = 0;                       // which line
  s32 n, nCenter;         // which list entry
  //  s16 opIdx; // index of operator
  const u16 num = net_num_ins(); // how many 
    
  // draw the header
  snprintf(lineBuf, SCREEN_W, "PARAMS ");
  screen_line(0, 0, lineBuf, 3);

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
  // draw footer 
  // (function labels)
  screen_line(0, SCREEN_H_1, "GATHER DISCONNECT STORE  PRESET ", 3);
}

//==================================================
//==== redraw outputs page
void redraw_outs(void) {
  u8 y = 0;                       // which line
  s32 n, nCenter;         // which list entry
  s16 target;
  const u16 num = net_num_outs(); // how many ops
  
  // draw the header
  snprintf(lineBuf, SCREEN_W, "ROUTING");
  screen_line(0, 0, lineBuf, 3);

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
    screen_line(0, SCREEN_H_2, "NO TARGET", 1);
  } else {
    snprintf(lineBuf, SCREEN_W, "  --> %s/%s",
	     net_op_name(net_in_op_idx(target)), net_in_name(target));
    screen_line(0, SCREEN_H_2, lineBuf, 1);
  }

  // (function labels)
  screen_line(0, SCREEN_H_1, "FOLLOW  DISCONNECT STORE PRESET ", 5);
}

/// redraw gathered outputs
void redraw_gathered(void) {
}

//==================================================
//==== redraw presets page
void redraw_presets(void) {

}

//==================================================
//==== redraw scenes page
void redraw_scenes(void) {
  u8 y = 0;                       // which line
  s32 n, nCenter;         // which list entry
  const u16 num = SCENE_COUNT; // how many 
  
  // draw the header
  snprintf(lineBuf, SCREEN_W, "SCENES");
  screen_line(0, 0, lineBuf, 3);

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

  // draw footer 
  // (function labels)
  screen_line(0, SCREEN_H_1, "CLEAR COPY STORE RECALL", 3);
}

//==================================================
//==== redraw play page
void redraw_play(void) {
  u8 y, n;
// draw the header
  snprintf(lineBuf, SCREEN_W, "PLAY");
  screen_line(0, 0, lineBuf, 6);
  n = SCREEN_H_1;
  for(y = 1; y < SCREEN_H; y++ ) {
    snprintf(lineBuf, SCREEN_W, "p%d : %f",
	     touchedParams[n].idx,
	     touchedParams[n].val );
    screen_line(0, y, lineBuf, 1);
    n--;
  }
}


/////---------------------------
/////  line redraws

// draw line of ops page
static void draw_line_ops(s32 n, u16 num, u8 y, u8 hl) {
  if ( (n < num) && (n >= 0) ) { 
    snprintf(lineBuf, SCREEN_W, " %d_%s",
             (int)n, net_op_name(n));
    //    screen_line(0, y, lineBuf, hl + net_op_status(n));
    screen_line(0, y, lineBuf, hl);
  } else {
    // no selection
    snprintf(lineBuf, SCREEN_W, "   .");
    screen_line(0, y, lineBuf, 0);
  }
  
}

// draw line of inputs page
static void draw_line_ins(s32 n, u16 num, u8 y, u8 hl) {
  s16 opIdx;
  if ( (n < num) && (n >= 0) ) { 
    opIdx = net_in_op_idx(n);
    if (opIdx >=0 ) {
      snprintf(lineBuf, SCREEN_W, "%d_(%d)%s/%s_%f",
	       (int)n,
	       opIdx, 
	       net_op_name(net_in_op_idx(n)), 
	       net_in_name(n), 
	       net_get_in_value(n));
    } else {
      /// parameter
      snprintf(lineBuf, SCREEN_W, "%d_P%d_%s_%f",
	       (int)n,
	       (int)net_param_idx(n),
	       net_in_name(n),
	       ////// FIXME
	       get_param_value(net_param_idx(n)) );
    }
    screen_line(0, y, lineBuf, hl);

  } else {
    // no selection
    snprintf(lineBuf, SCREEN_W, ".");
    screen_line(0, y, lineBuf, 0);
  }

}

// draw line of outputs page
static void draw_line_outs(s32 n, u16 num, u8 y, u8 hl) {
  s16 target;
  //u8 status;
  if ( (n < num) && (n >= 0) ) { 
    target = net_get_target(n);
    //status = net_op_status(net_out_op_idx(n));
    if (target >= 0) {
      snprintf(lineBuf, SCREEN_W, "%d.%s/%s->%d.%s/%s",
	       net_out_op_idx(n),
	       net_op_name(net_out_op_idx(n)), net_out_name(n), 
	       net_in_op_idx(target),
	       net_op_name(net_in_op_idx(target)), net_in_name(target)
	       );
    } else {
      snprintf(lineBuf, SCREEN_W, "%d.%s/%s",
	       net_out_op_idx(n), net_op_name(net_out_op_idx(n)), net_out_name(n));
    }
    //    screen_line(0, y, lineBuf, hl + status);
    screen_line(0, y, lineBuf, hl);
  } else {
    // no selection
    snprintf(lineBuf, SCREEN_W, ".");
    screen_line(0, y, lineBuf, 0);
  }
}

// draw line of scenes page
void draw_line_scenes(s32 n, u16 num, u8 y, u8 hl) {
  //  s16 target; u8 status;
  if ( (n < num) && (n >= 0) ) { 
    snprintf(lineBuf, SCREEN_W, "%s ", scene_name(n));
    screen_line(0, y, lineBuf, hl);
  } else {
    // no selection
    snprintf(lineBuf, SCREEN_W, ".");
    screen_line(0, y, lineBuf, 0);
  }
}
