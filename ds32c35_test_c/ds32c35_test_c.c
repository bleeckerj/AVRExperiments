/****************************************************************************
Title:    Access serial EEPROM 24C02 using I2C interace
Author:   Peter Fleury <pfleury@gmx.ch> http://jump.to/fleury
File:     $Id: ds32c35_test_c.c,v 1.2 2007/09/01 17:50:50 julian Exp $
Software: AVR-GCC 3.3
Hardware: AT90S8515 at 4 Mhz, any AVR device can be used

Description:
    This example shows how the I2C library i2cmaster.S can be used to 
    access a serial eeprom.
    Based on Atmel Application Note AVR300, adapted to AVR-GCC C interface

*****************************************************************************/
#include "global.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>             // contains dtostrf(double..)
#include <avr/io.h>
#include <avr/interrupt.h>	// include interrupt support

#include "i2c.h"
#include "avrlibdefs.h"
#include "avrlibtypes.h"
#include "stdint.h"
#include "uart2.h"
#include "rprintf.h"
#include "vt100.h"
#include "cmdline.h"


#define DS32C35_TWI_ADDR 0x68 // device address
u08 localBuffer[20];
u08 receiveBuffer[20];

int main(void);
void setup(void);
void getTime(void);

int main(void)
{
  setup();

    for(;;) {
      getTime();
      int i;
	  
      for(i=0; i<15; i++) {
	_delay_ms(32);
      }
      cbi(PORTB, PB3);
      for(i=0; i<15; i++) {
	_delay_ms(32);
      }
      sbi(PORTB, PB3);

    }
}

void setup(void)
{
  sbi(DDRB, DDB3);
  sbi(PORTB, PB3);

    uartInit();
    uartSetBaudRate(0, 4800);
    rprintfInit(uart0SendByte);
    vt100Init();
    vt100ClearScreen();
    vt100SetCursorPos(1,0);
    rprintfProgStrM("\r\nWelcome to the DS32C35 Test Suite!\r\n");
    i2cInit();
  localBuffer[0] = 0x45;
  localBuffer[1] = 0x59;
  localBuffer[2] = 0x23;
  localBuffer[3] = 0x1; 
  localBuffer[4] = 0x31;
  localBuffer[5] = 0x11 & 0x80; // century and month
  localBuffer[6] = 0x08;
  i2cMasterSend(0x68, 7, localBuffer);

  
}



void getTime() {
  unsigned char ret;
  /* write ok, read value back from eeprom address 0x05, wait until 
     the device is no longer busy from the previous write operation */
  i2cMasterReceive(0x68, 7, receiveBuffer);

  rprintfProgStrM(" s[");
  rprintf("%d", (receiveBuffer[0] & 0x70)>>4); rprintf("%d", receiveBuffer[0] & 0x0F);
  rprintfProgStrM("]");

  rprintfProgStrM(" m[");
  rprintfu04((receiveBuffer[1] & 0x70)>>4); rprintfu04(receiveBuffer[1] & 0x0F);
  rprintf("]");

  rprintf(" h[");
  rprintfu04((receiveBuffer[2] & 0x30)>>4); rprintfu04(receiveBuffer[2] & 0x0F);
  rprintf("]");

  rprintf(" a[");
  rprintfu08(receiveBuffer[3]);
  rprintf("]");

  rprintf(" d[");
  rprintfu04((receiveBuffer[4] & 0x30)>>4); rprintfu04(receiveBuffer[4] & 0x0F);
  rprintf("]");

  rprintf(" m[");
  rprintfu04((receiveBuffer[5] & 0x10)>>4); rprintfu04(receiveBuffer[5] & 0x0F);
  rprintf("]");

  rprintf(" y[");
  rprintfu04((receiveBuffer[6] & 0xF0)>>4); rprintfu04(receiveBuffer[6] & 0x0F);
  rprintf("]");

  rprintfCRLF();
}
