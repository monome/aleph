/*
 * audio.c
 * bees
 * aleph
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <unistd.h>

#include "module.h"
#include "audio.h"

//========================================
//============== defines

// detect the operating system

#ifdef __APPLE__
  #define SBX_LIB_PREFIX "/usr/lib/libsbx-"
  #define SBX_LIB_SUFFIX ".A.dylib"
#else
  #define SBX_LIB_PREFIX "/opt/lib/libsbx-"
  #define SBX_LIB_SUFFIX ".so"
#endif



//========================================================
//==============static variables
// functions pointers for sbx module routines
static int (*mod_callback)(const f32*, f32*) = 0;
static u16 (*mod_init)() = 0;
static u16 (*mod_deinit)() = 0;

static void* lib_handle = 0;

// PortAudio stuff
static PaStreamParameters inputParameters, outputParameters;
static PaStream *stream;
static PaError err;

//=================================================
//==================== static function declarations
static u16 paError(u16 err);
// PA only wants builtin types...
static int paCallback(const void* inBuf,
		      void* outBuf,
		      u32 frames,
		      const PaStreamCallbackTimeInfo* timeInfo,
		      PaStreamCallbackFlags statusFlags,
		      void* userData);

//==================================================
//===================== static function definitions
// audio callback
int paCallback(const void* inBuf,
	       void* outBuf,
	       u32 frames,
	       const PaStreamCallbackTimeInfo* timeInfo,
	       PaStreamCallbackFlags statusFlags,
	       void* userData) {
  const f32* in;
  f32* out;
  u32 frame;
  const void* dumdum = userData;
  
  if(inBuf == NULL) { DEBUG("bad input pointer\n"); return 1; }
  if(outBuf == NULL) { DEBUG("bad output pointer\n"); return 1; }
  
  in = (const float*) inBuf;
  out = (float*) outBuf;
  
  (*psbxCallback)(inBuf, outBuf);
}

// audio init
u16 initAudio(void) {
  
 err = Pa_Initialize();
 if (err != paNoError) paError(err);

  DEBUG("pa no error\n");
	
  // load a default plugin
  //  loadModule("cube");
  loadModule("echo");

  if (psbxCallback == 0) {
    DEBUG("couldn't load default plugin, aborting init\n");
    return 1;
  }
	
  inputParameters.device = Pa_GetDefaultInputDevice();
  inputParameters.channelCount = 2;       
  inputParameters.sampleFormat = paFloat32;
  inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
  inputParameters.hostApiSpecificStreamInfo = NULL;
	
  outputParameters.device = Pa_GetDefaultOutputDevice(); 
  outputParameters.channelCount = 2;      
  outputParameters.sampleFormat = paFloat32;
  outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
  outputParameters.hostApiSpecificStreamInfo = NULL;
	
  err = Pa_OpenStream(&stream,
		      &inputParameters,
		      &outputParameters,
		      SBX_SAMPLERATE,
		      SBX_BLOCKSIZE,   
		      0, // no flags == no clipping
		      paCallback,
		      NULL);
   if (err != paNoError) paError(err); 
   err = Pa_StartStream(stream);
   if (err != paNoError) paError(err); 
 }

 u16 paError(u16 err) {
   Pa_Terminate();
   DEBUG( "An error occured while using the portaudio stream\n" );
   DEBUG( "Error number: %d\n", err );
   DEBUG( "Error message: %s\n", Pa_GetErrorText( err ) );	
   return err;
}

// audio de-init
u16 deinitAudio(void) {
  Pa_Terminate();
    usleep(100000);
  (*psbxDeinit)();
  dlclose(lib_handle);
}


//--------------------------------------------------------
u16 loadModule(const s8* libstring_arg)// , PaStream* stream)
{
  // can't use our types for PA function pointers... irritating!
  int (*psbxCallback_save)(const f32*, f32*) = psbxCallback;
  u16 (*psbxInit_save)() = psbxInit;
  u16 (*psbxDeinit_save)() = psbxDeinit;
  
  void* newlib_handle;
  char libstring[256] = SBX_LIB_PREFIX;

  strncat(libstring, libstring_arg, 256);
  strncat(libstring, SBX_LIB_SUFFIX, 128);

  DEBUG("opening new libhandle\n");
  newlib_handle = dlopen(libstring, RTLD_LOCAL|RTLD_LAZY);
  DEBUG("opened %s\n", libstring);

  if (!newlib_handle) {
    DEBUG("library handle is zero: %s\n", libstring);
    return 1;
  }
  
  if (lib_handle) {
 
    ///// pause the Pa stream...
    if (stream != 0) Pa_StopStream(stream);
    DEBUG("closed stream\n");
    
    ///// wait 0.1sec
    usleep(100000);
 
    DEBUG("waited\n");
    
    // de-initialize the plugin (including parameters)
    (*psbxDeinit)();
    
    dlclose(lib_handle);
    DEBUG("closed libhandle\n");
    
    lib_handle = 0;
    
    psbxCallback = 0;
    psbxInit = 0;
    psbxDeinit = 0;
  }
  
  lib_handle = newlib_handle;
  
  psbxCallback = dlsym(lib_handle, "sbxCallback");
  if (!psbxCallback) {
    DEBUG("couldn't load callback symbol...\n");
    return 2;
  }	
  
  psbxInit = dlsym(lib_handle, "sbxInit");
  if (!psbxInit) {
    DEBUG("couldn't load init symbol...\n");
    return 2;
  }	
  
  psbxDeinit = dlsym(lib_handle, "sbxDeinit");
  if (!psbxDeinit) {
    DEBUG("couldn't load deinit symbol...\n");
    return 2;
  }	
  
  (*psbxInit)();	

  if (stream != 0) Pa_StartStream(stream);
	
  return 0;
}
