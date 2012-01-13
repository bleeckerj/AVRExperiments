#include "avrlibdefs.h"
#include "avrlibtypes.h"
#include <stdio.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>	// include interrupt support
#include "uart2.h"
#include "rprintf.h"
#include "vt100.h"
#include "cmdline.h"
#include "a2d.h"

u08 Run;
// functions
void goCmdline(void);
void exitFunction(void);
void helpFunction(void);
void dumpArgsStr(void);
void dumpArgsInt(void);
void dumpArgsHex(void);
int main(void);
void setup(void);
static void incr_pwr_led(void);

static u08 mPwrLedLevel = 0;
static u08 mLEDOn = 1;

ISR(SIG_OVERFLOW0) {
  incr_pwr_led();
}

ISR(TIMER0_COMPA_vect) {
  //  incr_pwr_led();
}

ISR(SIG_INTERRUPT0) {
  mPwrLedLevel = 0x00;
}

ISR(SIG_PIN_CHANGE0) {
  mPwrLedLevel = 0x00;
}

ISR(SIG_PIN_CHANGE1) {
  mPwrLedLevel = 0x00;
}

ISR(SIG_PIN_CHANGE2) {
  // turn off interrupt sensing on this pin
  cbi(PCICR, PCIE2);
  //  if(bit_is_clear(PORTB, PB3)) {
  //    sbi(PORTB, PB3);
  //  } else {
  //  TCCR0A |= (0<<COM0A0) | (0<<COM0A1); // turn off pulse LED
  cbi(PORTB, PB3);

  // if the switch is "down" clear the LED

  if(bit_is_set(PORTC, PC3)) {
    cbi(PORTB, PB3);
  } else {
    // if the interrupt was caused by the switch going up, turn it on
    sbi(PORTB, PB3);
  }

  sei();
}

ISR(SIG_PIN_CHANGE3) {
  mPwrLedLevel = 0x00;
  /*
  PORTB |= (1<<PB3);
  //  TCCR0B |= (0<<CS02) | (1<<CS01) | (0<<CS00);
  if(bit_is_set(PORTD, PD6)) {
    TCCR0A |= (1<<WGM00) | (0<<COM0A0) | (1<<COM0A1); // turn on pulse LED
  } else {
    TCCR0A |= (0<<COM0A0) | (0<<COM0A1); // turn off pulse LED
    PORTB |= (0<<PB3);
  }
  */
}

static char aDirection;
static void
incr_pwr_led(void)
{
  //TIMSK = 0x03;
  sbi(PCIFR, PCIF2);
  if(mPwrLedLevel < 0x20) {
    aDirection = 1;
  } 
  if(mPwrLedLevel > 0xE0) {
    aDirection = 0;

    sbi(PCICR, PCIE2);
  }
  //  printf("[[%d]]\n", mPwrLedLevel);

  if(aDirection == 1) mPwrLedLevel++;
  if(aDirection == 0) mPwrLedLevel--;

  OCR0A = mPwrLedLevel;
}


int main() {
  setup();
  goCmdline();
  return 0;
}

void setup() {
  EICRA |= (0<ISC01) | (1<<ISC00);
  EIMSK |= (0<<INT2) | (0<<INT1) | (1<<INT0);

  cbi(DDRD, DDD6); // input on the charge annunciator
  cbi(DDRC, DDC3); // input 
  cbi(DDRA, DDA3); // input
  //  cbi(PORTA, PA1); // low

  // We want PCINT30 enabled so we know when the charger is connected
  //  PCICR |= (1<<PCIE3); // enable pin change interrupt 3 for PCINT31..24. 
//  PCMSK3 |= 0xFF;//(1<<PCINT30); // enable PCINT30

  // arb switch
  PCICR |= (0<<PCIE3) | (1<<PCIE2) | (0<<PCIE1) | (1<<PCIE0);
  PCMSK2 |= (1<<PCINT19);
  PCMSK0 |= (1<<PCINT3);

  // set up the power pulse LED
  TCCR0A |= (1<<WGM00) | (1<<COM0A1); // mode 1, phase-correct PWM
  TCCR0B |= (1<<CS02); // clk/256 from prescaler
  ////TCCR0B |= (0<<CS02) | (1<<CS01) | (1<<CS00); // clk/64

  TCNT0 = 0x00;
  sbi(TIMSK0, TOIE0);

  uartInit();
  uartSetBaudRate(0, 9600);

  rprintfInit(uart0SendByte);
  rprintf("Hiddy Hi UART0\r\n");

  vt100Init();

  sbi(DDRB, PB3);
  sbi(PORTB, PB3);

  a2dInit();

  cbi(DDRA, 0); // x
  cbi(DDRA, 1); // y
  cbi(DDRA, 2); // z
  sbi(DDRA, 3); // use PA3 as an output
  cbi(DDRA, 7); // qt113_out is an input to the mega32

  cbi(DDRB, 4); // use PB4 as an input
  

  a2dSetPrescaler(ADC_PRESCALE_DIV128);

  
  sei();
}

