/*
  ftdi.c
  aleph-avr32

  FTDI usb functions.
*/

// asf
#include "print_funcs.h"
// aleph
#include "conf_usb_host.h"
#include "ftdi.h"
#include "global.h"
#include "uhi_ftdi.h"

//---- gloabl vars (initialized here) 
u8 ftdiPlug = 0;

//----- static vars
static u8 ftdi_in_buf[FTDI_IN_BUF_SIZE];
static u8 ftdi_out_buf[FTDI_OUT_BUF_SIZE];
static u8 ftdi_out_busy = 0;
static u8 ftdi_in_busy = 0;
static u8 ftdi_transfer_status = 0;

//------- static functions

// debug print
static void print_arr(u32 addr, u32 size);

// usb bulk transfer callback
static void ftdi_transfer_done(
			       usb_add_t add,
			       usb_ep_t ep,
			       uhd_trans_status_t status,
			       iram_size_t nb_transfered) {
  ftdi_transfer_status = status;
  if (ep & USB_EP_DIR_IN) { 
    ftdi_in_busy = 0;
  } else {
    ftdi_out_busy = 0;
  }
}

//-------- extern functions
void ftdi_send(void) {
    ftdi_out_busy = 1;
    if(!uhi_ftdi_out_run(ftdi_out_buf,
    			 FTDI_OUT_BUF_SIZE,
    			 ftdi_transfer_done)) {
      return;
    }
    while (ftdi_out_busy) { ;; }

    if (ftdi_transfer_status != UHD_TRANS_NOERROR) {
      return;
    }
}
    
void ftdi_receive(void) {
    // poll FTDI input
    ftdi_in_busy = true;
    if (!uhi_ftdi_in_run(ftdi_in_buf,
			 FTDI_IN_BUF_SIZE, ftdi_transfer_done)) {
      return;
    }
    while (ftdi_in_busy);
    if (ftdi_transfer_status != UHD_TRANS_NOERROR) {
      return;
    }
    print_arr((u32)ftdi_in_buf, FTDI_IN_BUF_SIZE);
}

/* void ftdi_send(void) { */
  
/* 	return uhd_ep_run(uhi_vendor_dev.dev->address, */
/* 			uhi_vendor_dev.ep_bulk_out, false, buf, buf_size, */
/* 			UHI_VENDOR_TIMEOUT, callback); */
/* } */


/* void ftdi_receive(void) { */
  
/* } */


void ftdi_change(uhc_device_t* dev, u8 plug) {
  ftdiPlug = plug;
}




static void print_arr (u32 start, u32 size ) {
  u32 i, j;
  u32 b, boff;
  u32 addr = start;
  print_dbg("\r\n");
  print_dbg_hex(addr);
  print_dbg(" : \r\n");
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
    if( (j%16) == 0) { print_dbg("\r\n"); }
  }
}
