#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "avrlibdefs.h"
#include "avrlibtypes.h"

/* define CPU frequency in Mhz here if not defined in Makefile */

static u08 mPwrLedLevel_R_Fixed = 255;
static u08 mPwrLedLevel_G_Fixed = 128;
static u08 mPwrLedLevel_B_Fixed = 68;

static u08 mPwrLedLevel_R = 0;
static u08 mPwrLedLevel_G = 0;
static u08 mPwrLedLevel_B = 0;

static void incr_pwr_led(void);

ISR(SIG_OVERFLOW0) {
  incr_pwr_led();
}


static char aDirection;
static void
pwr_up_led(void)
{
  if(mPwrLedLevel_R < mPwrLedLevel_R_Fixed) {
    mPwrLedLevel_R++;
  }
  if(mPwrLedLevel_G < mPwrLedLevel_G_Fixed) {
    mPwrLedLevel_G++;
  }
  if(mPwrLedLevel_B < mPwrLedLevel_B_Fixed) {
    mPwrLedLevel_B++;
  }

  OCR0A = mPwrLedLevel_R; // PB0
  OCR0B = mPwrLedLevel_G; // PB1
  OCR1B = mPwrLedLevel_B; // PB4
}


int main(void)
{
  // make PB3 an input
  cbi(DDRB, PB3);
  PORTB |= (1<<PORTB3);
   
  for(int j=0; j<30; j++)
    _delay_ms(32);
  cbi(PORTB, PB3);
  for(int j=0; j<30; j++)
    _delay_ms(32);
  sbi(PORTB, PB3);
  for(int j=0; j<30; j++)
    _delay_ms(32);

  // set up the power pulse LED
  TCCR0A |= (1<<WGM00) | (1<<COM0A1); // mode 1, phase-correct PWM
  TCCR0B |= (0<<CS00) | (1<<CS01) | (0<<CS02); // clk/64 from prescaler
  //TCCR0B |= (1<<CS00) | (1<<CS01); // clk/64 from prescaler
  //TCCR0B |= (1<<CS02); // clk/256 from prescaler

  TCNT0L = 0x00;
  
  //sbi(TIMSK0, TOIE0);
  //sbi(TIMSK, TOIE0);
  sei();

  OCR0A = mPwrLedLevel_R;

  while(1==1) {
    if(bit_is_set(PORTB, PB3)) {
      mPwrLedLevel_R = 0;      
  }

}

