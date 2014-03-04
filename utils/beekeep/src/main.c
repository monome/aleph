/*
  main.c
  beekeep

  a bees simulator, heavily stripped. 
  its first job is to parse scene files and convert them to JSON/something.
  
 */

#include <stdio.h>
#include <string.h>

// jannson is a JSON manipulation library
#include "jansson.h"

#include "app.h"
#include "json.h"

static char filename[32];
static char ext[16];

void scan_ext(void) {
  int len;
  int i;
  int dotpos = -1;

  len = strlen(filename);
  // search for '.' backwards from end
  i = len;
  while(i > 0) {
    --i;
    if(filename[i] == '.') {
      dotpos = i;
      break;
    }
  }
  if(dotpos >= 0) {
    strncpy(ext, filename + dotpos, len - i + 1);
  }

}

int main(int argc, char* argv[]) {

  if(argc < 2) { 
    print_dbg("\r\n need filename arg \r\n");
    return 1;
  }

  strcpy(filename, argv[1]);
  scan_ext();

  printf("\r\n file extension: %s", ext);

  app_init();
  app_launch(1);

  if(strcmp(ext, ".scn") == 0) {
    printf(" \r\n file type is .scn, converting to .json \r\n");
    files_load_scene_name(filename);
    net_print();
    strcat(filename, ".json");
    //    net_write_json_raw(filename);
    net_write_json_native(filename);
    return 0;
  }

  if(strcmp(ext, ".json") == 0) {
    printf(" \r\n file type is .json, converting to .scn \r\n");
    //    net_read_json_raw(filename);
    net_read_json_native(filename);
    strcat(filename, ".scn");
    files_store_scene_name(filename);
    return 0;
  }
  
  printf(" unrecognized file type, exiting \r\n");
  return 1;

}
