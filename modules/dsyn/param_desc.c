#include <string.h>
#include "module.h"
#include "dsyn.h"
#include "param_common.h"
#include "params.h" 

static const char* paramStrings[] = {
  "Gate",		// 0
  "Trig",		// 1
  "Amp",		// 2
  "AmpSus",      	// 3
  "AmpAtkSlew",	// 4
  "AmpDecSlew",	// 5
  "AmpRelSlew",	// 6
  "AmpSusDur",	// 7
  "FreqOff",	// 8
  "FreqOn",	// 9
  "FreqSus",	// 10 
  "FreqAtkSlew",	// 11
  "FreqDecSlew",	// 12
  "FreqRelSlew",	// 13
  "FreqSusDur",	// 14
  "RqOff",		// 15
  "RqOn",		// 16
  "RqSus",		// 17
  "RqAtkSlew",	// 18
  "RqDecSlew",	// 19
  "RqRelSlew",	// 20
  "RqSusDur",	// 21
  "Low",		// 22
  "High",		// 23
  "Band",		// 24
  "Notch",		// 25
  "SvfPre",	// 26
  "FreqEnv",	// 27
  "RqEnv",		// 28
};


void fill_param_desc(ParamDesc* desc) {
  u32 i, j, k;
  char buf[32] = "";
  const char * numstrs[] = { "0", "1", "2", "3" };
  //char strbuf[32];
  k = 0;
  //  for(i=0; i<eParamNumParams; i++) {
  for(i=0; i<DRUMSYN_NVOICES; i++) {
    for(j=0; j < PARAM_VOICE_NPARAMS; j++) {
    /// ... print to string
      strcpy(buf, paramStrings[j]);
      strcat(buf, numstrs[i]);
      strcpy(desc[k].label, buf);
      desc[k].type = eParamTypeFract;
      desc[k].min = 0;
      desc[k].max = FR32_MAX;
      k++;
    }
  }
}
