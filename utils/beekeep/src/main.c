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
  char filename[32];
  char ext[16];
  void* fp;

  setbuf(stdout, NULL);

  if(argc < 2) {
    printf("\r\n beekeep: filename argument required; exiting\r\n\r\n");
    return 1;
  }

  strcpy(filename, argv[1]);
  scan_ext(filename, ext);

  app_init();
  app_launch(1);

  if(strcmp(ext, ".scn") == 0) {
    files_load_scene_name(filename);
  }
  else if(strcmp(ext, ".json") == 0) {
    net_read_json_native(filename);
  }

  gtk_init (&argc, &argv);
  ui_init();
  gtk_main ();
}
