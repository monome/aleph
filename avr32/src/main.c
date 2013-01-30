/* main.c
 * avr32_test
 * aleph
 *
 * 
 */


//// ASF
#include <string.h>
#include <stdio.h>
//#include <sysclk.h>
#include "board.h"
#include "conf_sd_mmc_spi.h"
#include "compiler.h"
#include "cycle_counter.h"
#include "ctrl_access.h"
#include "util.h"
#include "gpio.h"
#include "intc.h"
#include "pdca.h"
#include "power_clocks_lib.h"
#include "print_funcs.h"
#include "sd_mmc_spi.h"
#include "sdramc.h"
#include "spi.h"
#include "sysclk.h"
#include "usart.h"
//// aleph
// bees
#include "menu.h"
#include "net.h"
#include "net_protected.h"
#include "preset.h"
#include "scene.h"
// common
#include "files.h"
#include "param_common.h"
#include "screen.h"
#include "simple_string.h"
#include "types.h"
// avr32
#include "adc.h"
#include "app_timers.h"
#include "bfin.h"
#include "conf_aleph.h"
#include "encoders.h"
#include "events.h"
#include "filesystem.h"
#include "fix.h"
#include "font.h"
#include "global.h"
#include "i2c.h"
#include "init.h"
#include "interrupts.h"
#include "memory.h"
#include "switches.h"
#include "timers.h"

//=========================================
//==== static variables

// control network, statically allocated
ctlnet_t ctlnet;
//  flag to wait for startup button press
static u8 startup = 1;
// mode switch
static u8 mode = 0;

/*
  static u8 ldrData[] = {
  #include "bfin_test.ldr.inc"
  };

  static u32 ldrSize = 
  #include "bfin_test.ldr_size.inc"
  ;
*/


//=========================================== 
//==== static functions

//------  declare
//  app event loop
static void check_events(void);
// startup routine (perform this once after an initial button press)
static void perform_startup(void);

// start master mode on TWI
//static void init_master(u8 addr);
// send some data as TWI master, return status
//static u8 send_master(u8 chip, u32 addr, u8 addr_len, u32 len, void* data);

// get params from bfin
// static void report_params(void);

//----------------------------------------------
//---- testing stuff
//static void twi_test(void);
  
// display scrolling history of events
static void scroll_event(const char* str, s16 val);
static char eventScroll[CHAR_ROWS][CHAR_COLS];
static u64 eventScrollTimes[CHAR_ROWS];
static s16 eventScrollData[CHAR_ROWS];
static u8 scrollIdx = 0;

// display ADC
static void displayAdcVal(u8 idx, u16 val);

// display processing time in event loop
// static void displayEventLoad(void);
// static u64 maxEventCycles = 0;
//-------------------------------------------------

// number of parameters
// volatile u32 numParams = 0;

//------- define
// app event loop
static void check_events(void) {
  static event_t e;  
  static u64 delay;
  //static U64 cycles = 0;
  //static U64 cyclesNow = 0;
  //  cycles = Get_system_register(AVR32_COUNT)  

  if( get_next_event(&e) ) {
    if(startup) {
      /// a hack!
      /// this should select only switch events...
      if(e.eventType > kEventEncoder3) { 
	perform_startup();
	///// TEST I2C as master
	//	i2c_tx(44, 0, 0, 5, &(e.eventType) - 1);
	return;
      }
    } else {
    
      switch(e.eventType) {

      case kEventRefresh:
	screen_refresh();
	break;
      case kEventSwitchDown0:
	menu_handleKey(eKeyFnDownA);
	break;
      case kEventSwitchUp0:
	menu_handleKey(eKeyFnUpA);
	break;
      case kEventSwitchDown1:
	print_dbg("\r\n switch f1 down");
	menu_handleKey(eKeyFnDownB);
	break;
      case kEventSwitchUp1:
	print_dbg("\r\n switch f1 up");
	menu_handleKey(eKeyFnUpB);
	break;
      case kEventSwitchDown2:
	print_dbg("\r\n switch f2 down");
	//	menu_handleKey(eKeyFnDownC);
	break;
      case kEventSwitchUp2:
	print_dbg("\r\n switch f2 up");
	//	menu_handleKey(eKeyFnUpC);
	break;
      case kEventSwitchDown3:
	menu_handleKey(eKeyFnDownD);
	break;
      case kEventSwitchUp3:
	menu_handleKey(eKeyFnUpD);
	break;
      case kEventSwitchDown4:
	mode ^= 1;
	if(mode) { gpio_set_gpio_pin(LED_MODE_PIN); }
	else { gpio_clr_gpio_pin(LED_MODE_PIN); }
	menu_handleKey(eKeyMode);
	break;
      case kEventSwitchUp4:
	break;
      case kEventSwitchDown5:
	screen_line(0, 0, "powering down!", 0x3f);
	print_dbg("\r\n AVR32 received power down switch event");
	screen_refresh();
	delay = 50000; while(--delay) {;;}
	gpio_clr_gpio_pin(POWER_CTL_PIN);
	break;
      case kEventSwitchUp5:
	break;
      case kEventEncoder0:
	if(e.eventData > 0) {
	  menu_handleKey(eKeyEncUpA);
	} else {
	  menu_handleKey(eKeyEncDownA);
	}
	break;
      case kEventEncoder1:
	if(e.eventData > 0) {
	  menu_handleKey(eKeyEncUpD);
	} else {
	  menu_handleKey(eKeyEncDownD);
	}
	break;
      case kEventEncoder2:
	if(e.eventData > 0) {
	  menu_handleKey(eKeyEncUpC);
	} else {
	  menu_handleKey(eKeyEncDownC);
	}
	break;
      case kEventEncoder3:
	if(e.eventData > 0) {
	  menu_handleKey(eKeyEncUpB);
	} else {
	  menu_handleKey(eKeyEncDownB);
	}
	break;
      case kEventAdc0:
	//print_dbg("\r\nadc val 0: ");
	//	print_dbg_hex(e.eventData);
	//	displayAdcVal(0, e.eventData);
	break;
      case kEventAdc1:
	// print_dbg("\r\nadc val 1: ");
	// print_dbg_hex(e.eventData);
	//	displayAdcVal(1, e.eventData);
	break;
      case kEventAdc2:
	// print_dbg("\r\nadc val 2: ");
	// print_dbg_hex(e.eventData);
	//	displayAdcVal(2, e.eventData);
	break;
      case kEventAdc3:
	//	print_dbg("\r\nadc val 3: ");
	//	print_dbg_hex(e.eventData);
	//	displayAdcVal(3, e.eventData);
	break;
      }
      /*
	cyclesNow = Get_system_register(AVR32_COUNT);
	if(cyclesNow > maxEventCycles) {
	maxEventCycles = cyclesNow;
	//      displayEventLoad();
	}
      */
    } // if event
  } // if !startup
}


