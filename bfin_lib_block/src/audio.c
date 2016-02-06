#include "audio.h"

// globally visible cycle count result
volatile u32 controlCycleCount;


// I/O flags
volatile u8 audioRxDone = 0;
volatile u8 audioTxDone = 0;

volatile u8 processAudio = 0;


// I/O buffers
buffer_t inputChannels0;
buffer_t inputChannels1;
buffer_t outputChannels0;
buffer_t outputChannels1;
