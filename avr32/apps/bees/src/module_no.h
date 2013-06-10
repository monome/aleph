/* Module.h
 * sandbox
 *
 * public interface for sandbox processing Modules
 */

#ifndef _SBX_MODULE_INTERFACE_H_
#define _SBX_MODULE_INTERFACE_H_

#include "types.h"
#include "util.h"
#include "param.h"

//=============== defines
// audio processing block size
#define SBX_BLOCKSIZE   32
// audio processing sample rate
#define SBX_SAMPLERATE  48000
// blockrate = samplerate / blocksize (rounded)
#define SBX_BLOCKRATE 1500

//=============== types
// typedef struct ModuleDataStruct* ModuleDataHandle;

//=============== generic module functions (defined in Module.c)
// getters
const char* get_module_name(void);
const u16 get_module_minor_version(void);
const u16 get_module_major_version(void);
u16 get_(void);

// TODO: move param allocation to Module.c and get rid of these fucked up access functions
// get a parameter handle... use responsibly
ParamHandle getParam(u16 idx);
// get a pointer to a parameter handle... use very responsibly
ParamHandle* getParamPointer(u16 idx);
// get a pointer the whole params array... use extra responsibly
ParamHandle** getParamArrayPointer(void);

// setters
void setNumParams(u16 num);
void setModuleName(const char* name);
void setModuleVersionMajor(u16 v);
void setModuleVersionMinor(u16 v);

// =========== processing module functions (defined in shared libraries)
// process a block of audio. SR and blocksize are fixed.
u8 sbxCallback(const f32*, f32*);
// allocate processing and parameter memory
u8 sbxInit(void);
// de-allocate
u8 sbxDeinit(void);

#endif //_SBX_MODULE_H_
