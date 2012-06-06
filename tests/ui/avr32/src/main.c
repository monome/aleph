/* main.c
 * 
 * avr32 interface board test
 */

#include <avr32/io.h>
#include "compiler.h"
#include "gpio.h"
//#include "intc.h"

#include "config.h"
#include "init.h"
#include "oled.h"

// test
#include "board.h"
#include "power_clocks_lib.h"
#include "usart.h"
#include "delay.h"


//-----------------------------
//---- static variables
//static U8 alpha = 0;

// main function
int main(void) {  
  //U32 i, j, x, y;

  init_avr();
  init_oled();
  //  register_interrupts();
  
  // checkerboard, hopefully
  //... ok!
  /*
  for (i=0; i<NROWS_2; i++) {
    for (j=0; j<NCOLS_2; j++) {
      data = 0x0f & alpha;
      write_data(data);
      delay_ms(10);
    }
    for (j=0; j<NCOLS_2; j++) {
      data = (alpha << 4) & 0xf0;
      write_data(data);
      delay_ms(10);
    }
    alpha++;
    alpha &= 0x0f;
  }
  */
  
  // try with drawing functions...
  // sort of...
  /*
  for (i=0; i<NROWS_2; i++) {
      y = i*2;
    for (j=0; j<NCOLS_2; j++) {
      x = j*2;
      oled_draw_pixel(x, y, alpha);
      x++;
      oled_draw_pixel(x, y, 0);
    }
    y++;
    for (j=0; j<NCOLS_2; j++) {
      x = j * 2;
      oled_draw_pixel(x, y, 0);
      x++;
      oled_draw_pixel(x, y, alpha);
    }
    alpha++;
    alpha &= 0x0f;
  }
  oled_refresh();
*/

  // simpler...
  /*
 for (i=0; i<NROWS; i++) {
   for (j=0; j<NCOLS; j++) {
     oled_draw_pixel(j, i, alpha);
   }
    alpha++;
    if (alpha > 0x0f) alpha = 0;

  }
  oled_refresh();
  */

  // something... 
  

  /*  
  for (i=0; i<NROWS; i++) {
    for (j=0; j<NCOLS; j++) {
      if ((j < NCOLS_2) && (i < NROWS_2)) {
	alpha = 0x0f;
      } else {
	alpha = 0x02;
      }
      oled_draw_pixel(j, i, alpha);
    }
  }
  oled_refresh();
  */
  
  
  //  oled_draw_string(0, 0, "ABCDEFGH//12345678", 0x0f);
  
  oled_draw_char(0, 0, *("W"), 0x04);
  oled_draw_char(8, 8, *("T"), 0x07);
  oled_draw_char(16, 16, *("F"), 0x0c);
  oled_draw_char(32, 32, *("!"), 0x0f);
  
  oled_refresh();
  
  delay_ms(1000);
  oled_draw_string(64, 48, "($^&%*#$", 0x0a);
  oled_draw_string(64, 56, "YYEEEAHHHH....", 0x04);

  oled_refresh();


  /*
  // try with raw drawing function
  for(i=0; i<GRAM_PIXELS; i++) {
    if(i % 2) {

    }
  }
  */

  return 0;
}

