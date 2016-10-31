/* 
anti-aliased fonts (bitmaps)
*/

#include "font.h"
#include "font_aa.h"
#include "fonts/ume_tgo5_18.h"


#define FONT_AA font_ume_tgo5_18
#define FONT_AA_CHARW 	FONT_UME_TGO5_18_W
#define FONT_AA_CHARH 	FONT_UME_TGO5_18_H

// render an anti-aliased (4-bit) glyph to a buffer
// arguments are character, buffer, target row size, invert flag
extern u8* font_glyph_aa(char ch, u8* buf, u8 w, u8 inv) {
  //#if 0
  const char* gl; // glyph data
  //  u8 gw, gh; // glyph width and height
  u8 i, j;
  // how many bytes to move buf pointer for 1st column of next row
  u32 nextRowBytes;
    // buf pointer;
  char* p;
  // pointer to copy function
  //  void (*fp)(const char* src, char* dst);
  
  // fixme: wtf why
  //  if(inv) { fp = &copyPxInv; } else { fp = &copyPx; }

#if 0
  /// FIXME: might want only numerals
  //// hackish
  if( (ch > 45) && (ch < 58)) { // dot + numerals
    gl = FONT_AA[ch - 46].glyph.data;
    // print_dbg("\r\n printing numeral ");
    // print_dbg_char(ch);
    // print_dbg(" , glyphidx: ");
    // print_dbg_ulong(ch - 46);
  } else { 
    return buf;
  }
#else
  //  gl = FONT_AA[ch - FONT_ASCII_OFFSET].glyph.data;
  /// flaarggg
  //  print_dbg("\r\n render glyph: ");
  //  print_dbg_char(ch);
  //  print_dbg(" ( 0x");
  //  print_dbg_hex((u32)ch);
  
  ////////
  /// FIXME: font is missing ` or _ or something
  if(ch > 95) { ch--; }
  ////////
  gl = FONT_AA[ch - FONT_ASCII_OFFSET].glyph.data;
#endif
  /// copy glyph to buffer...
  p = (char*)buf;
  nextRowBytes = w - FONT_AA_CHARW;
  //  print_dbg("\r\n");
  //  print_dbg("\r\n");

  if(inv) {
    // loop over rows
    for(i=0; i<FONT_AA_CHARH; i++) {
      // loop over columns
      for(j=0; j<FONT_AA_CHARW; j++) {
	//	print_dbg_char(*gl + 0x20);
	//      print_dbg(dbgStr[(u32)(*gl)]);
	//	print_dbg(" ");
	// copy/invert pixel and advance pointers
	*p++ = 0xf - *gl++;
      }
      //      print_dbg("\r\n");
      // reset column and advance row
      p += nextRowBytes;
    }
    // return original buf pointer plus glyph width
  } else {
    // loop over rows
    for(i=0; i<FONT_AA_CHARH; i++) {
      // loop over columns
      for(j=0; j<FONT_AA_CHARW; j++) {
	//	print_dbg_char(*gl + 0x20);
	//      print_dbg(dbgStr[(u32)(*gl)]);
	//	print_dbg(" ");
	// copy pixel and advance pointers
	*p++ = *gl++;
      }
      //      print_dbg("\r\n");
      // reset column and advance row
      p += nextRowBytes;
    }
  }
  // return original buf pointer plus glyph width
  return buf + FONT_AA_CHARW;
  //#endif
}

// render a string of packed glyphs to a buffer
extern u8* font_string_aa(const char* str, u8* buf, u32 size, u8 w, u8 inv) {
  u8* max = buf + size;
  // print_dbg("\r\n antialiased string \"");
  // print_dbg(str);
  // print_dbg("\" font w, h: (");
  // print_dbg_ulong(FONT_AA_CHARW);
  // print_dbg(" , ");
  // print_dbg_ulong(FONT_AA_CHARH);
  // print_dbg(" ) , buf w: ");
  // print_dbg_ulong(w);

  /// FIXME: this bounds check looks wrong
  while (buf < max) {
    if (*str == 0) {
      // end of string
      break;
    }
    buf = font_glyph_aa(*str, buf, w, inv);
    // 1-column space between chars
    //    buf++;
    //// cutting the fonts with space included
    //// so that background doesn't bleed through here
    str++;
  }
  return buf;  
}
