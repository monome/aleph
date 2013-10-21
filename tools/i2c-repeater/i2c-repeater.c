// avrdude -p m168 -b 115200 -P usb -c avrispmkII -U lfuse:w:0xff:m -U hfuse:w:0xdf:m -U efuse:w:0x01:m -U flash:w:46.hex:i

#define F_CPU 20000000UL

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include "twi.h"


// i2c addr, can be overwritten by makefile
#ifndef ADDR
#define ADDR 80
#endif


// pins

#define E0_CLK 0x01
#define E1_LD 0x02   
#define E2_SER4 0x04 

#define C0_SER 0x01
#define C1_RCK 0x02
#define C2_SCLK 0x04


// tuning
#define OUTPUT_BUFFER_LENGTH 256
#define LED_REFRESH_RATE 24
//#define INPUT_BUFFER_LENGTH 80


// i2c protocol
#define __QUERY 0
#define __SLEEP 1
#define __WAKE 2

#define __RING_SET 3
#define __RING_ALL 4
#define __RING_RANGE 5
#define __RING_MAP1 6
#define __RING_MAP2 7

// globals
uint8_t i1, i2, i3, i4;
uint8_t n1, n2, n3, n4;
volatile uint8_t output_buffer[OUTPUT_BUFFER_LENGTH];
volatile uint8_t output_write;
volatile uint8_t output_read;
//volatile uint8_t bus_input[INPUT_BUFFER_LENGTH];
//volatile uint8_t bus_input_length;
volatile uint8_t bus_address;
//volatile uint8_t input_new;



volatile uint8_t lights[64];
volatile uint8_t pos, scan;
volatile uint8_t push, push_old;
volatile uint8_t place, place_old;

volatile uint8_t rx_write;
volatile uint8_t rx[256];

uint8_t enc_status[2];

char map[4][4] = { {0,1,-1,0}, {-1,0,0,1}, {1,0,0,-1}, {0,-1,1,0} };

uint8_t intensity[16] = {0,1,2,3,4,5,6,7,8,10,12,15,18,22,27,32};


const uint8_t rx_length[8] = {1,1,1,3,2,4,17,17};


// BUS RECEIVE
// ===============================================================
// ===============================================================
void processRx(uint8_t *data, int index)
{	
	rx_write = 0;
	for(n1=0; n1 < index; n1++) {
		rx[rx_write] = data[n1];
		rx_write++;
	}
}

// BUS TRANSMIT
// ===============================================================
// ===============================================================
void processTx(void)
{
	twi_transmit(rx, 1);
}


int main(void)
{
	uint8_t rx_type;
	uint8_t rx_read;
	
	uint8_t backup[64];
	
	rx_write = rx_read = 0;
	for(i1=0;i1<64;i1++) backup[i1]=0;
	
	DDRB = 0x0;		// input
	DDRC = 0xff;	// output
	DDRD = 0;		// input
	PORTD = 0xff;	// turn on pull-ups
	PORTB = 0xff;
	
	bus_address = ADDR;//16;//ADDR;//(~PIND & 0x0f)*2 + 16;				// get address from jumpers on back of pcb
	
	
	// TCCR0A = 0;
	// TCCR0B = (1<<CS01);// | (1<<CS00);
	// TIMSK0 = (1 << OCIE0A);
	// OCR0A = LED_REFRESH_RATE;
	
	// TCCR1A = 0;
	// TCCR1B = (1<<CS10) | (1<<CS12);
	// TIMSK1 = (1<<TOIE1);

	// encoder int init
	// EIMSK = 1<<PCIE0;
	//PCICR = 1<<PCIE0;		// enable pcint0-7
	//PCMSK0 = 0x07;

	sei();
	
	twi_init();
	twi_setAddress(bus_address);
	twi_attachSlaveRxEvent(processRx);
	twi_attachSlaveTxEvent(processTx);
		
	
	
	while(1) {
		i1++;
	}
		
	return 0;
}