/*
  tool to convert GIMP image-include format to 4-bit grayscale format.

   stage 3 of the process of generating a bitmapped antialised font for the aleph screen.

   .xpm files and and intermediate includes are assembled and processed.

   a single final source file is generated, suitable for avr32 compilation.

 */

#include <string.h>
#include <stdio.h>

// include XPM files
//#include "aleph_hebrew.xpm"
 
#include "inc/aleph_font_headers.inc"

// array of pointers to arrays of strings (!)
// GIMP names each struct according to filename
static char*(*glyphs[])[] = {
  //  &(aleph_hebrew_xpm),
#include "inc/aleph_font_array.inc"

};

// array of glyph names
static char* names[] = {
  //  "aleph_hebrew",
#include "inc/aleph_font_names.inc"
};



/////
/////// ugh, must set this manually for now...
static const int nglyphs = 11;
////
///


static FILE* fregion;
// TODO:
// static FILE* fscreen;

// process a glyph
static void process_glyph(const char* name, char* (*glyph)[]) {
  int w, h, d; // width, height, depth, count
  char colors[16]; // color codes
  char val;
  int i, j, k;  
  char* p;

  i = 0;
  // first string : dimensions
  // width
  w = strtol((*glyph)[i], &p, 10);
  // height
  h = strtol(p, &p, 10);
  // color depth
  d  = strtol(p, NULL, 10);

  printf("\r\n name: %s, w: %d, h: %d, d: %d \r\n", name, w, h, d);

  if(d != 16) {
    printf("\r\n warning: unexpected color depth");
  }
  i++;
  // next N strings: color map
  for(i=0; i<16; i++) {
    colors[i] = (*glyph)[i+1][0];
  }

  // write file output
  fprintf(fregion, "\r\n const unsigned char %s_glyph[] = { \r\n", name);
  // width and height
  fprintf(fregion, "%d , %d, ", w, h);
  // next N strings: data
  for(j=0; j<h; j++) {
    p = (*glyph)[d + 1 + j];
    for(i=0; i<w; i++) {
      for(k=0; k<d; k++) {
	if(*p == colors[k]) {
	  val = k;
	  break;
	}
      }
      fprintf(fregion, "0x%02X , ", val);
      /// TODO: screen format
      printf("0x%02X ", val);
      p++;
    }
  }
    fprintf(fregion, "\r\n\r\n };");
    printf("\r\n");
}

// main
int main ( int argc, char *argv[] ) {
  int i;

  //  nglyphs = sizeof(names);

  // image files as regions (1byte == 1pix)
  fregion = fopen("aleph_bmp_region.c", "w");
  fprintf(fregion, "/* output from xpm-convert.c */ \r\n");
  fprintf(fregion, "/* first two bytes are width and height */ \r\n\r\n");
  fprintf(fregion, "/* data follows */ \r\n\r\n");
  
  // image files as raw screen daa (1byte == 2px, inverted)
  // TODO
  //   fscreen = fopen("aleph_bmp_screen.c", "w");

  for(i=0; i<nglyphs; i++) {
    process_glyph(names[i], glyphs[i]);
    fprintf(fregion, "\r\n\r\n");
  }

  //  fclose(fscreen);
  fclose(fregion);


}

