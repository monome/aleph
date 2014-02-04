#ifndef _ALEPH_BEEKEEP_JSON_H_
#define _ALEPH_BEEKEEP_JSON_H_

#include "jansson.h"

/*
  network <-> json conversion.
 */


#include "net_protected.h"


// huge-ass string buffer (4MB)
#define STRBUF_LEN 0x400000
char strbuf[STRBUF_LEN];



// "raw" style: full representation of memory contents
extern void net_write_json_raw(const char* name);
extern void net_read_json_raw(const char* name);



#endif
