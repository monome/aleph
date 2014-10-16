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

#include "app.h"
#include "jansson.h"
#include "json.h"
#include "ui.h"


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

int main (int argc, char **argv)
{
  char filename[32];
  char ext[16];

  strcpy(filename, argv[1]);
  scan_ext(filename, ext);

  app_init();
  app_launch(1);

  gtk_init (&argc, &argv);

  ui_init();

  gtk_main ();
}

