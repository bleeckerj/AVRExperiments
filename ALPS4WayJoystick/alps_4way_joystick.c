/* $Id$ */
#include "global.h"
#include "stdint.h"
#include "avrlibdefs.h"
#include "avrlibtypes.h"
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>	// include interrupt support
#include <avr/sleep.h> 
#include "rprintf.h"
#include "buffer.h"
#include "uart2.h"
#include "a2d.h"

static u08 key_down;

int main(void);
void setup(void);



int main(void) {
  u08 adc_1, adc_2, adc_3, adc_4, adc_5;
	u08 i;
  setup();
  while(1==1) {
  //rprintfStr("Hello");

  adc_1 = a2dConvert8bit(ADC_CH_ADC1);
  adc_2 = a2dConvert8bit(ADC_CH_ADC2);
  adc_3 = a2dConvert8bit(ADC_CH_ADC3);
  adc_4 = a2dConvert8bit(ADC_CH_ADC4);
  adc_5 = a2dConvert8bit(ADC_CH_ADC5);
  rprintfu08(adc_1);
 // rprintfu08(adc_2);
 // rprintfu08(adc_3);
 // rprintfu08(adc_4);
 // rprintfu08(adc_5);
  rprintfCRLF();
  _delay_ms(32);
  i++;
  }
  return 0;
}

void setup() {

  uart0Init();
  uartSetBaudRate(0, 9600);
  rprintfInit(uart0SendByte);
  rprintf("$Id$");

  // this goes to the switch common
  sbi(DDRA, DDA0); // output on PA0
  sbi(PORTA, PA0); // take it high

  cbi(DDRA, DDA1); 
  cbi(DDRA, DDA2);
  cbi(DDRA, DDA3);
  cbi(DDRA, DDA4);
  cbi(DDRA, DDA5);

  a2dInit();
  a2dSetReference(ADC_REFERENCE_AVCC);
  a2dSetPrescaler(ADC_PRESCALE_DIV128);

  sbi(DDRB, PB3); // output for the LED
  sbi(PORTB, PB3);
 }
