/* redraw.c
 * bees
 * aleph
 *
 * page-specific redraw functinos.
 */

// undefine to use fixed-point output formatting
//#define SNPRINTF

#include <string.h>
//#include "print_fix.h"
#include "fix.h"
#include "param.h"
#include "preset.h"
#include "redraw.h"
#include "scene.h"
#include "screen.h"
#include "types.h"

//------------------------
//----- static variables

// line buffer
static char lineBuf[SCREEN_W];
static char *pline = lineBuf;
static char * const pLineEnd = lineBuf + SCREEN_W;

//---------------------------------
//---- static functions

// write to top of line buffer
static inline void println(const char* str);
// append to line buffer
static inline void appendln(const char* str);
// write int to top of line buffer
static inline void println_int(int x);
// append int to line buffer
static inline void appendln_int(int x, int len);
// append char to line buffer
static inline void appendln_char(char c);

//// line redraws
// op
static void draw_line_ops(s32 n, u16 num, u8 y, u8 hl);
// in
static void draw_line_ins(s32 n, u16 num, u8 y, u8 hl);
// out
static void draw_line_outs(s32 n, u16 num, u8 y, u8 hl);
// presets
static void draw_line_presets(s32 n, u16 num, u8 y, u8 hl);
// scenes
static void draw_line_scenes(s32 n, u16 num, u8 y, u8 hl);
// dsp
static void draw_line_dsp(s32 n, u16 num, u8 y, u8 hl);

//==================================================
//==== redraw ops page
void redraw_ops(void) {
  u8 y = 0;                       // which line
  s32 n, nCenter;         // which list entry
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
    draw_line_ops(--n, num, --y, 1);
  }
  
  // re-center
  n = nCenter;
  y = SCREEN_ROW_CENTER;
  
  // print higher entries
  while (y < SCREEN_H_2) {
    draw_line_ops(++n, num, ++y, 1);
  }
      
  // draw footer 
  // (new op type)
#ifdef SNPRINTF
  snprintf(lineBuf, SCREEN_W, " + %s",
	   op_registry[userOpTypes[newOpType]].name);
#else
  println(" + ");
  appendln(op_registry[userOpTypes[newOpType]].name);
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
  screen_line(0, 0, "PARAMS", 3);

  // draw footer 
  // (function labels)
  screen_line(0, kScreenLines[CHAR_ROWS - 1], "GATHER DISCONNECT STORE  PRESET ", 3);
}

//==================================================
//==== redraw outputs page
void redraw_outs(void) {
  u8 y = 0;                       // which line
  s32 n, nCenter;         // which list entry
  //s16 target;
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

  // header
  screen_line(0, 0, "ROUTING", 3);
  // (footer)
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
  screen_line(0, 0, "PRESETS", 3);

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

  screen_line(0, 0, "SCENES", 3);

  // draw footer 
  // (function labels)
  screen_line(0, kScreenLines[CHAR_ROWS - 1], "CLEAR COPY STORE RECALL", 3);
}



//==================================================
//==== redraw dsp page
void redraw_dsp(void) {
  u8 y = 0;                       // which line
  s32 n, nCenter;         // which list entry
  u16 num;
  
  //  num = files_get_num_dsp();
  num = 16; // whatever just compile

  nCenter = page->selected;
  if (nCenter >= num) {
    nCenter = num;
  }
  n = nCenter;
  // print selection at center
  y = SCREEN_ROW_CENTER;
  
  // print lower entries
  while (y > 1) {
    n--;
    y--;
    draw_line_dsp(n, num, y, 1);
  }
  
  // re-center
  n = nCenter;
  y = SCREEN_ROW_CENTER;
  
  // print higher entries
  while (y < SCREEN_H_2) {
    n++;
    y++;
    draw_line_dsp(n, num, y, 1);
  }

  screen_line(0, 0, "DSP", 3);

  // draw footer 
  // (function labels)
  screen_line(0, kScreenLines[CHAR_ROWS - 1], "LOAD DEFAULT", 3);
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
    println("p");
    appendln_int(touchedParams[n].idx, 3);
#endif
    screen_line(0, kScreenLines[y], lineBuf, 1);
    n--;
  }
// draw the header
  screen_line(0, 0, "PLAY", 6);
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
  println_int(n);
  appendln(".");
  appendln(net_op_name(n));
#endif
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
      println_int(opIdx);
      appendln_char(pch);
      appendln( net_op_name(net_in_op_idx(n)) );
      appendln_char('/');
      appendln( net_in_name(n) );
      appendln_char('_');
      print_fix16(pline, net_get_in_value(n) );
#endif
    } else {
      /// parameter
#ifdef SNPRINTF
      snprintf(lineBuf, SCREEN_W, "p%d%c%s_%f",
	       //	       (int)n,
	       (int)net_param_idx(n),
	       pch,
	       net_in_name(n),
	       get_param_value(net_param_idx(n)) );
#else
      println("p");
      appendln_int( (int)net_param_idx(n), 4);
      appendln_char(pch);
      appendln( net_in_name(n) );
      print_fix16(pline, net_get_in_value(n) );
#endif
    }
    screen_line(0, kScreenLines[y], lineBuf, hl);
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
    println_int( net_out_op_idx(n) );
    appendln_char(pch);
    appendln( net_op_name(net_out_op_idx(n)) );
    appendln_char('/');
    appendln( net_out_name(n) );
    appendln("->");
    appendln_int( net_in_op_idx(target), 4);
    appendln_char('.');
    appendln( net_op_name(net_in_op_idx(target)) );
    appendln_char('/');
    appendln( net_in_name(target) );
#endif
  } else {
#ifdef SNPRINTF
    snprintf(lineBuf, SCREEN_W, "%d%c%s/%s",
	     net_out_op_idx(n),
	     pch,
	     net_op_name(net_out_op_idx(n)),
	     net_out_name(n) );
#else
    println_int( net_out_op_idx(n) );
    appendln_char(pch);
    appendln( net_op_name(net_out_op_idx(n)) );
    appendln_char('/');
    appendln( net_out_name(n) );
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
  println_int((int)n);
  appendln_char('.');
  appendln(preset_name(n));
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
#endif
  screen_line(0, kScreenLines[y], lineBuf, hl);
}

// draw line of dsp page
void draw_line_dsp(s32 n, u16 num, u8 y, u8 hl) {
}


///// snprintf replacement
// write to top of line buffer
static inline void println(const char* str) {
  pline = lineBuf;
  appendln(str);
}

// append to line buffer
static inline void appendln(const char* str) {
  while(*str != 0) {
    *pline++ = *str++;
  }
}

// write int to top of line buffer
static inline void println_int(int x) {
  pline = lineBuf;
  appendln_int(x, 6);
}

// append int to line buffer (fill to end)
static inline void appendln_int(int x, int len) {
  // bounds check :
  int rem = (u32)pLineEnd - (u32)pline;
  if (len > rem) len = rem;
  itoa_whole(x, pline, len); 
  pline += len; 
}

// append char to line buffer
static inline void appendln_char(char c) {
}

