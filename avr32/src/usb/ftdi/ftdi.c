/*
  ftdi.c
  aleph-avr32

  FTDI usb functions.
*/

// asf
#include "delay.h"
#include "print_funcs.h"
// aleph
#include "conf_usb_host.h"
#include "events.h"
#include "event_types.h"
#include "ftdi.h"
#include "global.h"
#include "uhi_ftdi.h"

//---- defines


//---- gloabl vars (initialized here) 
u8 ftdiPlug = 0;

//----- static vars
static u8 ftdi_in_buf[FTDI_IN_BUF_SIZE]; //= { 0, 0 };
static u8 ftdi_out_buf[FTDI_OUT_BUF_SIZE];// = { 0, 0 };
static volatile u8 ftdi_out_busy = 0;
static volatile u8 ftdi_in_busy = 0;
static volatile  uhd_trans_status_t ftdi_transfer_status = 0;

//------- static functions

// debug print
static void print_arr(u32 addr, u32 size);
static void print_unicode_string(char* str, u32 len);

// usb bulk transfer callback
static void ftdi_transfer_done(
			       usb_add_t add,
			       usb_ep_t ep,
			       uhd_trans_status_t status,
			       iram_size_t nb_transfered) {
  //  print_dbg("\r\n ftdi transfer done; status: ");
  //  print_dbg_hex((u32)status);
  //  print_dbg(" ; bytes transferred : ");
  //  print_dbg_ulong((u32)nb_transfered);

  ftdi_transfer_status = status;
  if (ep & USB_EP_DIR_IN) { 
    ftdi_in_busy = 0;
  } else {
    ftdi_out_busy = 0;
  }
}

//-------- extern functions
void ftdi_write(u32 data) {
  //  int i;
  u8 val;
  ftdi_out_busy = 1;

  //// testing
  if(data > 0) {
    val = 145;
  } else {
    val = 144;
  }
  //  for(i=0; i<FTDI_OUT_BUF_SIZE; i++) {
  //    ftdi_out_buf[i] = val;
  //  }
  ///////
  /*
  if(!uhi_ftdi_out_run(ftdi_out_buf,
		       FTDI_OUT_BUF_SIZE,
		       &ftdi_transfer_done)) {
  */
  if(!uhi_ftdi_out_run(&val,
		       1,
		       &ftdi_transfer_done)) {

    print_dbg("\r\n ftdi bulk output error");
    return;
  }
  while (ftdi_out_busy) { ;; }
  print_dbg("\r\n finished ftdi bulk output transfer");
  
  if (ftdi_transfer_status != UHD_TRANS_NOERROR) {
    return;
  }
}
    
void ftdi_read(void) {
    // poll FTDI input
    ftdi_in_busy = true;
    if (!uhi_ftdi_in_run(ftdi_in_buf,
			 FTDI_IN_BUF_SIZE, &ftdi_transfer_done)) {
      print_dbg("\r\n uhi_ftdi_in_run returned false; aborting input transfer");
      return;
    }
    while (ftdi_in_busy) { ;; }
    if (ftdi_transfer_status != UHD_TRANS_NOERROR) {
      print_dbg("\r\n ftdi input transfer error, exiting poll function");
      return;
    }
    print_arr((u32)ftdi_in_buf, FTDI_IN_BUF_SIZE);
    //// ... todo : parse events
}


// respond to connection or disconnection of ftdi device.
// may be called from an interrupt
void ftdi_change(uhc_device_t* dev, u8 plug) {
  event_t e;
  print_dbg("\r\n changed FTDI connection status");
  if(plug) { 
    e.eventType = kEventFtdiConnect; 
  } else {
    e.eventType = kEventFtdiDisconnect;
  }
  // posting an event so the main loop can respond
  post_event(&e);
  
}

// setup new device connection
void ftdi_setup(void) {
  char * pManufacturerString = 0;
  char * pProductString = 0; 
  char * pSerialString = 0;
  print_dbg("\r\n FTDI setup routine");

  // get string data...
  ftdi_get_strings(&pManufacturerString, &pProductString, &pSerialString);
  
  // print the strings
  print_unicode_string(pManufacturerString, FTDI_STRING_MAX_LEN);
  print_unicode_string(pProductString, FTDI_STRING_MAX_LEN);
  print_unicode_string(pSerialString, FTDI_STRING_MAX_LEN);

  // set connection flag
  ftdiPlug = 1;
}

static void print_arr (u32 start, u32 size ) {
  u32 i, j;
  u32 b, boff;
  u32 addr = start;
  print_dbg("\r\n");
  print_dbg_hex(addr);
  print_dbg(" : " ); //\r\n");
  j = 0;
  while(j<size) {
    b = 0;
    boff = 24;
    for(i=0; i<4; i++) {
      //      if(i == 2) { print_dbg(" "); }
      b |= ( *(u8*)addr ) << boff;
      addr++;
      boff -= 8;
    }
    print_dbg_hex(b);
    print_dbg(" ");
    j += 4;
    //    if( (j%16) == 0) { print_dbg("\r\n"); }
  }
}

static void print_unicode_string(char* str, u32 len) {
  u32 i; // byte index
  print_dbg("\r\n");
  for(i=0; i<len; i++) {
    print_dbg_char(str[i]);
  }
}
