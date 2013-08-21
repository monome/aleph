/*
 * audio.c
 * null
 * aleph
 *
 * portaudio-based test audio host for bfin module development
 * (linux only)
 */

// std
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

// external
#include <portaudio.h>

// aleph
//#include "module_common.h"
#include "module.h"
#include "types.h"
#include "audio.h"

fract32 in[4];

//=========================
//===== variables
PaStream *stream;
PaError err;
PaStreamParameters inputParameters;
PaStreamParameters outputParameters;


//========================
//== functions

static int paCallback( const void *inputBuffer,
		       void *outputBuffer,
		       unsigned long framesPerBuffer,
		       const PaStreamCallbackTimeInfo* timeInfo,
		       PaStreamCallbackFlags statusFlags,
		       void *userData ) {
    const float *in = (const float*)inputBuffer;
    float *out = (float*)outputBuffer;
    u32 samp;

    module_process_frame( in, out);

    

    //// TEST: wire
    //////////
    /* for(samp=0;  samp<(BLOCKSIZE * NUMCHANNELS); samp++) { */
    /*   out[samp] = in[samp]; */
    /* } */
    ///////////
    //////////////

    return 0;
}


void init_audio(void) {
  
  /// try doing this first
  module_init();

  err = Pa_Initialize();
  if(err != paNoError) goto done;

  inputParameters.device = Pa_GetDefaultInputDevice();
  if (inputParameters.device == paNoDevice) {
    fprintf(stderr,"Error: No input default device.\n");
    goto done;
  }

  //  printf("channels: %d ; format: %d ; samplerate: %d, blocksize: %d \n",
  //	 NUMCHANNELS, FORMAT, SAMPLERATE, BLOCKSIZE);

  inputParameters.channelCount = NUMCHANNELS;                   
  inputParameters.sampleFormat = FORMAT;           
  inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
  inputParameters.hostApiSpecificStreamInfo = NULL;

  outputParameters.device = Pa_GetDefaultOutputDevice();  /* default output device */
  if (outputParameters.device == paNoDevice) {
    fprintf(stderr,"Error: No default output device.\n");
    goto done;
  }
  outputParameters.channelCount = NUMCHANNELS;    
  outputParameters.sampleFormat = FORMAT;
  outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
  outputParameters.hostApiSpecificStreamInfo = NULL;

  err = Pa_OpenStream(
		      &stream,
		      &inputParameters,
		      &outputParameters,
		      (double)SAMPLERATE,
		      BLOCKSIZE,               
		      paClipOff,         
		      paCallback,
		      NULL );
  if( err != paNoError ) goto done;

  err = Pa_StartStream( stream );
  if( err != paNoError ) goto done;

  return;
  
 done:
  Pa_Terminate();
  
  if( err != paNoError ) {
    fprintf( stderr, "An error occured while using portaudio\n" );  
    fprintf( stderr, " Error number: %d\n", err );
  }
}

void deinit_audio(void) {
  err = Pa_CloseStream(stream);
  while( ( err = Pa_IsStreamActive( stream ) ) == 1 ) Pa_Sleep(1000);
  err = Pa_Terminate();
  if( err != paNoError ) {
    fprintf( stderr, "An error occured while using portaudio\n" );  
    fprintf( stderr, " Error number: %d\n", err );
  }
}
