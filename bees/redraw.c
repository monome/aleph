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
#include "types.h"
#include "ui.h"

//------------------------
//----- static variables

// line buffer
static char lineBuf[SCREEN_W];

//---------------------------------
//---- static functions

//// line redraws
// op
static void draw_line_ops(S32 n, U16 num, U8 y, U8 statMod);
// in
static void draw_line_ins(S32 n, U16 num, U8 y, U8 statMod);
// out
static void draw_line_outs(S32 n, U16 num, U8 y, U8 statMod);
// scenes
static void draw_line_scenes(S32 n, U16 num, U8 y, U8 statMod);

//==================================================
//==== redraw ops page
void redraw_ops(void) {
  U8 y = 0;                       // which line
  S32 n, nCenter;         // which list entry
  //  opStatus_t status = eUserOp;
  // total count of ops, including system-controlled
  const U16 num = net_num_ops();

  // draw the header
  snprintf(lineBuf, SCREEN_W, "         OPERATORS");
  ui_print(0, 0, lineBuf, 6);

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
  ui_print(SCREEN_H_2, 0, lineBuf, 1);
  // (function labels)
  // don't allow deletion of system operators
  if (net_op_status(net_num_ops() - 1) == eUserOp) {
    ui_print(SCREEN_H_1, 0, " A_PARAMS   B_ROUTING   C_CREATE  D_DELETE ", 5);
  } else  {
    ui_print(SCREEN_H_1, 0, " A_PARAMS   B_ROUTING   C_CREATE  ", 5);
  }
}

//==================================================
//==== redraw inputs page
void redraw_ins(void) {
  U8 y = 0;                       // which line
  S32 n, nCenter;         // which list entry
  //  S16 opIdx; // index of operator
  const U16 num = net_num_ins(); // how many ops
    
  // draw the header
  snprintf(lineBuf, SCREEN_W, "       PARAMS ");
  ui_print(0, 0, lineBuf, 6);

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
  ui_print(SCREEN_H_1, 0, "A_GATHER  B_DISCONNECT  C_STORE  D_PRESET ", 5);
}

//==================================================
//==== redraw outputs page
void redraw_outs(void) {
  U8 y = 0;                       // which line
  S32 n, nCenter;         // which list entry
  S16 target;
  const U16 num = net_num_outs(); // how many ops
  
  // draw the header
  snprintf(lineBuf, SCREEN_W, "      ROUTING");
  ui_print(0, 0, lineBuf, 6);

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
    ui_print(SCREEN_H_2, 0, "[ NO TARGET ]", 1);
  } else {
    snprintf(lineBuf, SCREEN_W, "  --> %s/%s",
	     net_op_name(net_in_op_idx(target)), net_in_name(target));
    ui_print(SCREEN_H_2, 0, lineBuf, 1);
  }

  // (function labels)
  ui_print(SCREEN_H_1, 0, " A_FOLLOW  B_DISCONNECT C_STORE  D_PRESET ", 5);
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
  U8 y = 0;                       // which line
  S32 n, nCenter;         // which list entry
  const U16 num = SCENE_COUNT; // how many 
  
  // draw the header
  snprintf(lineBuf, SCREEN_W, "      SCENES");
  ui_print(0, 0, lineBuf, 6);

  nCenter = page->selected;
  if (nCenter >= num) {
    nCenter = num;
  }
  n = nCenter;
  // print selection at center
  y = SCREEN_ROW_CENTER;
  draw_line_scenes(n, num, y, 4);  
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
  ui_print(SCREEN_H_1, 0, " A_CLEAR B_COPY C_STORE D_RECALL ", 5);

}

//==================================================
//==== redraw play page
void redraw_play(void) {
  U8 y, n;
// draw the header
  snprintf(lineBuf, SCREEN_W, "      PLAY");
  ui_print(0, 0, lineBuf, 6);
  n = SCREEN_H_1;
  for(y = 1; y < SCREEN_H; y++ ) {
    snprintf(lineBuf, SCREEN_W, "param %d : %f",
	     touchedParams[n].idx,
	     touchedParams[n].val );
    ui_print(y, 0, lineBuf, 1);
    n--;
  }
}


/////---------------------------
/////  line redraws

// draw line of ops page
static void draw_line_ops(S32 n, U16 num, U8 y, U8 hl) {
  if ( (n < num) && (n >= 0) ) { 
    snprintf(lineBuf, SCREEN_W, "   %d__%s",
             (int)n, net_op_name(n));
    ui_print(y, 0, lineBuf, hl + net_op_status(n));
  } else {
    // no selection
    snprintf(lineBuf, SCREEN_W, "   .");
    ui_print(y, 0, lineBuf, 0);
  }
  
}

// draw line of inputs page
static void draw_line_ins(S32 n, U16 num, U8 y, U8 hl) {
  S16 opIdx;
  if ( (n < num) && (n >= 0) ) { 
    opIdx = net_in_op_idx(n);
    if (opIdx >=0 ) {
      snprintf(lineBuf, SCREEN_W, "   %d_(%d)%s/%s_%f",
	       (int)n,
	       opIdx, 
	       net_op_name(net_in_op_idx(n)), 
	       net_in_name(n), 
	       net_get_in_value(n));
    } else {
      /// parameter
      snprintf(lineBuf, SCREEN_W, "   %d_P%d_%s_%f",
	       (int)n,
	       (int)net_param_idx(n),
	       net_in_name(n),
	       ////// FIXME:
	       //!!!!!  the param / input stuff is really messy
	       get_param_value(net_param_idx(n)) );
    }
    ui_print(y, 0, lineBuf, hl);

  } else {
    // no selection
    snprintf(lineBuf, SCREEN_W, "   .");
    ui_print(y, 0, lineBuf, 0);
  }

}

// draw line of outputs page
static void draw_line_outs(S32 n, U16 num, U8 y, U8 hl) {
  S16 target; U8 status;
  if ( (n < num) && (n >= 0) ) { 
    target = net_get_target(n);
    status = net_op_status(net_out_op_idx(n));
    if (target >= 0) {
      snprintf(lineBuf, SCREEN_W, "   %d_(%d)%s/%s-->%s/%s",
	       (int)n, net_out_op_idx(n),
	       net_op_name(net_out_op_idx(n)), net_out_name(n), 
	       net_op_name(net_in_op_idx(target)), net_in_name(target)
	       );
    } else {
      snprintf(lineBuf, SCREEN_W, "   %d_(%d)%s/%s",
	       (int)n, net_out_op_idx(n), net_op_name(net_out_op_idx(n)), net_out_name(n));
    }
    ui_print(y, 0, lineBuf, hl + status);
  } else {
    // no selection
    snprintf(lineBuf, SCREEN_W, "   .");
    ui_print(y, 0, lineBuf, 0);
  }
}

// draw line of scenes page
void draw_line_scenes(S32 n, U16 num, U8 y, U8 hl) {
  //  S16 target; U8 status;
  if ( (n < num) && (n >= 0) ) { 
    snprintf(lineBuf, SCREEN_W, "  %s  ", scene_name(n));
    ui_print(y, 0, lineBuf, hl);
  } else {
    // no selection
    snprintf(lineBuf, SCREEN_W, "   .");
    ui_print(y, 0, lineBuf, 0);
  }
}

