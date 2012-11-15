/* ui.c
 * nullp
 */

// std
#include <stdio.h>
#include <stdlib.h>

//aleph
//#include "fix.h"
#include "module.h"
#include "types.h"
#include "ui.h"

/// variables

//static uiMode mode = eUiTop;
static char input[256];
static int paramIdx;

inline static void prompt(void) {
  printf(">");
  scanf("%s", input);
}

/// functions

void init_ui(void) {
}

void deinit_ui(void) {
}

void handle_key(u16 ch) {
  f32 pval;
  switch(ch) {
  case CMD_KEY_PARAM:
    
    printf("enter param index:\n");
    prompt();
    
    paramIdx = atoi(input);
    printf("enter param value (float): \n"); 
    prompt();
    pval = atof(input);
    
    module_set_param(paramIdx, pval);

    break;
  }
}
