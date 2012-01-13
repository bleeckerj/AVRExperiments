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

  if(bit_is_set(PINA, PA2) && bit_is_clear(PINA, PA3)) {
    rprintfStr("0"); //rprintfCRLF();
    //	  rprintfCRLF();
	  rprintfChar(0);
  }
  
  if(bit_is_clear(PINA, PA2) && bit_is_set(PINA, PA3)) {
    rprintfStr("1");
      //	  rprintfCRLF();
	  rprintfChar(0);
  }
  if(bit_is_clear(PINA, PA4) && bit_is_clear(PINA, PA5)) {
    rprintfStr("4");
      //	  rprintfCRLF();
	  rprintfChar(0);
  }
  /*
  if(bit_is_clear(PINA, PA2) && bit_is_clear(PINA, PA3)) {
    rprintfStr("00Y"); rprintfCRLF();
  }
  */
  /*
  if(bit_is_set(PINA, PA2) && bit_is_set(PINA, PA3)) {
    rprintfStr("11Y"); rprintfCRLF();
  }
  */

  if(bit_is_clear(PINA, PA0) && bit_is_set(PINA, PA1)) {
    rprintfStr("2"); 
      //      rprintfCRLF();
      rprintfChar(0);      
  }
  if(bit_is_set(PINA, PA0) && bit_is_clear(PINA, PA1)) {
    rprintfStr("3"); //rprintfCRLF();
    //    rprintfCRLF();
    rprintfChar(0);
  }
/*
  if(bit_is_set(PINA, PA0) && bit_is_set(PINA, PA1)) {
    rprintfStr("11X"); rprintfCRLF();
  }
*/
  /*
  if( ((inb(PINA) & (1<<PA2)) == 0) ^
      ((inb(PINA) & (0<<PA3)) == 0) ) {
    
    rprintfStr("1");rprintfCRLF();
  } else
  if( ((inb(PINA) & (1<<PA0)) == 0) ^
      ((inb(PINA) & (0<<PA1)) == 0) ) {
    rprintfStr("4");rprintfCRLF();
  } else
  if( ((inb(PINA) & (0<<PA0)) == 0) ^
      ((inb(PINA) & (1<<PA1)) == 0) ) {
    
    rprintfStr("2"); rprintfCRLF();
  } else
  if( ((inb(PINA) & (1<<PA2)) == 0) ^
      ((inb(PINA) & (1<<PA3)) == 0) ) {
    
    rprintfStr("3"); rprintfCRLF();
  }
  */
  _delay_ms(70);
  sbi(PCICR, PCIE0);
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
  int i;
  i = 0;
  for(i=0; i<30; i++)
    _delay_ms(32);
  setup();
  while(1==1) { 
    i++;
      
    /*
      if(bit_is_clear(PCICR, PCIE2) && bit_is_set(PINC, PINC3)) {
	sbi(PCIFR, PCIF2);
	cbi(PCICR, PCIE2);
      } 
      if(bit_is_clear(PCICR, PCIE2) && bit_is_clear(PINC, PINC3)) {
	//sbi(PCIFR, PCIF2);
	sbi(PCICR, PCIE2);
      }
    */      
    }
  return 0;
}

void setup() {
  uartInit();
  uartSetBaudRate(0, 9600);
  rprintfInit(uart0SendByte);
  rprintf("Hello..\r\n");
  //vt100Init();


  cbi(DDRC, DDC3); // input 

  cbi(DDRA, DDA0); // input
  cbi(DDRA, DDA1); // input
  cbi(DDRA, DDA2); // input
  cbi(DDRA, DDA3); // input
  cbi(DDRA, DDA4);
  
  sbi(DDRA, DDA5); // output connected by select to PA4
  cbi(PORTA, PA5);
  
  sbi(PORTA, PA0); // pull-ups enabled
  sbi(PORTA, PA1);
  sbi(PORTA, PA2);
  sbi(PORTA, PA3);
  sbi(PORTA, PA4);
  cbi(PORTA, PA5);
  

  cbi(DDRD, DDD6); // input
  //cbi(PORTB, PB3);

  // We want PCINT30 enabled so we know when the charger is connected
  //  PCICR |= (1<<PCIE3); // enable pin change interrupt 3 for PCINT31..24. 
//  PCMSK3 |= 0xFF;//(1<<PCINT30); // enable PCINT30

  // arb switch
  PCICR |= (0<<PCIE3) | (0<<PCIE2) | (0<<PCIE1) | (1<<PCIE0);
  //  PCMSK3 |= (1<<PCINT30);
  //  PCMSK3 |= (1<<PCINT26);
  //  PCMSK2 |= (1<<PCINT19);
  PCMSK0 |= (1<<PCINT0) | (1<<PCINT1) | (1<<PCINT2) | (1<<PCINT3) | (1<<PCINT4) | (1<<PCINT5);


  // set up the power pulse LED
  
  TCCR0A |= (1<<WGM00) | (1<<COM0A1); // mode 1, phase-correct PWM
  TCCR0B |= (1<<CS02); // clk/256 from prescaler
  //TCCR0B |= (0<<CS02) | (1<<CS01) | (1<<CS00); // clk/64

  TCNT0 = 0x00;
  sbi(TIMSK0, TOIE0);

  sbi(DDRB, PB3);
  sbi(PORTB, PB3);
/*
  cbi(DDRD, PD6); // configure PD6 as input
  sbi(PORTD, PD6); // turn on pull-up resistor
*/
  sei();
}
