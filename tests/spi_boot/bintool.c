/* bintool.c
 * 
 * tool to print the contents of a binary file as ASCII byte array, for easy inclusion in .c source
 */

#include <stdio.h>

int main(const int argc, const char** argv) {
  FILE *rf, *wf_data, *wf_size;
  char wname_data[64];
  char wname_size[64];
  unsigned char ch;
  unsigned long long int byteCount = 0;

  if (argc < 2) {
    printf("no input file specified, exiting\n");
    return 1;
  }
  
  rf = fopen(argv[1], "rb");

  if(rf == NULL) {
    printf("error opening input file\n");
    return 1;
  }

  snprintf(wname_data, 64, "%s.inc", argv[1]);
  wf_data = fopen(wname_data, "w");
  snprintf(wname_size, 64, "%s_size.inc", argv[1]);
  wf_size = fopen(wname_size, "w");
  
  while(fread(&ch, 1, 1, rf) == 1) {
    fprintf(wf_data, "0x%.2X,\n", (int)ch);
    byteCount++;
  }
  fprintf(wf_size, "%llu", byteCount);

  printf("wrote byte array to %s\n", wname_data);
  printf("wrote byte count to %s\n", wname_size);

  fclose(rf);
  fclose(wf_data);
  fclose(wf_size);

  return 0;
}
