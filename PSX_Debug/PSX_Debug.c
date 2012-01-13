#define F_CPU 16000000
#include "avrlibdefs.h"
#include "avrlibtypes.h"
#include <stdio.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>	// include interrupt support
#include <avr/sleep.h> 
#include "PSX_Debug.h"

/*
ISR(SIG_PIN_CHANGE1)
{
  cli();
  if(bit_is_set(PINA, PA1)) {
    cbi(PORTA, PA1); 
	} else {
	sbi(PORTA, PA1);
	}
   sei();
}
*/

int main() {
   int i;
   i=10;
     int u;
  //DDRA = 0xFF; // output
  //PORTA = 0xFF;
  //sbi(PORTA, PA1);
  //cbi(DDRB, PB4); // input
  u=10;
  // We want PCINT30 enabled so we know when the charger is connected
  //  PCICR |= (1<<PCIE3); // enable pin change interrupt 3 for PCINT31..24. 
  //  PCMSK3 |= 0xFF;//(1<<PCINT30); // enable PCINT30

  // arb switch
  PCICR |= (0<<PCIE3) | (0<<PCIE2) | (1<<PCIE1) | (0<<PCIE0);

  PCMSK1 |= (1<<PCINT12);

  //sei();
   setup();
  while(1) {
     i++;
  }
  return 0;
}

void setup() {
  int u;
  DDRA = 0xFF; // output
  PORTA = 0xFF;
  sbi(PORTA, PA1);
  cbi(DDRB, PB4); // input
  u=10;
  // We want PCINT30 enabled so we know when the charger is connected
  //  PCICR |= (1<<PCIE3); // enable pin change interrupt 3 for PCINT31..24. 
  //  PCMSK3 |= 0xFF;//(1<<PCINT30); // enable PCINT30

  // arb switch
  PCICR |= (0<<PCIE3) | (0<<PCIE2) | (1<<PCIE1) | (0<<PCIE0);

  PCMSK1 |= (1<<PCINT12);

  //sei();

}
