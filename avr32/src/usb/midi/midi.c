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
static u32 rxBytes = 0;
static u8 rxBusy = 0;
static u8 txBusy = 0;

// current packet data
//union { u8 buf[MIDI_MAX_PACKET_SIZE]; s32 data; } packet;
// address of last packet byte
//u8* packetEnd = &(packet.buf[3]);
// event
static event_t ev = { .eventType = kEventMidiPacket, .eventData = 0x00000000 };
static union { u8 buf[4]; u32 data; s32 sdata; } packet;
// pointers into the event data (hacky)
//static u8* const packetStart = (u8*)&(ev.eventData);
//static u8* const packetEnd = (u8*)&(ev.eventData) + 3;
static u8* const packetStart = &(packet.buf[0]);
static u8* const packetEnd = &(packet.buf[3]);

//------------------------------------
//----- static functions
// parse the buffer and spawn appropriate events
static void midi_parse(void) {
  // current byte data
  u8 b; 
  //// FIXME:
  /// for some reason, the UHC is getting an extra byte at the beginning
  /// this byte has the upper nibble of the real first byte,
  ///  echoed in its lower nibble.... weird
  u8* src = rxBuf + 1;
  // temp pointer to packet
  u8* dst = packetStart;
  // flag if we have received a status byte
  u8 stat = 0;
  // bytes processed
  u8 nb = 0;

  // clear packet data
  //  ev.eventData = 0x00000000;
  packet.data = 0x00000000;

  print_dbg("\r\n ");

  while(nb < rxBytes) {
    b = *src;
    ++src;
    ++nb;
    if(b & 0x80) {
      // this is a status byte
      dst = packetStart;
      //      print_dbg("\r\n parsed status byte. packet contents: 0x");
      //      print_dbg_hex(packet.data);
      print_dbg(" | SB ( ");
      print_dbg_hex(packet.data);
      print_dbg(" )");

      if(stat) {
	// if we already had a status byte, send the previous packet
	ev.eventData = packet.sdata;
	print_dbg("\r\n sending MIDI packet: "); 
	print_dbg_hex(packet.data);
	//	post_event(&ev);
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
    print_dbg("\r\n sending MIDI packet: "); 
    ev.eventData = packet.sdata;
    print_dbg_hex(packet.data);
    //    post_event(&ev);
  }
  print_dbg("\r\n");
  print_dbg("\r\n");
}

/*   for(i=0; i<rxBytes; ++i) { */
/*     b = *src; */
/*     //    print_dbg("\r\n midi_parse, byte: "); */
/*     //    print_dbg_hex((u32)b); */
/*     if(b & 0x80) { */
/*       // this is a status byte, start a new packet */
/*       dst = packetStart; */
/*       if(packOk) { */
/* 	// send the previous packet */
/* 	print_dbg("\r\n sending MIDI packet: "); */
/* 	print_dbg_char_hex((ev.eventData & 0xff000000) >> 24); */
/* 	print_dbg(" "); */
/* 	print_dbg_char_hex((ev.eventData & 0xff0000) >> 16); */
/* 	print_dbg(" "); */
/* 	print_dbg_char_hex((ev.eventData & 0xff00) >> 8); */
/* 	print_dbg(" "); */
/* 	print_dbg_char_hex(ev.eventData & 0xff); */
/* 	print_dbg(" "); */
/* 	post_event(&ev); */
/* 	// don't want to send again at the end of the loop */
/* 	packOk = 0; */
/*       } else { */
/* 	// maybe this packet is only a status byte */
/* 	packOk = 1; */
/*       } */
/*       // store status byte */
/*       *dst = b; */
/*     } else { */
/*       if(++dst > packetEnd) { */
/* 	// exceeded packet size, don't write, don't send */
/* 	packOk = 0; */
/*       } else { */
/* 	// store data byte, or whatever */
/* 	*dst = b; */
/* 	packOk = 1; */
/*       }  */
/*     } */
/*     ++src; */
/*   } */
/*   if(packOk) { */
/*     // send the previous packet */
/*     print_dbg("\r\n sending MIDI packet: "); */
/*     print_dbg_char_hex(ev.eventData & 0xff); */
/*     print_dbg(" "); */
/*     print_dbg_char_hex((ev.eventData & 0xff00) >> 8); */
/*     print_dbg(" "); */
/*     print_dbg_char_hex((ev.eventData & 0xff0000) >> 16); */
/*     print_dbg(" "); */
/*     print_dbg_char_hex((ev.eventData & 0xff000000) >> 24); */
/*     print_dbg(" "); */
/*     post_event(&ev); */
/*   } */
/* } */

// callback for the non-blocking asynchronous read.
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
      print_dbg_char_hex(rxBuf[i]);
      print_dbg(" ");
    }
  }
  //  rxBytes = nb;
  // weird extra first byte...
  rxBytes = nb - 1;
  midi_parse();
 
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
  event_t e;
  if(plug) { 
    e.eventType = kEventMidiConnect; 
  } else {
    e.eventType = kEventMidiDisconnect;
  }
  // posting an event so the main loop can respond
  post_event(&e); 
  //  midiConnect = plug;
}

// main-loop setup routine for new device connection
///  do we need to make any control requests?
////
/* extern void midi_setup(void) { */
/* } */
