/*
  main.c
  beekeep

  a bees simulator, heeavily stripped. 
  its first job is to parse scene files and convert them to JSON/something.
  
 */

#include <stdio.h>
#include <string.h>

#include "app.h"



int main(int argc, char* argv[]) {
  // argument: filename
  char sceneName[32];

  if(argc < 2) { 
    print_dbg("\r\n need filename arg \r\n");
    return 1;
  }

  strcpy(sceneName, argv[1]);

  app_init();

  app_launch(0);

  files_load_scene_name(sceneName);

  net_print();

  printf("\r\n\r\n");}
