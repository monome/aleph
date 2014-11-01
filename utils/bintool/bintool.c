/* bintool.c
 * 
 * tool to print the contents of a binary file as ASCII byte array, for easy inclusion in .c source
 */

#include <stdio.h>
#include <string.h>
//#include <stdarg.h>
#include <getopt.h>

static int parse_args(const int argc, char* const* argv);

int main(int argc, char** argv) {
  FILE *rf, *wf_data, *wf_size;
  char infilename[64];
  char wname_data[64];
  char wname_size[64];
  unsigned char ch;
  unsigned long long int byteCount = 0;
  int c;
  int swap = 0;
  
  if (argc < 2) {
    printf("no input file specified, exiting\n");
    return 1;
  }


  while ((c = getopt (argc, argv, "b")) != -1) {
    switch (c) {
    case 'b':
      swap = 1;
      printf("\r\n byte-swapping (4b words) \r\n ");
      break;
    }
  }

  if(optind==argc) {
    printf("\r\n usage: bintool (infile) [-b] \r\n");
    return 1;
  }

  strcpy(infilename, argv[optind]);


  rf = fopen(infilename, "rb");

  if(rf == NULL) {
    printf("error opening input file\n");
    return 1;
  }

  snprintf(wname_data, 64, "%s.inc", infilename);
  wf_data = fopen(wname_data, "w");
  snprintf(wname_size, 64, "%s_size.inc", infilename);
  wf_size = fopen(wname_size, "w");
  
  if(swap) {
    char b[4];
    while(fread(&ch, 1, 1, rf) == 1) {
      if(byteCount % 4 == 3) {
	/// swap and print
	/* fprintf(wf_data, "0x%.2X,\n", (int)b[3]); */
	/* fprintf(wf_data, "0x%.2X,\n", (int)b[2]); */
	/* fprintf(wf_data, "0x%.2X,\n", (int)b[1]); */
	/* fprintf(wf_data, "0x%.2X,\n", (int)b[0]); */
	fprintf(wf_data, "0x%.2X,\n", (unsigned)(unsigned char)b[3]);
	fprintf(wf_data, "0x%.2X,\n", (unsigned)(unsigned char)b[2]);
	fprintf(wf_data, "0x%.2X,\n", (unsigned)(unsigned char)b[1]);
	fprintf(wf_data, "0x%.2X,\n", (unsigned)(unsigned char)b[0]);

      }
      b[byteCount % 4] = ch;
      byteCount++;
    }
  } else {
    while(fread(&ch, 1, 1, rf) == 1) {
      fprintf(wf_data, "0x%.2X,\n", (int)ch);
      byteCount++;
    }
  }
  fprintf(wf_size, "%llu", byteCount);

  printf("wrote byte array to %s\n", wname_data);
  printf("wrote byte count to %s\n", wname_size);

  fclose(rf);
  fclose(wf_data);
  fclose(wf_size);

  return 0;
}
