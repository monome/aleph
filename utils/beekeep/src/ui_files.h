#ifndef _BEEKEEP_UI_FILES_H_
#define _BEEKEEP_UI_FILES_H_


extern void scan_ext(char* filename, char* ext);

// export .scn file
extern void write_scn(void);

// export .json  file
extern void write_json(void);

// export graphviz file
extern void write_gv(void);

#endif
