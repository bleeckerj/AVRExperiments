/* $Id$ */
#include "stdint.h"
#include "avrlibdefs.h"
#include "avrlibtypes.h"
//#include <stdio.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>	// include interrupt support
#include <avr/sleep.h> 
#include "rprintf.h"
#include "buffer.h"
#include "uart2.h"


static u08 key_down;

int main(void);
void setup(void);



ISR(SIG_PIN_CHANGE3) {
  // turn off interrupt sensing on these pins
  //cli();
  cbi(PCICR, PCIE3);
  // down is PIND6 / 0x01
  if(bit_is_set(PIND, PIND6) && bit_is_clear(PIND, PIND7)) {
    key_down = 0x01;
    //rprintfu08(0x01);
    //rprintfChar(0x00);
  }
  // up is PIND5 / 0x02
  if(bit_is_set(PIND, PIND5) && bit_is_clear(PIND, PIND7)) {
    //rprintfu08(0x02);
    //rprintfChar(0x00);
  }    
  // pushed in is 0x10
  if(bit_is_set(PIND, PIND6) && bit_is_set(PIND, PIND7)) {
    //rprintfu08(0x10);
    //rprintfChar(0x00);
  }
  // pushed in and down 0x11
  if(bit_is_set(PIND, PIND6) && bit_is_set(PIND, PIND7)) {
    //rprintfu08(0x11);
    //rprintfChar(0x00);
  }
  // pushed in and up 0x12
  if(bit_is_set(PIND, PIND6) && bit_is_clear(PIND, PIND7)) {
    //rprintfu08(0x12);
    //rprintfChar(0x00);
  }  
  //sbi(PCICR, PCIE3);
  //sei();
}

int main(void) {
  u08 i;
  setup();
  while(1==1) { 
    if(bit_is_clear(PIND, PIND5)) {
      sbi(PORTB, PB3);
      rprintfChar('1'); rprintfCRLF();
	  rprintfChar(0);
    }  else
    if(bit_is_clear(PIND, PIND6)) {
      sbi(PORTB, PB3);
      rprintfChar('2'); rprintfCRLF();
	  rprintfChar(0);
    } else
    if(bit_is_clear(PIND, PIND7)) {
      sbi(PORTB, PB3);
      rprintfChar('3'); rprintfCRLF();
	  rprintfChar(0);
    } else
      {
	cbi(PORTB, PB3);
      }
  }
   return 0;
}

void setup() {
  uartInit();
  uartSetBaudRate(0, 9600);
  rprintfInit(uart0SendByte);
  //rprintf("$Id$");

  // this goes to the switch common
  sbi(DDRD, PD4); // output on PD4
  cbi(PORTD, PD4); // take it low


  // rocker
  cbi(DDRD, PD5); // input on PD5
  sbi(PORTD, PD5); // tri-state the input

  // rocker
  cbi(DDRD, PD6);
  sbi(PORTD, PD6); // tri-state the input

  // rocker press
  cbi(DDRD, PD7);
  sbi(PORTD, PD7);

  sbi(DDRB, PB3); // output for the LED
  sbi(PORTB, PB3);
 }
