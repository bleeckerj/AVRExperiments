/*
ps2_listener v1.0 - 6/6/2008

Listens to the SPI communications between a PlayStation and Controller.
Reports data via serial either by storing bytes in one large
buffer and then completing one dump when full, or by utilizing a circular
buffer to continuously report data.

Copyright 2008 Curious Inventor, LLC
Creative Commons Attribution-Noncommercial 3.0
http://creativecommons.org/licenses/by-nc/3.0/
- you can modify and share, but please leave a link back to our
site: CuriousInventor.com , and seek permission before using this code
for commercial purposes. 

email: support@CuriousInventor.com or post on our forums

*/

//------------ DEFINES ------------------------------- //

// this is a shortcut for reversing the bit order 
#define bitrev(c)	c = (c & 0x0F) << 4 | (c & 0xF0) >> 4; \
					c = (c & 0x33) << 2 | (c & 0xCC) >> 2; \
					c = (c & 0x55) << 1 | (c & 0xAA) >> 1;





//------------ DEFINES ------------------------------- //

// this is a macro shortcut (this code will replace bitrev(c) at compile time)
// for reversing the bit order 
#define bitrev(c)	c = (c & 0x0F) << 4 | (c & 0xF0) >> 4; \
					c = (c & 0x33) << 2 | (c & 0xCC) >> 2; \
					c = (c & 0x55) << 1 | (c & 0xAA) >> 1;
#define BUFSIZE 240 // can't get to 256 for some reason

//------------ CONFIG ------------------------------- //

// Configure the chip in code instead of through MPLAB's Configure--> Configuration Bits... 
// menu.

// In summary, using an 8MHz crystal oscillator, these settings will yield
// 4 MIPs (Million Instructions per Sec), and disable the brown-out reset and watch-dog timer

// #pragma tells the compiler to write in a specific section of memory, in this case the config section

// FOSC and CPUDIV setup up the clock speed and oscillator settings.
// HS = high speed, more power is needed at higher frequencies
// HS = HSPLL_HS uses the built in PLL (Phase Locked Loop--basically a frequency multiplier)
#pragma config FOSC = HSPLL_HS
#pragma config CPUDIV = OSC4_PLL6  // since we're using PLL, it divides the PLL freq (96MHz) by 6 to get 16 MHz.
// (this is a dual purpose setting, if config FOSC = HS, it would divide that freq by 4)
#pragma config PLLDIV = 2 // The PLL is designed to run at 96MHz from a 4MHz crystal, this scales down our 8 MHz first.
#pragma config BOR = OFF // brownout reset disabled (resets chip when power voltage gets too low)
#pragma config WDT = OFF // watch dog timer OFF (resets chip if a certain bit doesn't get set every so often)








//------------ INCLUDES ------------------------------- //

// loads pic18 family constants and defines, etc.
#include <p18cxxx.h>
// loads 18f4550 specific defines, etc.
#include <p18f4550.h>

#include "main.h"
#include <usart.h>
#include <stdio.h>
#include <portb.h>
#include <delays.h>
#include <string.h>
#include <stdlib.h>
#include <spi.h>




//------------ Prototypes ------------------------------- //

void Initial(void);
void delay_uS (char d);
void delay_mS (short long d);
void test_delay(char, char);
void print_ascii( unsigned char);
void print_ascii_r(unsigned char t, unsigned char t2);
void read_spi_to_buf(char*, unsigned char);
void write_buf_to_usart(char*, unsigned char);
void record_to_buffer_then_dump_to_usart(void);
void continuously_read_spi_and_dump_to_usart(void);





//------------ Global Variables ------------------------------- //

int read_buf_index =0;

// most variables are stored in a memory bank that can't exceed 256 bytes, so larger
// variables need to be placed in their own memory bank with the following commands.
// even thought there may be total memory available, the compiler may not be able
// to fit everything in because the large variable must be contiguous.
// see http://www.firstwiki.net/index.php/Multi-bank_programming
#pragma udata gpr0
	char buf[BUFSIZE]; 
#pragma udata gpr1
	char buf2[BUFSIZE]; 
#pragma udata gpr3
	char buf3[160]; 

#pragma udata 



void print_ascii( unsigned char t) {
	char buf[20];
	bitrev(t);
	sprintf(buf, "%02X", t);
	putsUSART(buf); // replace with USART hardware version

}


void read_spi_to_buf(char *write_buf_p, unsigned char size) {
	unsigned char write_buf_index;	
	// read in BUFSIZE bytes
	write_buf_index = 0;
	while (write_buf_index < size) {
		while(!SSPSTATbits.BF);		
		*write_buf_p = SSPBUF;
		write_buf_index++;
		write_buf_p++;
	}	
}

void write_buf_to_usart(char * read_buf_p, unsigned char size) {
	char * mystring[16];
	unsigned char temp_read_index = 0;
	while (temp_read_index < size) {
		itoa(read_buf_index++, mystring);	
		//itoa(temp_read_index++, mystring);	
		putsUSART(mystring);	
		while(BusyUSART());putcUSART(':');	
		while(BusyUSART());putcUSART(' ');	
		temp_read_index++;
		print_ascii(*read_buf_p);
		putcUSART(0x0D);	
		read_buf_p++;
	}


}


/*-----------------------------------------------*/
/*------------   M A I N   -----------------------*/
/*-----------------------------------------------*/



void main (void) {		

	Initial(); 	
	
	record_to_buffer_then_dump_to_usart();

//	continuously_read_spi_and_dump_to_usart();


  
    while(1);
}


