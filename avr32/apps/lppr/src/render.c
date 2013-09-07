// asf
#include "print_funcs.h"

// avr32
#include "font.h"
#include "screen.h"

// lppr
#include "render.h"

/*
  screen is `128x64 px
  horizontal byte count is pix/2 = 64
 */
/* #define QUAD_BUF_W 	32 // 2px = 1 byte */
/* #define QUAD_BUF_W__2 	1 */
/* #define QUAD_BUF_H 	32 */
/* #define QUAD_BUF_H__2	16 */
/* #define QUAD_BUF_LEN 	1024 */

//------------------------
//----  region buffers

// footer is 4 rectangles, single-character height : 8px * (128 / 4)
#define FOOT_W 		32
#define FOOT_H 		8
#define FOOT_SIZE 	128
static u8 footRegion[4][FOOT_SIZE];

// 4 quads divide the rest of the space. 
// 128 / 2 * ((64 - CHAR_H) / 2)
#define QUAD_W 		64
#define QUAD_H 		28
#define QUAD_SIZE 	1792
static u8 quadRegion[4][QUAD_SIZE];

// coordinates
static const u32 xQuad0 = 0;
static const u32 yQuad0 = 0;
static const u32 xQuad1 = 64;
static const u32 yQuad1 = 0;
static const u32 xQuad2 = 0;
static const u32 yQuad2 = 28;
static const u32 xQuad3 = 64;
static const u32 yQuad3 = 28;

static const u32 xFoot0 = 0;
static const u32 yFoot0 = 56;
static const u32 xFoot1 = 32;
static const u32 yFoot1 = 56;
static const u32 xFoot2 = 64;
static const u32 yFoot2 = 56;
static const u32 xFoot3 = 96;
static const u32 yFoot3 = 56;


// region-dirty flags
static u8 dirtyQuad0 = 0;
static u8 dirtyQuad1 = 0;
static u8 dirtyQuad2 = 0;
static u8 dirtyQuad3 = 0;

static u8 dirtyFoot0 = 0;
static u8 dirtyFoot1 = 0;
static u8 dirtyFoot2 = 0;
static u8 dirtyFoot3 = 0;

// fill glyph buffers
void render_init(void) {
  u32 i;
  // render
  font_string("Q1", quadRegion[0], QUAD_SIZE, QUAD_W, 0xf, 0);
  font_string("Q2", quadRegion[1], QUAD_SIZE, QUAD_W, 0xf, 0);
  font_string("Q3", quadRegion[2], QUAD_SIZE, QUAD_W, 0xf, 0);
  font_string("Q4", quadRegion[3], QUAD_SIZE, QUAD_W, 0xf, 0);

  dirtyQuad0 = 1;
  dirtyQuad1 = 1;
  dirtyQuad2 = 1;
  dirtyQuad3 = 1;

  /// debug: print buffer contents
  print_dbg("\r\n quad 0: ");
  for(i=0; i<QUAD_SIZE; i++) {
    if( (i % QUAD_W) == 0) {
      print_dbg("\r\n");
    }
    if(quadRegion[0][i] > 0x5) {
      print_dbg("#"); 
    } else {
      print_dbg("_");
    }
  }

}

// update dirty regions
void render_update(void) {
  if(dirtyQuad0) {
    screen_draw_region(xQuad0, yQuad0, QUAD_W, QUAD_H, quadRegion[0]);
    dirtyQuad0 = 0;
  }
  if(dirtyQuad1) {
    screen_draw_region(xQuad1, yQuad1, QUAD_W, QUAD_H, quadRegion[1]);
    dirtyQuad1 = 0;
  }
  if(dirtyQuad2) {
    screen_draw_region(xQuad2, yQuad2, QUAD_W, QUAD_H, quadRegion[2]);
    dirtyQuad2 = 0;
  }
  if(dirtyQuad3) {
    screen_draw_region(xQuad3, yQuad3, QUAD_W, QUAD_H, quadRegion[3]);
    dirtyQuad3 = 0;
  }
}

// force refresh
void render_refresh(void) {
  dirtyQuad0 = 1;
  dirtyQuad1 = 1;
  dirtyQuad2 = 1;
  dirtyQuad3 = 1;
  render_update();
}

void render_sw_on(u8 sw, u8 on) {
  //render_quad(sw, quadSwGlyph[on]);
}

// draw a test quadrant buffer
void render_quad(u8 quad, u8* data) {
  //  u8 x, y;

  /* x = ((quad & 1)>0) ? QUAD_BUF_W : 0; */
  /* y = ((quad & 2)>0) ? QUAD_BUF_H : 0; */

  /* /\* print_dbg("\r\n render quad, x: "); *\/ */
  /* /\* print_dbg_ulong(x); *\/ */
  /* /\* print_dbg(" , y: "); *\/ */
  /* /\* print_dbg_ulong(y); *\/ */

  /* screen_draw_region(x, y, QUAD_BUF_W, QUAD_BUF_H, data ); */

}


// render milliseconds
void render_ms(u8 x, u8 y, u32 ms) {
}

// render text
void render_txt(u8 x, u8 y, char* txt) {
}

// render fixed-point number
void render_fix(u8 x, u8 y, fix16 val) {
}

#undef LINE_BUF_LEN
