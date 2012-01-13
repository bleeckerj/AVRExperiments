/* $Id: psp_joystick.c,v 1.1.1.1 2007/08/21 05:14:23 julian Exp $ */
#define F_CPU 8000000
#include "stdint.h"
#include "avrlibdefs.h"
#include "avrlibtypes.h"
#include <stdio.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>	// include interrupt support
#include <avr/sleep.h> 
#include "rprintf.h"
#include "buffer.h"
#include "uart2.h"
#include "vt100.h"
#include "a2d.h"

static u08 mPwrLedLevel = 0;
static u08 mAsleep = FALSE;

static void incr_pwr_led(void);

ISR(SIG_OVERFLOW0) {
  incr_pwr_led();
}


static char aDirection;

int main(void);
void setup(void);

static void
incr_pwr_led(void)
{
  //TIMSK = 0x03;

  if(mPwrLedLevel == 0x10) {
    aDirection = 1;
  } 
  if(mPwrLedLevel == 0xF0) {
    aDirection = 0;
  }
  if(aDirection == 1) mPwrLedLevel++;
  if(aDirection == 0) mPwrLedLevel--;

  OCR0A = mPwrLedLevel;
}

ISR(SIG_PIN_CHANGE3) {
  // turn off interrupt sensing on this pin
  cli();
  cbi(PCICR, PCIE2);
  if(bit_is_clear(PIND, PIND2)) {
    if(bit_is_set(TIMSK0, TOIE0)) {
      sleep_enable();

      //      cbi(PORTB, PB3);
      TCCR0A &= (0<<WGM00) | (0<<COM0A0) | (0<<COM0A1);
      //      cbi(PORTB, PB3);      
      cbi(TIMSK0, TOIE0); // disable PWM
      cbi(PORTB, PB3);      
      //cbi(DDRB, PB3);
      sei();
	  mAsleep = TRUE;
      set_sleep_mode(SLEEP_MODE_PWR_DOWN);
      sleep_mode();
      // this is where we wake back up
	  while(bit_is_set(PIND, PIND2)) 
	  	sleep_mode();
      sleep_disable();
      //sbi(DDRB, PB3);
      TCCR0A |= (1<<WGM00) | (1<<COM0A1); // mode 1, phase-correct PWM
      sbi(TIMSK0, TOIE0); // re-enable PWM
     // sbi(PORTB, PB3);
    }
  }
  sbi(PCICR, PCIE2);
  sei();
}


int main() {
  u08 i;
  i = 0;
  u08 adc_0, adc_1;
  setup();
  while(1==1) { 
    adc_0 = a2dConvert8bit(ADC_CH_ADC0);
    adc_1 = a2dConvert8bit(ADC_CH_ADC1);
    rprintfu08(adc_0);
    rprintfu08(adc_1);   
    rprintfChar(0);
    //  rprintfCRLF();

	_delay_ms(32);
/*
	i++;
	if(i<=128) {
		sbi(PORTB, PB3);
	}
	if(i>128) {
		cbi(PORTB, PB3);
	}
*/
	//_delay_ms(32);
	//_delay_ms(32);
	//_delay_ms(32);
	
  }
  return 0;
}

void setup() {
  uart0Init();
  uartSetBaudRate(0, 9600);
  rprintfInit(uart0SendByte);
  //rprintf("Hello.."); rprintfCRLF();
  rprintf("$Id: psp_joystick.c,v 1.1.1.1 2007/08/21 05:14:23 julian Exp $");
  vt100Init();

  sbi(DDRD, DDD4); // output PD4
  sbi(PORTD, PD4); // set it high
  cbi(DDRD, DDD5); // input PD5
  sbi(PORTD, PD5); // set it low

  cbi(DDRA, DDA0); // input (one axis)
  cbi(DDRA, DDA1); // input (the other axis)
  //cbi(DDRA, DDA2); // input
  //sbi(DDRA, DDA3); // output
  //sbi(PORTA, PA0);
  //sbi(PINA, PINA0); // set high
  //sbi(PINA, PA3);
  a2dInit();

  // set up the power pulse LED
  TCCR0A |= (1<<WGM00) | (1<<COM0A1); // mode 1, phase-correct PWM
  TCCR0B |= (1<<CS02); // clk/256 from prescaler
  //TCCR0B |= (0<<CS02) | (1<<CS01) | (1<<CS00); // clk/64

  // set up and enable the power pulse thing
  TCNT0 = 0x00;
  sbi(TIMSK0, TOIE0);

  sbi(DDRB, DDB3); // output on the LED
  sbi(PORTB, PB3); // turn it on
  u08 i, j;
  for(j=0; j<1; j++) {
  for(i=0; i<10; i++) {
  	_delay_ms(32);
  }
  cbi(PORTB, PB3);
  for(i=0; i<10; i++) {
  	_delay_ms(32);
  }
  sbi(PORTB, PB3);
  for(i=0; i<10; i++) {
	  _delay_ms(32);
  }
  }
  cbi(PORTB, PB3);
  sei();
}
