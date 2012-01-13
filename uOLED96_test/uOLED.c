#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "uart2.h"

#include "avrlibdefs.h"
#include "avrlibtypes.h"

#define DISPLAY_RESET PA7
#define TXD1 PD3
#define RXD1 PORTD2

u08 m_MSBColor;
u08 m_LSBColor;


void drawCircle(u08 x, u08 y, u08 r, u08 msbColor, u08 lsbColor) 
{
  uart1SendByte('C');
  uart1SendByte(x);
  uart1SendByte(y);
  uart1SendByte(r);
  uart1SendByte(msbColor);
  uart1SendByte(lsbColor);
  while((uart1GetByte()) == -1);
}


void displayPowerDown()
{
  uart1SendByte('Y');
  uart1SendByte(0x03);
  uart1SendByte(0x00);
  while((uart1GetByte()) == -1);
}

void displayPowerUp()
{
  uart1SendByte('Y');
  uart1SendByte(0x03);
  uart1SendByte(0x01);
  while((uart1GetByte()) == -1);
}

void displayOff()
{
  uart1SendByte('Y');
  uart1SendByte(0x01);
  uart1SendByte(0x00);
  while((uart1GetByte()) == -1);
}

void displayOn()
{
  uart1SendByte('Y');
  uart1SendByte(0x01);
  uart1SendByte(0x01);
  while((uart1GetByte()) == -1);
}

void displaySetContrast(u08 aContrastValue)
{
  uart1SendByte('Y');
  uart1SendByte(0x02);
  uart1SendByte(aContrastValue);
  while((uart1GetByte()) == -1);
}

void delay250()
{
  u08 i;
  for(i=0; i<25; i++) {
    _delay_ms(10);
  }
}

void delay100()
{
  u08 i;
  for(i=0; i<10; i++) {
    _delay_ms(10);
  }
}

void displayRampContrastDown()
{
  s08 i;
  for(i=15; i>-1; i--) {
    displaySetContrast(i);
    delay100();
  }

}

void displayRampContrastUp()
{
  s08 i;
  for(i=0; i<=15; i++) {
    displaySetContrast(i);
    delay100();
  }

}


void setColor(u08 msbColor, u08 lsbColor)
{
  m_MSBColor = msbColor;
  m_LSBColor = lsbColor;
}


void displayPlaceWords(u08 col,  u08 row, u08 msbColor, u08 lsbColor, char aString[])
{

}

void displayPlaceString(u08 col, u08 row, u08 msbColor, u08 lsbColor, char aString[])
{
  u08 i = 0;
  uart1SendByte('s');
  uart1SendByte(col);
  uart1SendByte(row);
  uart1SendByte(0x02);
  uart1SendByte(msbColor);
  uart1SendByte(lsbColor);

  while(aString[i] != 0x00 && i<255) {
    uart1SendByte(aString[i++]);
  }
  uart1SendByte(0x00);
  while((uart1GetByte()) == -1);  
}

void initDisplay(void)
{
  u16 i;
  s08 data;
  sbi(DDRA, PA7); // connected to display reset, output
  sbi(DDRD, DDD3); // txd1 output
  sbi(PORTD, PD3); // txd1 high
  sbi(PORTA, PA7); // reset is active low.. 
  sbi(DDRD, DDD2); // rxd1 output just for now..

  // an LED
  sbi(DDRA, PA0);
  sbi(DDRA, PA1);

  cbi(PORTA, PA0); // on
  sbi(PORTA, PA1); // off

  //  displayOn();
  //  displayPowerUp();

  // 500ms wait
  for(i=0; i<50; i++) {
    _delay_ms(10);
  }

  cbi(DDRD, RXD1); // rxd1 input now

  uartInit();
  uartSetBaudRate(1, 9600);

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
}



int main(void) 
{
  u16 i;
  s08 data;
  initDisplay();

  /*
  if(data == 0x15 || data == 0x06 || data == -1) {
    cbi(PORTA, PA0);
    cbi(PORTA, PA1);
  }
  */

  for(i=0; i<100; i++) {
    _delay_ms(10);
  }

  u08 j = 5;
  u08 k = 5;
  //  uart1SendByte('E'); // erase
  //  while((data = uart1GetByte()) == -1);

  //  u08 msbColor = 0x50;
  while(1==1) {
    _delay_ms(20);



    //    msbColor+=10;
    drawCircle(j+=5, k, 2, 0x50, 0x85);

    /*
    uart1SendByte('C');
    uart1SendByte(j+=5); uart1SendByte(k);
    uart1SendByte(2);
    uart1SendByte(0xF5);
    uart1SendByte(0x85);
    */

    if(j>=90) {
      j=5;
      k+=10;
      if(k>=60) {
      	k=5;
	displayPlaceString(1, 0, 0xFF, 0xA0, "Hey There Old Man");
	/*
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
	*/
	for(i=0; i<200; i++) {
	  _delay_ms(10);
	}
	displayRampContrastDown();
        uart1SendByte('E'); // erase
	while((data = uart1GetByte()) == -1);
	for(i=0; i<200; i++) {
	  _delay_ms(10);
	}
	displayRampContrastUp();
	//displayPowerDown();
      }
    }
  }
}
