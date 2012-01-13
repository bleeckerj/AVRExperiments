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

ISR(SIG_PIN_CHANGE0) {
  cbi(PCICR, PCIE0);

  sbi(PCICR, PCIE0);
}


ISR(SIG_PIN_CHANGE3) {
  // turn off interrupt sensing on this pin
  cbi(PCICR, PCIE2);

  sbi(PCICR, PCIE2);
}


int main() {
  int i;
  i = 0;
  for(i=0; i<30; i++)
    _delay_ms(32);
  setup();
  while(1==1) { 
    if(bit_is_set(PIND, PD6)) {
		i++;
     } 
	if(bit_is_clear(PIND, PD6)) {
		i++;
	}
    }
  return 0;
}

void setup() {
  uartInit();
  uartSetBaudRate(0, 9600);
  rprintfInit(uart0SendByte);
  rprintf("Hello..\r\n");
  //vt100Init();
  sbi(PORTB, PB3);
  sbi(PORTB, PB3);
  
  PCICR |= (1<<PCIE3) | (1<<PCIE0);
  PCMSK3 |= (1<<PCINT28) | (1<<PCINT29) | (1<<PCINT30) | (1<<PCINT31);
  PCMSK0 |= (1<<PCINT0) | (1<<PCINT1) | (1<<PCINT2) | (1<<PCINT3);


  sbi(DDRD, PD5); // output
  sbi(PORTD, PD5); // pull high

  cbi(DDRD, PD6); // input
  cbi(PORTD, PD6); // Hi-Z
  sei();
}
