/* ui.c
 * nullp
 */

// std
#include <stdio.h>
#include <stdlib.h>

//aleph
#include "fix.h"
#include "param_common.h"
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
  pval pv;
  u32 pnum;
  u32 i;
  ParamDesc desc;
  switch(ch) {
  case CMD_KEY_PARAM:

    pnum = module_get_num_params();
    for(i=0; i<pnum; i++) {
      printf("\rparam %d: %s\n", i, moduleData->paramDesc[i].label);
    }
    printf("enter param index:\n");
    prompt();
    paramIdx = atoi(input);
    printf("\rsetting parameter %d, %s\n", paramIdx, moduleData->paramDesc[paramIdx].label);
    printf("enter param value (float): \n"); 
    prompt();
    //    pv.fix = atof(input);
    pv.fix = fix16_from_float(atof(input));
    module_set_param(paramIdx, pv);

    break;
  default:
    break;
  }
}
