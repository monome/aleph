/* main.c
 * nullp
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "audio.h"
#include "ui.h"

#include "fix.h"
#include "fract32_emu.h"

//// test
//#include "env.h"


u8 run = 1;

///// test env
//env_asr* env;


int main(void) { 
  u16 ch;
  u16 i;

   init_audio();
   init_ui();

   //   sleep(1000);

   ////////
   // test env
/*
   env = (env_asr*) malloc(sizeof(env_asr));
   env_asr_init(          env);
   env_asr_set_atk_dur(   env, 40);

   env_asr_set_atk_shape( env, float_to_fr32(0.99));
   env_asr_set_rel_dur(   env, 100);
   env_asr_set_rel_shape( env, float_to_fr32(0.5));
   
   printf("\r\nenv atk/sus:\n");
   env_asr_set_gate(env, 1);
   for(i=0; i<50; i++) {
     printf("\r%f\n", fr32_to_float( env_asr_next(env) ) );
   }
   printf("\r\nenv release:\n");
   env_asr_set_gate(env,0);
   for(i=0; i<110; i++) {
     printf("\r%f\n", fr32_to_float( env_asr_next(env) ) );
   }
*/


   while(run) {
     ch = getchar();
     printf("%c", ch); 
     handle_key(ch);
     }

   deinit_audio();
   deinit_ui();
   return 0;
}
