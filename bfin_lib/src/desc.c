/*

  desc.c

small utility to build the paramater descriptor data for a module.

this sourcefile builds a small linux program to wrute param descriptor binary for a given module.

compile with 
-D NAME="waves" 
, etc.

 */

#include <string.h>
#include <stdio.h>

#include "param_common.h"

#include "fix.h"
#include "module.h"

#include "module_custom.h"
#include "osc.h"
#include "params.h" 
#include "pickle.h"
#include "types.h"


//----------------
//--- static vars
//ModuleData data;
ParamDesc desc[eParamNumParams];
char str[64] = "aleph-";

// big ol buffer (1meg)
#define MAXBYTES 0x100000
char buf[MAXBYTES];

//--------------------
//---- static func
//void fill_param_desc(void);

////////////////////////////////x
//// FIXME:
/* copying these functions from BEES. dirty
*/

// pickle/unpickle for param descriptors 
// (struct definition in param_common.h
u8* pdesc_pickle(ParamDesc* pdesc, u8* dst) {
  u8 i;
  // store label string
  for(i=0; i<PARAM_LABEL_LEN; ++i) {
    *dst = pdesc->label[i];
    ++dst;
  }
  // store type
  // pad for alignment
  dst = pickle_32((u32)(pdesc->type), dst);
  // store min
  dst = pickle_32(pdesc->min, dst);
  // store max
  dst = pickle_32(pdesc->max, dst);
  // store radix
  // pad for alignment
  dst = pickle_32((u32)(pdesc->radix), dst);

  return dst;
}

/////////
////////////////////////////////

//---- main function
int main() {
  FILE* out;
  int i, j;
  // write pointer
  u8* pwr;


  //  data.paramDesc = desc;
  strcat(str, NAME);
  strcat(str, ".dsc");

  fill_param_desc(desc);
  printf("\r\n %s_desc_build ... ", NAME);
  printf("\r\n filled param descripor table... \r\n");
  
  out = fopen(str, "w");

  pwr = buf;

  // pack the count of parameters
  pwr = pickle_32(eParamNumParams, pwr);

  // serialize the descriptors
  for(i=0; i<eParamNumParams; i++) {
    pwr = pdesc_pickle(&(desc[i]), pwr);
  }

  // write to file
  fwrite(buf, 1, (size_t)((size_t)pwr - (size_t)&buf), out);

  printf("\r\n wrote %d bytes... \r\n", (int) ((size_t)pwr - (size_t)&buf));
  
  fclose(out);
  printf("\r\n done. \r\n");
  

  return 0;
}

