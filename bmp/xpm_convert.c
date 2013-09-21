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
static const int nglyphs = 92;
////
///


static FILE* fregion;
// TODO:
// static FILE* fscreen;

// output colors
static const unsigned char color16[16] = {
  0x00, 0x11, 0x22, 0x33,
  0x44, 0x55, 0x66, 0x77,
  0x88, 0x99, 0xaa, 0xbb,
  0xcc, 0xdd, 0xee, 0xff
};

// process a glyph
static void process_glyph(const char* name, char* (*glyph)[]) {
  int w, h, d; // width, height, depth, count
  unsigned char colorCode[16]; // color codes
  unsigned char colorIn[16]; // input color map
  unsigned char colorOut[16]; // output color map
  //  char colors[16];
  unsigned char val;
  int i, j, k;  
  char* p;
  unsigned char matchcol = 0;
  int dif, mindif;
  char buf[2];
  
  i = 0;
  // first string : dimensions
  // width
  w = strtol((*glyph)[i], &p, 10);
  // height
  h = strtol(p, &p, 10);
  // color depth
  d  = strtol(p, NULL, 10);

  printf("\r\n name: %s, w: %d, h: %d, d: %d \r\n", name, w, h, d);


  i++;
  // next N strings: color map
  for(i=0; i<16; i++) {
    printf("\r\n %s ", (*glyph)[i+1]);
    colorCode[i] = (*glyph)[i+1][0];
    // nastily hack the last 2 chars out of color string and convert to uint
    p = (*glyph)[i+1];
    p = p + strlen(p) - 2;
    printf(" .. color string (?) : %s", p);
    //    colorIn[i] = strtoul(p + strlen(p) - 2, NULL, 0xf);
    /// this will only work if image was index with our cusom palette!
    if(strcmp(p, "00")==0) { colorIn[i] = 0x00; }
    if(strcmp(p, "11")==0) { colorIn[i] = 0x11; }
    if(strcmp(p, "22")==0) { colorIn[i] = 0x22; }
    if(strcmp(p, "33")==0) { colorIn[i] = 0x33; }
    if(strcmp(p, "44")==0) { colorIn[i] = 0x44; }
    if(strcmp(p, "55")==0) { colorIn[i] = 0x55; }
    if(strcmp(p, "66")==0) { colorIn[i] = 0x66; }
    if(strcmp(p, "77")==0) { colorIn[i] = 0x77; }
    if(strcmp(p, "88")==0) { colorIn[i] = 0x88; }
    if(strcmp(p, "99")==0) { colorIn[i] = 0x99; }
    if(strcmp(p, "AA")==0) { colorIn[i] = 0xaa; }
    if(strcmp(p, "BB")==0) { colorIn[i] = 0xbb; }
    if(strcmp(p, "CC")==0) { colorIn[i] = 0xcc; }
    if(strcmp(p, "DD")==0) { colorIn[i] = 0xdd; }
    if(strcmp(p, "EE")==0) { colorIn[i] = 0xee; }
    if(strcmp(p, "FF")==0) { colorIn[i] = 0xff; }
    printf("   ... input color: 0x%02X", colorIn[i]);
    /// a nasty hack, grabbing last byte in string
    //    colorIn[i] = (*glyph)[i+1][ strlen((*glyph)[i+1]) -1];
  }

  //  if(d != 16) {
    //    for(j=0; j<16; j++) {
    //      colorOut[j] = j;
    //    }
    //  } else {
    //    printf("\r\n warning: unexpected color depth ! \r\n");    
    //  }
    /// arg fuck have to actually search for matching colors... what bullshit!
  for(j=0; j<d; j++) {
    // for each color in the input space...
    // search against all colors in the output space
    mindif = 1000;
    matchcol = 0x00;
    for(k=0; k<16; k++) {
      dif = (int)(color16[k]) - (int)(colorIn[j]);
      if(dif < 0) { dif *= -1; }
      if(dif < mindif) {
	mindif = dif;
	matchcol = color16[k];
      }
    }
    colorOut[j] = matchcol >> 4;
    printf("\r\n matched target %02X to colorspace %02X", colorIn[j], matchcol);
  }
  


  // write file output
  //  fprintf(fregion, "\r\n const unsigned char %s_glyph[] = { \r\n", name);
  fprintf(fregion, "\r\n // %s: \r\n", name);
  fprintf(fregion, "{ .raw = { \r\n ");
  // width and height
  fprintf(fregion, "%d , %d, \r\n", w, h);
  // next N strings: data
  for(j=0; j<h; j++) {
    p = (*glyph)[d + 1 + j];
    for(i=0; i<w; i++) {
      for(k=0; k<d; k++) {
	if(*p == colorCode[k]) {
	  val = colorOut[k];
	  //	  val = k;
	  //	  val = ((*p)>>4) & 0xf;
	  break;
	}
      }
      fprintf(fregion, "0x%02X , ", val);
      /// TODO: screen format
      printf("0x%02X ", val);
      p++;
    }
  }
    fprintf(fregion, "\r\n } } , \r\n");
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