////////////////////
////////////////////
//test pdca
static void test_pdca(void) {
  int i, j;
  int n;
  // Read the first//#define BUFSIZE 512 >> 2 << 9
  //N sectors
  //  n = 0x8fffffff >> 9;
  n = 16;
  for(j = 0; j < n; j++) {
  
    pdca_load_channel( AVR32_PDCA_CHANNEL_SPI_RX,
		       &pdcaRxBuf,
		       FS_BUF_SIZE);

    pdca_load_channel( AVR32_PDCA_CHANNEL_SPI_TX,
		       (void *)&pdcaTxBuf,
		       FS_BUF_SIZE );

    fsEndTransfer = false;
    // open sector number j
    if(sd_mmc_spi_read_open_PDCA (j)) {
      print_dbg("\r\n\nblock");
      print_dbg_ulong(j);
      print_dbg(" : \r\n");

      spi_write(SD_MMC_SPI,0xFF); // Write a first dummy data to synchronise transfer
      pdca_enable_interrupt_transfer_complete(AVR32_PDCA_CHANNEL_SPI_RX);
      pdcaRxChan =(volatile avr32_pdca_channel_t*) pdca_get_handler(AVR32_PDCA_CHANNEL_SPI_RX); 
      pdcaTxChan =(volatile avr32_pdca_channel_t*) pdca_get_handler(AVR32_PDCA_CHANNEL_SPI_TX);
      pdcaRxChan->cr = AVR32_PDCA_TEN_MASK; // Enable RX PDCA transfer first
      pdcaTxChan->cr = AVR32_PDCA_TEN_MASK; // and TX PDCA transfer

      while(!fsEndTransfer);

      // display sector contents
      for( i = 0; i < 512; i++) {
	if((i % 16) == 0) { print_dbg("\r\n"); }
	if((i % 2) == 0) { print_dbg(" "); }
        print_dbg_char_hex( (U8)(*(pdcaRxBuf + i)));
      }
    }
    else {
      print_dbg("\r\n unable to open memory; block ");
      print_dbg_ulong(j);
      print_dbg("\r\n");
    }
  }
  print_dbg("\r\n");
}
////////////////////
////////////////////


