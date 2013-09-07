// asf
#include "print_funcs.h"

// avr32
#include "font.h"
#include "screen.h"

// lppr
#include "renderer.h"

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

// fill glyph buffers
void render_init(void) {
  //  u32 i;

  // render
  font_string("Q0", quadRegion[0], QUAD_SIZE, QUAD_W, 0xf, 0);
  

  /* u32 j; */
  /* u32 a, b; */

  /* for(i=0; i<QUAD_BUF_LEN; i++) { */
  /*   quadSwGlyph[1][i] = 0xff; */
  /*   quadSwGlyph[0][i] = 0x22; */
  /* } */

  /* for(i=0; i<QUAD_BUF_W; i++) { */
  /*   for(j=0; j<QUAD_BUF_W; j++) { */
  /*     if(i<QUAD_BUF_W__2) {  */
  /* 	a = i * 0xf / QUAD_BUF_W__2; */
  /*     } else { */
  /* 	a = (QUAD_BUF_W - i) * 0xf / QUAD_BUF_W__2; */
  /*     } */
  /*     if(i<QUAD_BUF_W__2) {  */
  /* 	b = j * 0xf / QUAD_BUF_H__2; */
  /*     } else { */
  /* 	b = (QUAD_BUF_H - j) * 0xf / QUAD_BUF_H__2; */
  /*     } */
  /*     //      testData1[j*QUAD_BUF_H + i] = a | b; */
  /*     //      testData2[j*QUAD_BUF_H + i] = a & b; */
  /*     //      testData1[j*QUAD_BUF_H + i] = i % 0xe + 1; */
  /*     //      testData2[j*QUAD_BUF_H + i] = j % 0xe + 1; */
  /*     //      testData1[j*QUAD_BUF_H + i] = 0xff; */
  /*     //      testData2[j*QUAD_BUF_H + i] = 0x22; */
  /*   } */
  /* } */
}

// update dirty regions
void render_update(void) {
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
