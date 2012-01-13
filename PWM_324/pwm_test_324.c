#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "avrlibdefs.h"
#include "avrlibtypes.h"

/* define CPU frequency in Mhz here if not defined in Makefile */

static char mPwrLedLevel = 0;
static void incr_pwr_led(void);

ISR(SIG_OVERFLOW0) {
  incr_pwr_led();
}


static char aDirection;
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
  //  printf("[[%d]]\n", mPwrLedLevel);

  if(aDirection == 1) mPwrLedLevel++;
  if(aDirection == 0) mPwrLedLevel--;

  OCR0A = mPwrLedLevel;
}


int main(void)
{
  sbi(DDRB, PB3);
  PORTB |= (1<<PORTB3);
   
  for(int j=0; j<30; j++)
    _delay_ms(32);
  cbi(PORTB, PB3);
  for(int j=0; j<30; j++)
    _delay_ms(32);
  sbi(PORTB, PB3);
  for(int j=0; j<30; j++)
    _delay_ms(32);

  cbi(PORTB, PB3);

  for(int j=0; j<50; j++) {
    _delay_ms(32);
  }

  // set up the power pulse LED
  TCCR0A |= (1<<WGM00) | (1<<COM0A1); // mode 1, phase-correct PWM
  //TCCR0B |= (0<<CS00) | (1<<CS01) | (0<<CS02); // clk/64 from prescaler
  TCCR0B |= (1<<CS00) | (1<<CS01); // clk/64 from prescaler
  //TCCR0B |= (1<<CS02); // clk/256 from prescaler
  //  TCCR0B |= (1<<CS02) | (1<<CS00); cbi(TCCR0B, CS01);


  TCNT0 = 0x00;
  sbi(TIMSK0, TOIE0);
  //sbi(TIMSK, TOIE0);
  sei();

  int j=0x00;
  while(1==1) {
    if(j==0) {
      j++;
    } else {
      j--;
    }
  }

}
