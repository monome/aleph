/*
  main.c
  beekeep

  a bees simulator, heavily stripped. 
  its first job is to parse scene files and convert them to JSON/something.
  
*/

#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include "getopt.h"

#include "files.h"
#include "net_protected.h"

#include "app.h"
#include "dot.h"
#include "json.h"
#include "ui.h"
#include "ui_files.h"

int main (int argc, char **argv)
{
  char path[64];
  char ext[16];
  void* fp;
  bool arg = 0;

  setbuf(stdout, NULL);

  if(argc < 2) {
    // start with blank scene, use working directory
  } else {
    arg = 1;
  }
  
  if(arg) {
    strcpy(path, argv[1]);
    scan_ext(path, ext);
  }

  app_init();
  app_launch(1);

  if(arg) {
    if(strcmp(ext, ".scn") == 0) {
      files_load_scene_name(path);
    }
    else if(strcmp(ext, ".json") == 0) {
      net_read_json_native(path);
    }
	strip_filename(path, 64);
	strcpy(workingDir, path);
  }
  
  printf("\r\n working directory now: %s", workingDir);

  gtk_init (&argc, &argv);
  ui_init();
  gtk_main ();
}
