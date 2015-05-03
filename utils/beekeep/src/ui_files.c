// std
#include <stdio.h>
#include <string.h>

//bees
#include "files.h"
#include "scene.h"

// beekeep
#include "dot.h"
#include "json.h"
#include "ui_files.h"

// working directory
char workingDir[64] = "";

// strip space from the end of a string
static void strip_space(char* str, u8 len) {
  u8 i;
  for( i=(len-1); i>0; i-- ) {
    if(str[i] == 0) { continue; }
    else if(str[i] == ' ') { str[i] = 0; }
    else { break; }
  }
}


// strip filename from the end of a path
void strip_filename(char* str, u8 len) {
  u8 i;
  for( i=(len-1); i>0; i-- ) {
    if(str[i] == 0) { continue; }
    else if(str[i] == '/') { 
		if(i<(len-1)) {
			str[i+1] = 0; 
		}
	}
    else { break; }
  }
}

// copy extension
void scan_ext(char* filename, char* ext) {
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


void write_scn(void) {
  char str[128];
  memset(str, '\0', 64);
  strcpy(str, workingDir);
  strcat( str, scene_get_name());
  strip_space(str, 64);
  strcat(str, ".scn");
  printf("\r\n writing .scn binary; name: %s", str);
  files_store_scene_name(str);
}

void write_json(void) {
  char str[128];
  memset(str, '\0', 64);
  strcpy(str, workingDir);
  strcat( str, scene_get_name());
  strip_space(str, 64);
  strcat(str, ".json");
  printf("\r\n writing %s...", str);
  net_write_json_native(str);
  printf("\r\n done.");
}

void write_gv(void) {
  char str[128];
  void* fp;
  memset(str, '\0', 64);
  strcpy(str, workingDir);
  strcat( str, scene_get_name());
  strip_space(str, 64);
  strcat(str, ".gv");
  fp = fopen(str, "w");

  net_write_dot(fp);

  fclose(fp);
}
