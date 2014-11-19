#ifndef _BEEKEEP_UI_FILES_H_
#define _BEEKEEP_UI_FILES_H_

// working directory
extern char workingDir[];

// strip filename from the end of a string
void strip_filename(char* str, u8 len);

// scan extension
extern void scan_ext(char* filename, char* ext);

// export .scn file
extern void write_scn(void);

// export .json  file
extern void write_json(void);

// export graphviz file
extern void write_gv(void);

#endif