////main function
int main (void) {
  u32 dum;

  U32 waitForCard = 0;
  volatile avr32_tc_t *tc = APP_TC;
  //  volatile u64 delay;

  //  gpio_set_gpio_pin(POWER_CTL_PIN);
  //  gpio_clr_gpio_pin(POWER_CTL_PIN);
  
  // initialize clocks:
  init_clocks();
  
  // initialize Interrupt Controller
  INTC_init_interrupts();

  // disable interrupts
  Disable_global_interrupt();

  // initialize RS232 debug uart
  init_ftdi_usart();

  // initialize spi1: OLED, ADC, SD/MMC
  init_spi1();

  // initialize PDCA controller
  init_local_pdca();

  // initialize blackfin resources
  init_bfin_resources();

  // initialize application timer
  init_tc(tc);

  // initialize other GPIO
  init_gpio();

  // register interrupts
  register_interrupts();

  // initialize the OLED screen
  init_oled();
  
  // intialize the event queue
  init_events();
  
  // intialize encoders
  init_encoders();

  // intialize switches (debouncing)
  init_switches();

  // initialize sdram
  sdramc_init(FHSB_HZ);

  //memory manager
  init_mem();
  //  sdram_test(SDRAM_SIZE >> 3, (SDRAM_SIZE >> 3) * 6);

  // initialize twi
  init_twi();
  
  // Enable all interrupts.
  Enable_global_interrupt();

  // Wait for a card to be inserted
  
  screen_line(0, 0, "ALEPH", 0x3f);
  screen_line(0, 1, "waiting for SD card...", 0x3f);
  screen_refresh();
  
  print_dbg("\r\n SD check... ");
  while (!sd_mmc_spi_mem_check()) {
    waitForCard++;
    //    print_dbg("\r\n not found: ");
    //    print_dbg_ulong(waitForCard);
  }
  print_dbg("\r\nfound SD card. ");

  screen_blank_line(0, 0);
  screen_blank_line(0, 1);
  screen_line(0, 0, "SD card detected.", 0x3f);
  screen_line(0, 1, "press any key to continue...", 0x3f);
  screen_refresh();
  
  ///////////////
  ////////////
  //// I2C test
  //    i2c_init(44);
  //       i2c_init(66);
  //  screen_line(0, 0, "testing I2C as master.", 0x3f);
  //  screen_refresh();
  
  //  test_pdca();

  // set up file navigation
  init_files();


  // scan drives, list files
  //  files_list();  
  /// print all files to UART

  //  files_scan_dsp();

  //  files_load_dsp_name("bfin_test.ldr");
  //////////////////////
  //////////////// TEST
  //  bfin_load(ldrSize, ldrData);
    ///////////////
    /////////////////

  // send ADC config
  init_adc();

  // start application timers
  init_app_timers();
  print_dbg("starting event loop.\n\r");

  // keep the blackfin out of reset
  gpio_set_gpio_pin(BFIN_RESET_PIN);  
  dum = 0;

  // event loop  screen_blank_line(0, 1);
  screen_blank_line(0, 2);
  screen_blank_line(0, 3);
  screen_blank_line(0, 4);
  screen_blank_line(0, 5);
  screen_blank_line(0, 6);
  screen_blank_line(0, 7);

  net_init(&ctlnet);
  //  print_dbg("\r\nNET INIT");

  preset_init();
  //  print_dbg("\r\nPRESET INIT");

  scene_init();
  //  print_dbg("\r\nSCENE INIT"); 

  menu_init();
  //  print_dbg("\r\nMENU INIT");
}

/*

// load the default DSP
//load_bfin_sdcard_test();   
files_load_dsp("aleph-osc2.ldr");
// get parameters
print_dbg("\r\nwaiting to read params...");
//    delay_ms(1000);
delay = FCPU_HZ >> 8; while(delay--) {;;}
print_dbg("\r\nwaited.");
//
report_params();
// load scene 0
//...
*/

/// TEST: add a string to the scroll output

static void scroll_event(const char* str, s16 val) {
  /*
    u8 i;
    s8 n;
    u8 x=0;
    print_dbg("\r\n scrolling event: ");
    print_dbg(str);
    strcpy(eventScroll[scrollIdx], str);
    //  itoa_whole(val, eventScroll[scrollIdx] + CHAR_COLS - 6, 5); 
    eventScrollData[scrollIdx] = val;
    eventScrollTimes[scrollIdx] = tcTicks;
    // display
    for(i=0; i<CHAR_ROWS_1; i++) {
    n = scrollIdx - i;
    if(n < 0) { n += CHAR_ROWS; }
    screen_blank_line(0, i);
    screen_int(0, i, (s16)eventScrollTimes[n], 0xf);
    screen_line(60, i, eventScroll[n], 0xf);
    screen_int(CHAR_COLS - 5, i, eventScrollData[n], 0xf);
    screen_hl_line(x, i, n % 4);
    }
    // advance index
    scrollIdx = (scrollIdx + 1) % CHAR_ROWS;
  */
}

void displayAdcVal(u8 idx, u16 val) {
  //  screen_blank_line(0, CHAR_ROWS_1);
  //  screen_int(FONT_CHARW * (5 * idx + 3), CHAR_ROWS_1, val, 1);
  //  screen_hl_line(0, CHAR_ROWS_1, 0xf);
}

/*
  void displayEventLoad(void) {
  u8 x = 0;
  x = screen_line(0, 0, "max event ms: ", 0xf);
  screen_int(x, 0, (int)( (float)maxEventCycles / (float)FMCK_HZ * 1000.f ), 0xf );
  }
*/

/*
static void twi_test(void) {
  static u8 txData[16] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
  u8 txByteIdx = 0;
  u8 chip = 10;
  u8 addr = 170;
  u8 addr_len =1;
  u8 status;
   
  while(1) {
    status = send_master(chip, addr, addr_len, 16, txData);
    txData[txByteIdx]++;
    txByteIdx = (txByteIdx + 1 ) & 0xf;
    print_dbg("\r\bsent 16 bytes on TWI, status: ");
    if(status) { print_dbg("SUCCESS"); } else { print_dbg("FAIL"); }   
  }
}
*/

