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

// main function
int main(void) {  
  U32 i, x, y;
  U8 alpha;

  init_avr();
  init_oled();

  //  register_interrupts();
  x = 0;
  y = 0;
  alpha = 1;
  for(i=0; i<(CHAR_ROWS * CHAR_COLS); i++) {
    oled_draw_char(x, y, i, alpha);
    x += FONT_CHARW;
    if (x >= NCOLS) {
      x = 0;
      y += FONT_CHARH;
    }
    alpha++;
    if(alpha > 0x0f) {
      alpha = 1;
    } 
  }
  oled_refresh();
  return 0;
}

