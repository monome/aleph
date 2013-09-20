/*
  aleph bitmap fonts.

  stage 2 of the process of generating a bitmapped antialised font for the aleph screen.
  
  in it, .xmp output from GIMP is scanned, and appropriate include files are generated for xpm_convert.c

 */


#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// path where gimp output lives
static const char dirpath[] = "/home/emb/aleph/bmp/xpm";
// path to generated headers
static const char outpath[] = "/home/emb/aleph/bmp/inc"; 


void main(void) {
  // output file for list of #includes
  FILE * f_headers;
  // output file for glyph names
  FILE * f_names;
  // output file for glyph data initialization
  FILE * f_array;

  // directory pointer
  DIR * dirp;
  // list of dir entry pointers
  struct dirent ** entries;
  // current entry
  struct dirent* entry;
  // number of entries
  int n = scandir(dirpath, &entries, 0, alphasort);
  // length of filename
  int len = 0;
 // tmp
  char buf[64];
  int i, j;
  
  strcpy(buf, outpath);
  strcat(buf, "/aleph_font_headers.inc");
  printf("\r\n output file path: %s\r\n", buf);
  f_headers = fopen( buf, "w");

  strcpy(buf, outpath);
  strcat(buf, "/aleph_font_names.inc");
  printf("\r\n output file path: %s\r\n", buf);
  f_names = fopen( buf, "w");

  strcpy(buf, outpath);
  strcat(buf, "/aleph_font_array.inc");
  printf("\r\n output file path: %s\r\n", buf);
  f_array = fopen( buf, "w");

  //  dirp = opendir(dirpath);

  //  while( (entry = readdir(dirp)) != NULL) {
  for(j=0; j<n; j++) {
    entry = entries[j];
    len = strlen(entry->d_name);
    if(  len > 2) {
      printf("%s\r\n", entry->d_name);
      strcpy(buf, entry->d_name);      
      // replace special characters
      for(i=0; i<len; i++) {
	if(buf[i] == ' ') { buf[i] = '_'; }
	if(buf[i] == '/') { buf[i] = '_'; }
	if(buf[i] == '.') { buf[i] = '_'; }
	if(buf[i] == ',') { buf[i] = '_'; }
	if(buf[i] == '-') { buf[i] = '_'; }
	if(buf[i] == '+') { buf[i] = '_'; }
	if(buf[i] == '=') { buf[i] = '_'; }
      }      
      fprintf( f_headers, "#include \"%s\"\r\n", entry->d_name);
      fprintf( f_array, "&(%s), \r\n", buf);
      fprintf( f_names, "\"%s\", \r\n", buf);
    }
    free(entry);
  }
  free(entries);
  //  closedir(dirp);

  fclose(f_headers); 
  fclose(f_array); 
  fclose(f_names); 
}

