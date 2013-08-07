/*
  midi.c
  aleph-avr32

  usb MIDI functions.
*/

// asf
#include "print_funcs.h"
// aleph-avr32
#include "conf_usb_host.h"
#include "events.h"
#include "event_types.h"
#include "midi.h"
#include "uhi_midi.h"


//------------------------------------
//------ defines

/// ... ????
#define MIDI_RX_BUF_SIZE 64

//------------------------------------
//------ extern variables
u8 midiConnect = 0;

//------------------------------------
//------ static variables
u8 rxBuf[MIDI_RX_BUF_SIZE];
u32 rxBytes = 0;
u8 rxBusy = 0;
u8 txBusy = 0;

//------------------------------------
//----- static functions
static void midi_rx_done( usb_add_t add,
			  usb_ep_t ep,
			  uhd_trans_status_t stat,
			  iram_size_t nb) {
  int i;
  if(nb > 0) {

    print_dbg("\r\n midi rx; status: 0x");
    print_dbg_hex((u32)stat);
    print_dbg(" ; nb: ");
    print_dbg_ulong(nb);
    print_dbg(" ; data: ");
    for(i=0; i<nb; i++) {
      print_dbg("0x");
      print_dbg_hex(rxBuf[i]);
      print_dbg(" ");
    }
  }
}

static void midi_tx_done( usb_add_t add,
			  usb_ep_t ep,
			  uhd_trans_status_t stat,
			  iram_size_t nb) {

  //...
}

//-----------------------------------------
//----- extern functions
// read and spawn events (non-blocking)
extern void midi_read(void) {
  rxBytes = 0;
  rxBusy = true;
  if (!uhi_midi_in_run((u8*)rxBuf,
		       MIDI_RX_BUF_SIZE, &midi_rx_done)) {
    //    print_dbg("\r\n midi rx endpoint error");
  }
  return;
}

// write to MIDI device
extern void midi_write(u8* data, u32 bytes) {
  if (!uhi_midi_out_run(data, bytes, &midi_tx_done)) {
    txBusy = true;
    //    print_dbg("\r\n midi tx endpoint error");
  }
  return;
}

// MIDI device was plugged or unplugged
extern void midi_change(uhc_device_t* dev, u8 plug) {
  midiConnect = plug;
}

// main-loop setup routine for new device connection
///  do we need to make any control requests?
////
/* extern void midi_setup(void) { */
/* } */
