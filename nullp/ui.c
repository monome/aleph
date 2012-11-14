/* ui.c
 * nullp
 */

#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "module.h"
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
  f32 paramVal;
  switch(ch) {
  case CMD_KEY_PARAM:
    
    printf("enter param index:\n");
    prompt();
    
    paramIdx = atoi(input);
    printf("enter param value (float): \n"); 
    prompt();
    paramVal = atof(input);
    
    module_set_param(paramIdx, paramVal);

    break;
  }
}
