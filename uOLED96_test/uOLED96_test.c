#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "uart2.h"
#include "rprintf.h"

#include "avrlibdefs.h"
#include "avrlibtypes.h"


int main(void) 
{
  s08 data;
  u16 i;
  sbi(DDRA, PA7); // connected to display reset, output
  sbi(DDRD, PD3); // txd1 output
  sbi(PORTD, PD3); // txd1 high
  sbi(PORTA, PA7); // reset is active low.. 
  sbi(DDRD, PD2); // rxd1 output just for now..

  // an LED
  sbi(DDRA, PA0);
  sbi(DDRA, PA1);

  cbi(PORTA, PA0); // on
  sbi(PORTA, PA1); // off

  // 500ms wait
  for(i=0; i<50; i++) {
    _delay_ms(10);
  }

  cbi(DDRD, PD2); // rxd1 input now

  uartInit();
  //  uartSetBaudRate(0, 4800);
  uartSetBaudRate(1, 4800);
  //  rprintfInit(uart0SendByte);

  // 1 second

  for(i=0; i<100; i++) {
    _delay_ms(10);
  }


  sbi(PORTA, PA0); // off
  cbi(PORTA, PA1); // on


  uart1SendByte('U'); // lock on a capital U
  while(uart1GetByte() == -1);
  uart1SendByte(0x45);
  while(uart1GetByte() == -1);

  uart1SendByte(0x42);
  uart1SendByte(0xAA);
  uart1SendByte(0x80);
  while((data = uart1GetByte()) == -1);
    //  data = uart1GetByte();
  //  rprintf("%x", data);

  if(data == 0x15 || data == 0x06 || data == -1) {
    cbi(PORTA, PA0);
    cbi(PORTA, PA1);
  }

  for(i=0; i<100; i++) {
    _delay_ms(10);
  }

  /*
  cbi(PORTD, PD7);
  _delay_ms(20);
  sbi(PORTD, PD7);
  _delay_ms(20);
  */

  u08 j = 5;
  u08 k = 5;
  uart1SendByte(0x43); // erase
  while((data = uart1GetByte()) == -1);

  while(1==1) {

//        uart1SendByte('E'); // erase
//        while((data = uart1GetByte()) == -1);

    _delay_ms(20);
    /*
    uart1SendByte(0x67);
    uart1SendByte(0x03);
    uart1SendByte(j+=5);     uart1SendByte(k);
    uart1SendByte(k);     uart1SendByte(j);
    uart1SendByte(50);     uart1SendByte(50);
    */


    uart1SendByte('C');
    uart1SendByte(j+=5); uart1SendByte(k);
    uart1SendByte(2);
    uart1SendByte(0xF5);
    uart1SendByte(0x85);

    while((data = uart1GetByte()) == -1);

    // 7B 97 E0

    //  for(i=0; i<5; i++) {
    //    _delay_ms(10);
    //  }


  //  rprintf("%x ", uart1GetByte());
  /*
  uart1SendByte(7); // col
  //rprintf("%x ", uart1GetByte());
  uart1SendByte(j++); // row
  //rprintf("%x", uart1GetByte());
  uart1SendByte(0x02);
  //rprintf("%x", uart1GetByte());
  uart1SendByte(0xAA);
  //rprintf("%x", uart1GetByte());
  uart1SendByte(0xFF);
  //rprintf("%x", uart1GetByte());
  uart1SendByte('S');
  //rprintf("%x", uart1GetByte());
  uart1SendByte('L');
  //rprintf("%x", uart1GetByte());
  uart1SendByte('O');
  //rprintf("%x", uart1GetByte());
  uart1SendByte('W');
  //rprintf("%x", uart1GetByte());
  uart1SendByte(0x00);
  //rprintf("%x", uart1GetByte());
  */


    if(j>=90) {
      j=5;
      k+=10;
      if(k>=60) {
      	k=5;
	uart1SendByte('S');
	uart1SendByte(10); uart1SendByte(10);
	uart1SendByte(0x00); // 5x7 font
	uart1SendByte(0xFF); uart1SendByte(0xFF);
	uart1SendByte(2); uart1SendByte(2);
	uart1SendByte('H');
	uart1SendByte('e');
	uart1SendByte('l');
	uart1SendByte('l');
	uart1SendByte('o');
	uart1SendByte(0x00);
	while((data = uart1GetByte()) == -1);
	for(i=0; i<200; i++) {
	  _delay_ms(10);
	}
        uart1SendByte('E'); // erase
	while((data = uart1GetByte()) == -1);
	for(i=0; i<200; i++) {
	  _delay_ms(10);
	}
      }
    }
  }
}