void goCmdline(void)
{
	u08 c;

	// print welcome message
	vt100ClearScreen();
	vt100SetCursorPos(1,0);
	rprintfProgStrM("\r\nWelcome to the Command Line Test Suite!\r\n");

	// initialize cmdline system
	cmdlineInit();

	// direct cmdline output to uart (serial port)
	cmdlineSetOutputFunc(uart0SendByte);

	// add commands to the command database
	cmdlineAddCommand("exit",		exitFunction);
	cmdlineAddCommand("help",		helpFunction);
	cmdlineAddCommand("a",	dumpArgsStr);
	cmdlineAddCommand("b",	dumpArgsInt);
	cmdlineAddCommand("c",	dumpArgsHex);

	// send a CR to cmdline input to stimulate a prompt
	cmdlineInputFunc('\r');

	// set state to run
	Run = TRUE;

	// main loop
	while(Run)
	  {
	      // pass characters received on the uart (serial port)
	      // into the cmdline processor
	      while(uartReceiveByte(0, &c)) cmdlineInputFunc(c);
	      // run the cmdline execution functions
	      cmdlineMainLoop();
	  }

	rprintfCRLF();
	rprintf("Exited program!\r\n");
}

void exitFunction(void)
{
	// to exit, we set Run to FALSE
	Run = FALSE;
}

void helpFunction(void)
{
	rprintfCRLF();

	rprintf("Available commands are:\r\n");
	rprintf("help      - displays available commands\r\n");
	rprintf("dumpargs1 - dumps command arguments as strings\r\n");
	rprintf("dumpargs2 - dumps command arguments as decimal integers\r\n");
	rprintf("dumpargs3 - dumps command arguments as hex integers\r\n");

	rprintfCRLF();
}

void dumpArgsStr(void)
{
  u08 k_x_1, k_y_1, k_z_1;
	rprintfCRLF();
	rprintf("Dump arguments as strings\r\n");

	rprintfProgStrM("Arg0: "); rprintfStr(cmdlineGetArgStr(0)); rprintfCRLF();
	rprintfProgStrM("Arg1: "); rprintfStr(cmdlineGetArgStr(1)); rprintfCRLF();
	rprintfProgStrM("Arg2: "); rprintfStr(cmdlineGetArgStr(2)); rprintfCRLF();
	rprintfProgStrM("Arg3: "); rprintfStr(cmdlineGetArgStr(3)); rprintfCRLF();
	rprintfCRLF();

    k_x_1 = a2dConvert10bit(ADC_CH_ADC0);
    k_y_1 = a2dConvert10bit(ADC_CH_ADC1);
    k_z_1 = a2dConvert10bit(ADC_CH_ADC2);

    rprintfNum(16, 4, FALSE, 'x', k_x_1); 	rprintfCRLF();
    rprintfNum(16, 4, FALSE, 'x', k_y_1);	rprintfCRLF();
    rprintfNum(16, 4, FALSE, 'x', k_z_1);	rprintfCRLF();

    rprintfNum(16, 4, FALSE, ' ', PCICR);	rprintfCRLF();
}

void dumpArgsInt(void)
{
	rprintfCRLF();
	rprintf("Dump arguments as integers\r\n");

	// printf %d will work but only if your numbers are less than 16-bit values
	rprintf("Arg1 as int: %d\r\n", cmdlineGetArgInt(1));
	rprintf("Arg2 as int: %d\r\n", cmdlineGetArgInt(2));
	rprintf("Arg3 as int: %d\r\n", cmdlineGetArgInt(3));

	// printfNum is good for longs too
	rprintf("Arg1 as int: "); rprintfNum(10, 10, TRUE, ' ', cmdlineGetArgInt(1)); rprintfCRLF();
	rprintf("Arg2 as int: "); rprintfNum(10, 10, TRUE, ' ', cmdlineGetArgInt(2)); rprintfCRLF();
	rprintf("Arg3 as int: "); rprintfNum(10, 10, TRUE, ' ', cmdlineGetArgInt(3)); rprintfCRLF();
	rprintfCRLF();



}

void dumpArgsHex(void)
{
	rprintfCRLF();
	rprintf("Dump arguments as hex integers\r\n");

	rprintf("Arg1 as hex: "); rprintfNum(16, 8, FALSE, ' ', cmdlineGetArgHex(1)); rprintfCRLF();
	rprintf("Arg2 as hex: "); rprintfNum(16, 8, FALSE, ' ', cmdlineGetArgHex(2)); rprintfCRLF();
	rprintf("Arg3 as hex: "); rprintfNum(16, 8, FALSE, ' ', cmdlineGetArgHex(3)); rprintfCRLF();
	rprintfCRLF();
}
