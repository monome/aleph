/*
  main.c
  beekeep

  a bees simulator, heavily stripped. 
  its first job is to parse scene files and convert them to JSON/something.
  
 */

#include <stdio.h>
#include <string.h>
#include "getopt.h"

// jannson is a JSON manipulation library
#include "jansson.h"

#include "app.h"
#include "json.h"


u8 ui_loop(void) {
/*    char c = getc(stdin), stderr;
    if (c =='a') {
        printf("oh we got an a\n");
    }
    else {
        printf("not an a\n");
    }*/
    return 1;
}
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

static int display_error_message (void);
static int display_error_message (void) {
    print_dbg("\r\nUsage:\r\n");
    print_dbg("beekeep-0.5.0 (infile) [-o (outfile)] [-n] [-g] [-s]\r\n");
    print_dbg("-s:\tsimulate beekeep network (work in progress)\n");
    print_dbg("-n:\tdon't convert scn files to/fro json\n");
    print_dbg("-g:\tfire up aleph frontpanel mockup (work in progress)\n");
    print_dbg("\r\nExamples:\r\n");
    print_dbg("Convert scn file to json:\r\n\tbeekeep-0.5.0 example.scn -o example.json\r\n");
    print_dbg("Convert json file to scn:\r\n\tbeekeep-0.5.0 example.json -o example.scn\r\n");
    print_dbg("Run bees network with python gui front-end and don't do any conversion:\r\n\tbeekeep-0.5.0 example.json -s -n -g\r\n");
}
int main (int argc, char **argv)
{
    int sflag = 0;
    int gflag = 0;
    int nflag = 1;
    char *ovalue = NULL;
    int index;
    int c;
    char filename[32];
    char ext[16];

    opterr = 0;

    while ((c = getopt (argc, argv, "sgno:")) != -1)
    switch (c) {
        case 's':
            sflag = 1;
            break;
        case 'n':
            nflag = 0;
            break;
        case 'g':
            gflag = 1;
            break;
        case 'o':
            ovalue = optarg;
            break;
        case '?':
            if (optopt == 'o')
                fprintf (stderr, "Option -%c requires an argument.\n", optopt);
            else if (isprint (optopt))
                fprintf (stderr, "Unknown option `-%c'.\n", optopt);
            else
                fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
            return 1;
        default:
            abort ();
    }

    if(optind==argc) {
        display_error_message();
        return 1;
    }
    strcpy(filename, argv[optind]);
    scan_ext(filename, ext);
    if(index-optind > 0) {
        display_error_message();
        return 1;
    }

  if(!nflag) {
    ovalue=NULL;
  }
 fprintf (stderr, "\r\nRunning Beekeep with the folling parameters:\r\n\t simulate_network = %d, show_gui = %d, convert_scene = %d, outfile = %s, scenefile = %s\n", sflag, gflag, nflag, ovalue, filename);
  app_init();
  app_launch(1);

  if(strcmp(ext, ".scn") == 0) {
    files_load_scene_name(filename);
    net_print();
    strcat(filename, ".json");
    if(nflag) {
        printf(" \r\n file type is .scn, converting to .json \r\n");
        if(ovalue && nflag )
            net_write_json_native(ovalue);
        else if (nflag)
            net_write_json_native(filename);
        //    net_write_json_raw(filename);
    }
  }
  else if(strcmp(ext, ".json") == 0) {
    //    net_read_json_raw(filename);
    net_read_json_native(filename);
    strcat(filename, ".scn");
    if(nflag) {
        printf(" \r\n file type is .json, converting to .scn \r\n");
        if(ovalue)
            files_store_scene_name(ovalue);
        else
            files_store_scene_name(filename);
    }
  }
  else {
    printf(" unrecognized file type, exiting \r\n");
    return 1;
  }
  int run=1;
  if (sflag)
    while (run)
        run=ui_loop();
}
