//#include "WProgram.h"
#include "avrlibdefs.h"
#include "avrlibtypes.h"
#include "stdint.h"
#include <stdio.h>
#include <stdlib.h>             // contains dtostrf(double..)

//#include "Flavonoid.h"

#include "Wire.h"
//#include "Serial.h"
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>	// include interrupt support
#include <math.h>
//#include "timer.h"

#ifdef __cplusplus
extern "C"{
#include "uart2.h"
#include "rprintf.h"
#include "vt100.h"
#include "cmdline.h"

} //extern "C"
#endif

#define DS32C35_TWI_ADDR 0x68
#define DS32C35_FRAM_TWI_ADDR 0x50

void setup(void);
static void delay_1s(void);
void getRTCTime(void);
void getFRAM(void);
void setFRAM(void);

static void
delay_1s(void)
{
  uint8_t i;
  for (i = 0; i < 100; i++)
    _delay_ms(10);
}


double convert(unsigned short adc_value) {
  return (3.3*(float)adc_value)/1024.0;
}

int main() {
  sbi(DDRB, PB3);
  cbi(PORTB, PB3);
  delay_1s();
  setup();
  sbi(PORTB, PB3);
  //  setFRAM();
  while(1==1) {
    cbi(PORTB, PB3);
    getRTCTime();
    //setFRAM();
    //getFRAM();
    sbi(PORTB, PB3);
    delay_1s();
    cbi(PORTB, PB3);
    delay_1s();
  }
}


void setup()
{
  uartInit();
  uartSetBaudRate(0, 4800);
  rprintfInit(uart0SendByte);
  vt100Init();
  vt100ClearScreen();
  vt100SetCursorPos(1,0);
  rprintf("Hello UART0\r\n");
  rprintf("\r\nWelcome to the DS32C35 RTC CPP Test\r\n");

  Wire.begin(); // join i2c bus (address optional for master)

  rprintf("\r\nStart Wire up\r\n");
  sbi(DDRB, PB3); // PB3 output
  sbi(PORTB, PB3); // PB3 high, on v04 is pwr led


  /* set the clock */



  rprintf("\r\nWire up\r\n");
  Wire.beginTransmission(DS32C35_TWI_ADDR);
  Wire.send(0x00);
  Wire.send(0x45); // seconds
  Wire.send(0x18); // minutes
  Wire.send(0x18); // hours
  Wire.send(0x06); // day of week
  Wire.send(0x05); // date
  Wire.send(0x04); // month
  Wire.send(0x07); // year
  Wire.endTransmission();

  rprintf("\r\nClock Set\r\n");

  while(1==1) {
    getRTCTime();
  }

  }
}


void getFRAM() {
  Wire.beginTransmission(DS32C35_FRAM_TWI_ADDR);
  Wire.send(0x00);// | 0x80); 
  Wire.send(0x00);// | 0x80); 
  Wire.endTransmission();
  Wire.requestFrom(DS32C35_FRAM_TWI_ADDR, 7);
  s08 i;
  while(Wire.available())
    {
      i = Wire.receive();
      rprintfu08(i); rprintfStr(" ");
      rprintfCRLF();
  }
}

void setFRAM() {
  Wire.beginTransmission(DS32C35_FRAM_TWI_ADDR);
  Wire.send(0x00);// | 0x80); 
  Wire.send(0x00);// | 0x80); 
  Wire.send(0xB0);
  Wire.send(0xAF);
  Wire.send(0x0B);
  Wire.endTransmission();
}

void getRTCTime() {
  u08 z_val = 0x00;
  Wire.beginTransmission(DS32C35_TWI_ADDR);
  Wire.send(0x00);// | 0x80); 
  Wire.endTransmission();
  // Now do a transfer reading one byte from the DS1340
  // This data will be the contents of register 0x00
  Wire.requestFrom(DS32C35_TWI_ADDR, 1);

  if(Wire.available())
    {
      z_val = Wire.receive();
    }

  rprintfProgStrM(" s[");
  rprintf("%d", (z_val & 0x70)>>4); rprintf("%d", z_val & 0x0F);
  rprintfProgStrM("]");

  if((z_val & 0x10) == 0x10) {
    sbi(PORTB, PB3);
  } else {
    cbi(PORTB, PB3);
  }
  //  bcd_time->second = z_val & 0x7F;


  Wire.beginTransmission(DS32C35_TWI_ADDR);
  Wire.send(0x01);
  Wire.endTransmission();
  Wire.requestFrom(DS32C35_TWI_ADDR, 1);

  // minutes
  if(Wire.available())
    {
      z_val = Wire.receive();
    }


  rprintfProgStrM(" m[");
  rprintfu04((z_val & 0x70)>>4); rprintfu04(z_val & 0x0F);
  rprintf("]");

  //  bcd_time->minute = z_val & 0x7F;

  /*
  // ten minutes
  if(bcd_time->minute & 0x10) {
    sbi(PORTD, PA5);
  } else {
    cbi(PORTD, PA5);
  }
  */

  Wire.beginTransmission(DS32C35_TWI_ADDR);
  Wire.send(0x02);// | 0x80); 
  Wire.endTransmission();

  Wire.requestFrom(DS32C35_TWI_ADDR, 5);
  if(Wire.available())
    {
      z_val = Wire.receive();

      rprintf(" h[");
      rprintfu04((z_val & 0x30)>>4); rprintfu04(z_val & 0x0F);
      rprintf("]");

      //bcd_time->hour = z_val & 0x3F;
    }

  // day
  if(Wire.available())
    {
      z_val = Wire.receive();

      rprintf(" a[");
      rprintfu08(z_val);
      //rprintfu04(z_val & 0x07);
      //bcd_time->day = z_val & 0x07;
    }

  // date
  if(Wire.available())
    {
      z_val = Wire.receive();

      rprintf(" d[");
      rprintfu04((z_val & 0x30)>>4); rprintfu04(z_val & 0x0F);
      //printf("%d", (z_val & 0x30)>>4); printf("%d", z_val & 0x0F);
      rprintf("]");
      //bcd_time->date = z_val & 0x3F;
    }

  // month
  if(Wire.available())
    {
      z_val = Wire.receive();
      rprintf(" m[");
      rprintfu04((z_val & 0x10)>>4); rprintfu04(z_val & 0x0F);
      //printf("%d", (z_val & 0x10)>>4); printf("%d", z_val & 0x0F);
      rprintf("]");
      //bcd_time->month = z_val & 0x1F;
    }

  // year
  if(Wire.available())
    {
      z_val = Wire.receive();
      rprintf(" y[");
      rprintfu04((z_val & 0xF0)>>4); rprintfu04(z_val & 0x0F);
      rprintf("]");
      //bcd_time->year = z_val;
    }
  //printf("\n\r");
  //  return bcd_time;
  rprintfCRLF();
}

