/* main.c
 * nullp
 *
 */

#include <stdio.h>

#include "audio.h"
#include "ui.h"

u8 run = 1;

int main(void) { 
   u16 ch;
  
   init_audio();
   init_ui();

   //   sleep(1000);

   while(run) {
     ch = getchar();
     //     printf("%c", ch); 
     handle_key(ch);
     }

   deinit_audio();
   deinit_ui();
   return 0;
}
