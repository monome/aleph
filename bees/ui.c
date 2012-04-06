#include <stdio.h>
#include <string.h>
#include "../common/types.h"
#include "net.h"
#include "ui.h"

//================ 
/// static vars

// enumerate UI state machine
typedef enum uiState {
  eMain,
  eCreateOp,
  eConnectOutput,
  eActivateInput,
} eUiState;

static eUiState state;

//=========================
// static function dec
static void print_op_types(void);
static void print_ops(void);
static void print_inputs(void);
static void create_op(void);

//==========================
// static funciton defs

static void print_op_types(void) {
  U8 i;
  for(i=0; i<numOpClasses; i++) {
    printf("%d: %s\n", i, op_registry[i].name);
  }
}

static void print_ops (void) {
  U8 i;
  for(i=0; i<net_num_ops(); i++) {
    printf("%d) %s\n", i, net_op_name(i));
  }
}

static void print_inputs(void) {
  U8 i;
  for(i=0; i<net_num_ins(); i++) {
    printf("%d) %s : %s\n", i, net_op_name(net_in_op_idx(i)), net_in_name(i));
  } 
}

static void print_outputs(void) {
  U8 i;
  for(i=0; i<net_num_outs(); i++) {
    printf("%d) %s : %s\n", i, net_op_name(net_out_op_idx(i)), net_out_name(i));
  } 
}

static void create_op(void) {
  char str[32];
  U8 dum;
  U8 type;
  S16 idx;
  printf("enter an operator type index, or (l) to list types\n");
  scanf("%s", &str);
  dum = getchar();
  if (strcmp(str, "l") == 0) {
    print_op_types();
    create_op(); // recurse
  } else {
    type = (U8)atoi(str);
    if(type < numOpClasses) {
      idx = net_add_op(type);
      if (idx >= 0) {
        printf("created a new operator of type %s at index %d\n", op_registry[type].name, idx);
      } else {
        printf("failed to create a new operator!\n");
      }
    } else {
      printf("invalid operator index\n");
    }
  }
}

/*
static void connect(void) {
  char str[32];
  U8 dum;
  U32 in, out;
  printf("enter an input index, or (l) to list inputs\n");
  scanf("%s", &str);
  dum = getchar();
  if (strcmp(str, "l") == 0) {
    print_inputs();
    connect(); // recurse
  } else {
    in = (U32)atoi(str);
    if (in < net_num_ins()) {
      
      printf("enter an output index, or (l) to list outputs\n");
      scanf("%s", &str);
      dum = getchar();
      
    } else {
      printf("invalid input index\n");
    }
  }
}
*/
static void connect(void) {
  char str[16];
  U8 dum;
  U32 in, out;
  
  print_inputs();
  printf("enter an input index\n");
  scanf("%s", &str);
  dum = getchar();

  in = (U32)atoi(str);
  if (in < net_num_ins()) {
    print_outputs();
    printf("enter an output index\n");
    scanf("%s", &str);
    dum = getchar();
    
    out = (U32)atoi(str);
    if (out < net_num_outs()) {
      printf("connecting input %d to output %d\n", in, out);
      net_connect(out, in);
    } else {
      printf("invalid output index\n");
    }
  } else {
    printf("invalid input index\n");
  }
}

 
static void activate(void) {  
  char str[16];
  U8 dum;
  U16 in;
  S32 val;
  
  print_inputs();
  printf("enter an input index\n");
  scanf("%s", &str);
  dum = getchar();
  in = (U16)atoi(str);
  
  printf("enter a value (32-bit signed)\n");
  scanf("%s", &str);
  dum = getchar();
  val = (S32)atoi(str);
  
  net_activate(in, &val);
  
}

//=========================
// external function defs

void ui_init(void) {
  state = eMain;
}

U8 ui_loop(void) {
  U8 cmd, dum;
  switch(state) {
    case eMain:
      printf("main menu: \n(l) list ops, (c) create op, (t) connect, (a) activate input, (q) quit\n");
      cmd = getchar();
      dum = getchar(); // newline
      if (cmd == 'l') {
        print_ops();
        return ui_loop(); // recurse
      }
      else if (cmd == 'c') {
        create_op();
        return ui_loop(); // recurse
      } 
      else if (cmd == 't') {
        connect();
        return ui_loop(); // recurse
      } 
      else if (cmd == 'a') {

        activate();
        return ui_loop(); // recurse
      } 
      else if (cmd == 'q') {
        return 0; // quit
      }
      break;
    default:
      return 0; // whoops, quit
  }
  return 0; // whoops, quit
}
