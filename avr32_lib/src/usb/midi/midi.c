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

// RX buffer size.
// the buffer will start filling up if events come in faster than the polling rate
// the more full the buffer, the longer we'll spend in the usb read ISR parsing it...
// so it is a tradeoff.
#define MIDI_RX_BUF_SIZE 64

#define MIDI_TX_BUF_SIZE 64

// max packet length.
// a packet contains status and data byte for a typical midi message.
// sysex and other long/weird messages can be ignored for now.
// 4 bytes will fit in event data.
// longer than that we will have to use a global pointer to the rx buf or some shit.
#define MIDI_MAX_PACKET_SIZE 	4
#define MIDI_MAX_PACKET_SIZE_1 	3

//------------------------------------
//------ extern variables
u8 midiConnect = 0;

//------------------------------------
//------ static variables
static u8 rxBuf[MIDI_RX_BUF_SIZE];
static volatile u32 rxBytes = 0;
static volatile u8 rxBusy = 0;

// try using an output buffer and adding the extra nib we saw on input ... 
static u8 txBuf[MIDI_TX_BUF_SIZE];
static volatile u8 txBusy = 0;

// current packet data
//union { u8 buf[MIDI_MAX_PACKET_SIZE]; s32 data; } packet;
// address of last packet byte
//u8* packetEnd = &(packet.buf[3]);
// event
static event_t ev = { .type = kEventMidiPacket, .data = 0x00000000 };
static union { u8 buf[4]; u32 data; s32 sdata; } packet;
// pointers into the event data (hacky)
//static u8* const packetStart = (u8*)&(ev.data);
//static u8* const packetEnd = (u8*)&(ev.data) + 3;
static u8* const packetStart = &(packet.buf[0]);
static u8* const packetEnd = &(packet.buf[3]);

//------------------------------------
//----- static functions
// parse the buffer and spawn appropriate events
static void midi_parse(void) {
  // current byte data
  u8 b; 
  // skip the first byte ( CABLE | COM )
  volatile u8* src = rxBuf + 1;
  // temp pointer to packet
  u8* dst = packetStart;
  // flag if we have received a status byte
  u8 stat = 0;
  // bytes processed
  u8 nb = 0;

  // clear packet data
  //  ev.data = 0x00000000;
  packet.data = 0x00000000;

  //  print_dbg("\r\n ");

  while(nb < rxBytes) {
    b = *src;
    ++src;
    ++nb;
    if(b & 0x80) {
      // this is a status byte
      dst = packetStart;
      //      print_dbg("\r\n parsed status byte. packet contents: 0x");
      //      print_dbg_hex(packet.data);
      //      print_dbg(" | SB ( ");
      //      print_dbg_hex(packet.data);
      //      print_dbg(" )");

      if(stat) {
	// if we already had a status byte, send the previous packet
	ev.data = packet.sdata;
	//	print_dbg("\r\n sending MIDI packet: "); 
	//	print_dbg_hex(packet.data);
	event_post(&ev);
      }
      *dst = b;
      stat = 1;  
    } else {
      // this is a data byte or whatever
      ++dst;
      if(dst > packetEnd) {
	// exceeded packet size (sysex?), so give up on this packet
	stat = 0;
      } else {
	*dst = b;
      }
    }
  }
  // at the end of a buffer, send a packet if we have a pending status
  if(stat) {
    //    print_dbg("\r\n sending MIDI packet: "); 
    ev.data = packet.sdata;
    //    print_dbg_hex(packet.data);
    event_post(&ev);
  }
}

// callback for the non-blocking asynchronous read.
static void midi_rx_done( usb_add_t add,
			  usb_ep_t ep,
			  uhd_trans_status_t stat,
			  iram_size_t nb) {
  //  int i;
  rxBusy = 0;
  if(nb > 0) {
    /* print_dbg("\r\n midi rx; status: 0x"); */
    /* print_dbg_hex((u32)stat); */
    /* print_dbg(" ; nb: "); */
    /* print_dbg_ulong(nb); */
    /* print_dbg(" ; data: "); */
    /* for(i=0; i<nb; i++) { */
    /*   print_dbg_char_hex(rxBuf[i]); */
    /*   print_dbg(" "); */
    /* } */

    // ignoring 1st byte, virtual cable select
    rxBytes = nb - 1;
    midi_parse();
  } 
}

// callback for async write
static void midi_tx_done( usb_add_t add,
			  usb_ep_t ep,
			  uhd_trans_status_t stat,
			  iram_size_t nb) {

  txBusy = false;
  /* print_dbg("\r\n midi tx transfer callback. status: 0x");  */
  /* print_dbg_hex((u32)stat);  */
  if (stat != UHD_TRANS_NOERROR) {
    if(stat == UHD_TRANS_TIMEOUT) { 
      print_dbg("\r\n midi tx timeout error");
    } else {
      print_dbg("\r\n midi tx error (other)");
    }

    // FIXME
    // reschedule somehow if tx failed?
    // schedule next in tx queue, if we had one?

    return;
  }
}

//-----------------------------------------
//----- extern functions
// read and spawn events (non-blocking)
extern void midi_read(void) {
  rxBytes = 0;
  rxBusy = true;
  if (!uhi_midi_in_run((u8*)rxBuf,
		       MIDI_RX_BUF_SIZE, &midi_rx_done)) {
    print_dbg("\r\n midi rx endpoint error");
  }
  return;
}

// write to MIDI device
extern void midi_write(const u8* data, u32 bytes) {
  volatile u8* pbuf = &(txBuf[1]);
  int i;
 
  // copy to buffer 
  for(i=0; i<bytes; ++i){
    *pbuf++ = data[i];
  }

  // high nib of 1st byte = virtual cable index
  // low nib = 4b com code, duplicated from status byte
  txBuf[0] = (u8) (data[0] >> 4); 
  /// try cable idx 1
  /// FIXME: we should try and determine this from descriptors.
  /// see commented attempts at relevant requests in uhu_midi.c
  txBuf[0] |= 0x10;
 
  /* print_dbg("\r\n midi tx; data: "); */
  /* for(i=0; i<(bytes+1); i++) { */
  /*   print_dbg_char_hex(txBuf[i]); */
  /*   print_dbg(" "); */
  /* } */


  //  print_dbg("\r\n midi_write..."); 
  txBusy = true;
  if (!uhi_midi_out_run(txBuf, bytes+1, &midi_tx_done)) {
    print_dbg("\r\n midi tx endpoint error");
  }
  return;
}

// MIDI device was plugged or unplugged
extern void midi_change(uhc_device_t* dev, u8 plug) {
  event_t e;
  if(plug) { 
    e.type = kEventMidiConnect; 
  } else {
    e.type = kEventMidiDisconnect;
  }
  // posting an event so the main loop can respond
  event_post(&e); 
  //  midiConnect = plug;
}

// main-loop setup routine for new device connection
// FIXME: make control requests here (like virtual cable IDs)
////
/* extern void midi_setup(void) { */
/* } */