void Initial()
{

	ADCON0 = 0b00110000;
	TRISB = 0b11111111;
	OpenSPI(SLV_SSOFF, MODE_10, SMPMID); // SMPMID --> output sample at mid-point of cycle, not end.
	
	TRISCbits.TRISC7 = 0;	// SDO (SPI Output)	
	LATBbits.LATB2 = 1;  //ATT for SPI)
	
	TRISCbits.TRISC6 = 0; // makes TX an output  

	  
  	
 	// if the hardware USART was being used, the following code would initialize

 	// for sending MIDI out messages, 31250	 
  	OpenUSART( USART_TX_INT_OFF & 
			 USART_RX_INT_OFF &
			 USART_ASYNCH_MODE &
			 USART_EIGHT_BIT &
			 USART_SYNC_MASTER &
			 USART_BRGH_HIGH, 16);	// 7	// 15 is 31250	// 8 for 57600	// 12 for 	38400	  
  	// baud = Fosc / (16 (x + 1)), where x is the last arg to OpenUSART. 
	// 57600 = 16 000 000 / (16 * (x+1))
	// 16 000 000 / 57600 / 16 - 1 = 
    
	// initialize LED ports and set their tris bits
	PORTEbits.RE2 = 1;	  
  	TRISEbits.TRISE2 = 0; 	
}


void record_to_buffer_then_dump_to_usart() {
	char temp;
	
	read_buf_index =0;	
	while(BusyUSART());putcUSART(0x0D); // return
	while(BusyUSART());putcUSART(0x0D);
	while(BusyUSART());putcUSART('S');
	while(BusyUSART());putcUSART('T');
	while(BusyUSART());putcUSART('A');
	while(BusyUSART());putcUSART('R');
	while(BusyUSART());putcUSART('T');
	while(BusyUSART());putcUSART(0x0D);
	
	// re-stating the SPI hardware seems to prevent recording gibberish due to 
	// mis-interpretation of where the beginning of a byte is.
	while(!SSPSTATbits.BF);
	temp = SSPBUF;
	Delay1KTCYx(255); // let the start up weirdness pass	
	CloseSPI();
	OpenSPI(SLV_SSOFF, MODE_10, SMPMID);
	
	LATEbits.LATE2 = 1;
	// burn some bytes, ram is limited, and the first 40 or so bytes
	// when a controller is first connected are all 0x01's.
	while (read_buf_index++ < 50) {
		while(!SSPSTATbits.BF);		
		temp = SSPBUF;
	}		
	// read in (2nd_argument) number of bytes to (first_argument) buffer
	read_spi_to_buf(buf, BUFSIZE);
	read_spi_to_buf(buf2, BUFSIZE);
	read_spi_to_buf(buf3, 150);
	LATEbits.LATE2 = 0; // process indicator LED
	
	// optional delay.  Two PICs were programmed with this code, one recording the 
	// command bus and the other recording the data bus.  This was done since the 
	// PIC didn't have two SPI inputs, and the software SPI cannot act as a slave.
	// One PIC would immediately transmit back it's recording, while the other
	// would be programmed with the delay so that the single RS-232 line back
	// to the computer could be manually switched.
/*	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);
	Delay1KTCYx(255);*/
	LATEbits.LATE2 = 1;
	write_buf_to_usart(buf, BUFSIZE);
	write_buf_to_usart(buf2, BUFSIZE);
	write_buf_to_usart(buf3, 150);
	Delay1KTCYx(100);
	LATEbits.LATE2 = 0;	

	
	while(1);

}


// this function continuously records from the SPI port and writes to the USART line.
// It uses a circular buffer to be able to handle the higher burst rate of the SPI
// bus (play station is 500kb/s), and then transfer to the USART in between bursts.
void continuously_read_spi_and_dump_to_usart() {

	char * read_buf_p;
	char * write_buf_p;
	char * mystring[8]; 
	char temp_count;
	int total_count = 0;
	char temp;		
	unsigned char write_buf_index = 0;
	unsigned char read_buf_index = 0;
	
	TRISB = 0b11111111;
	OpenSPI(SLV_SSOFF, MODE_10, SMPMID); // SMPMID --> output sample at mid-point of cycle, not end.
	

	LATEbits.LATE2 = 1;
	write_buf_p = buf;
	read_buf_p = buf;
	
	while (1) {
		while(!SSPSTATbits.BF) { // read out the buffer to the serial port 
								 // while no data is coming in the SPI bus
			LATEbits.LATE2 = LATEbits.LATE2 ^ 1;
			if (read_buf_index != write_buf_index) { // the reading pointer is behind the	
													 // writing buffer and has some catching
													 // up to do.
				print_ascii(*read_buf_p); // print a byte and format the results into bit-reversed, pretty hex
				putcUSART(0x0D);
				// increment or wrap around the read pointer and index
				if (read_buf_index < BUFSIZE-1) {
					read_buf_index++;
					read_buf_p++;
				}
				else { // reset to beg. of buffer
					read_buf_index = 0;	
	  				read_buf_p = buf;
				}
			}		
		}		
		*write_buf_p = SSPBUF;
		if (write_buf_index < BUFSIZE-1) {
			write_buf_index++;
			write_buf_p++;
		}
		else { // reset to beg. of buffer
			write_buf_index = 0;	
	  		write_buf_p = buf;
		}	
		// small delay to give more spi data a chance to load (hack), should use an interrupt
		temp_count = 0;
		while(!SSPSTATbits.BF && temp_count++ < 10 ) {
			Delay10TCYx(1);				
		}
		if (total_count++ > 500) // comment this out to run continuously
			break;
	}
	LATEbits.LATE2 = 0;
    while(1);

}

