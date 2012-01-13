/*******************************************************************************
File:			Main.c

				Main Video Overlay function library.

Functions:	extern int		main(void)
				SIGNAL(SIG_OVERFLOW0)
				SIGNAL(SIG_INTERRUPT0)
				SIGNAL(SIG_INTERRUPT1)

Created:		1.00	12/12/04	GND	Gary Dion

Revisions:	1.01	12/15/04	GND	Improved screen font and added background

Copyright(c)	2004, Gary N. Dion (me@garydion.com). All rights reserved.
					This software is available only for non-commercial amateur radio
					or educational applications.  ALL other uses are prohibited.
					This software may be modified only if the resulting code be
					made available publicly and the original author(s) given credit.

*******************************************************************************/

/* OS headers */
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/signal.h>
#include <avr/io.h>

/* General purpose include files */
#include "StdDefines.h"

/* App required include files. */
#include "Main.h"

#define WatchdogReset() asm("wdr")	/* Macro substitution to kick the dog		*/
#define Wait() while(!(SPSR & (1<<SPIF)))

/* Static functions and variables */

unsigned char line;						/* State of Delay function						*/
unsigned char print_line;				/* State of Delay function						*/
unsigned char maindelay;				/* State of mainDelay function				*/
short ltemp;						/* State of Delay function						*/
short ntemp;						/* State of Delay function						*/

unsigned char head[14] = {'N','4','T','X','I','@','B','A','L','L','O','O','N','@'};

unsigned char spd[3] = {'1','6','7'};
unsigned char hdg[3] = {'2','8','9'};

unsigned char time[6] = {'1','4','5','4','4','6'};
unsigned char date[6] = {'1','2','2','5','0','4'};

unsigned char lat[9] = {'@','3','4','3','5','.','6','8','N'};
unsigned char lon[9] = {'0','8','9','1','8','.','7','5','W'};

/*									' '  A   B   C   D   E   F   G   H   I   J   K   L   M   N   O   P   Q   R   S   T   U   V   W   X   Y   Z  */
unsigned char ltrs[189] = {255,231,131,195,135,129,129,195,189,131,129,189,191,125,125,195,131,195,131,195,131,189,189,125,125,125,  1,
									255,219,189,189,187,191,191,189,189,239,247,187,191, 57, 61,189,189,189,189,189,239,189,189,125,187,187,251,
									255,189,189,191,189,191,191,191,189,239,247,183,191, 85, 93,189,189,189,189,191,239,189,189,109,215,215,247,
									255,189,131,191,189,131,131,177,129,239,247,143,191,109,109,189,131,189,131,195,239,189,219,109,239,239,239,
									255,129,189,191,189,191,191,189,189,239,247,183,191,125,117,189,191,179,183,253,239,189,219,109,215,239,223,
									255,189,189,189,187,191,191,189,189,239,183,187,129,125,121,189,191,185,187,189,239,189,219,147,187,239,191,
									255,189,131,195,135,129,191,195,189,131,207,189,129,125,125,195,191,205,189,195,239,195,231,147,125,239,  1};

/*									-   .   /   0   1   2   3   4   5   6   7   8   9   :  */
unsigned char nums[98] = {255,255,207,131,239,131,131,227,  1,131,  1,131,131,255,
								  255,255,187,125,207,125,125,219,127,127,253,125,125,239,
								  255,255,187,125,239,253,253,187,127,127,251,125,125,255,
								  131,255,207,125,239,243,227,123,131,  3,247,131,129,255,
								  255,255,255,125,239,207,253,  1,253,125,239,125,251,255,
								  255,231,255,125,239,191,125,251,125,125,239,125,247,239,
								  255,231,255,131,199,  1,131,251,131,131,239,131,207,255};

/*$FUNCTION$*******************************************************************/
extern int		main(void)
/*******************************************************************************
* ABSTRACT:	Main program entry function.
*
* INPUT:		None
* OUTPUT:	None
*/
{
//	static unsigned short loop;			/* Generic loop variable					*/

/*Initialize serial communication functions */

/* PORT D - unused right now	*/
	PORTD = 0x00;							/* Initial state is both LEDs off			*/
	DDRD  = 0x00;							/* Data direction register for port D		*/

/* Initialize the Analog Comparator */
	SFIOR = 0;								/* Select AIN1 as neg. input					*/
	ACSR = (1<<ACBG) | (1<<ACIE);		/* Select Bandgap for pos. input				*/

/*	Initialize the 8-bit Timer0 to clock at 1.8432 MHz */
	TCCR0 = 0x01; 							/* Timer0 clock prescale of 8					*/

/* Use the 16-bit Timer1 to measure frequency; set it to clock at 1.8432 MHz	*/
	TCCR1B = 0x02;							/* Timer2 clock prescale of 8					*/

/*	Initialize the 8-bit Timer2 to clock at 1.8432 MHz */
	TCCR2 = 0x07; 							/* Timer2 clock prescale of 1024				*/

/* Initialize the Serial Peripheral Interface											*/
	PORTB = 0x00;
	DDRB = 0xFF;							/* Initial state is both LEDs off			*/
	SPCR = /* (1<<SPIE) | */ (1<<SPE) | (1<<MSTR) | (1<<CPHA);
	SPSR = 1;

/* Enable Timer interrupts	*/
//	TIMSK = 1<<TOIE0;

/* Enable the watchdog timer */
	MCUCR	= (1<<ISC01) + (1<<ISC11);	/* Set interrupt on falling edge 			*/
	GICR	= (1<<INT0) + (1<<INT1);	/* Enable interrupt for interrupt0			*/

/* Enable the watchdog timer */
	WDTCR	= (1<<WDCE) | (1<<WDE);		/* Wake-up the watchdog register				*/
	WDTCR	= (1<<WDE) | 7;				/* Enable and timeout around 2.1s			*/

/* Enable interrupts */
	sei();

/* Reset watchdog */
	WatchdogReset();


	set_sleep_mode(SLEEP_MODE_IDLE);

/* Initialization complete - system ready.  Run program loop indefinitely. */
	while (TRUE)
	{
		sleep_mode();
		WatchdogReset();
	}

	return(1);
}	/* main */


/*$FUNCTION$*******************************************************************/
SIGNAL(SIG_OVERFLOW0)
/*******************************************************************************
* ABSTRACT:	This routine now decodes packets by sampling the state of
*				rxtoggled, and assembling a packet if toggles occur.  A checksum is
*				performed for validation, then msg_end is set to message size.
*
* INPUT:		None
* OUTPUT:	None
*/
{

	TIMSK &= ~(1<<TOIE0);				/* Disable the timer0 interrupt				*/

	if ((line > 40) && (line < 48))
	{
		SPSR = 0;
		ltemp = (line - 41) * 27 - 64;
		ntemp = (line - 41) * 14 - 45;
	DDRD  = 0x80;							/* Data direction register for port D		*/
		SPDR = ltrs[head[0] + ltemp];		Wait();
		SPDR = nums[head[1] + ntemp];		Wait();
		SPDR = ltrs[head[2] + ltemp];		Wait();
		SPDR = ltrs[head[3] + ltemp];		Wait();
		SPDR = ltrs[head[4] + ltemp];		Wait();
		SPDR = ltrs[head[5] + ltemp];		Wait();
		SPDR = ltrs[head[6] + ltemp];		Wait();
		SPDR = ltrs[head[7] + ltemp];		Wait();
		SPDR = ltrs[head[8] + ltemp];		Wait();
		SPDR = ltrs[head[9] + ltemp];		Wait();
		SPDR = ltrs[head[10] + ltemp];	Wait();
		SPDR = ltrs[head[11] + ltemp];	Wait();
		SPDR = ltrs[head[12] + ltemp];	Wait();
	DDRD  = 0x00;							/* Data direction register for port D		*/
	}

	if ((line > 220) && (line < 228))
	{
		ltemp = (line - 221) * 27 - 64;
		ntemp = (line - 221) * 14 - 45;

		SPSR = 1;											/* Switch to small font		*/
	DDRD  = 0x80;							/* Data direction register for port D		*/
		SPDR = ltrs['S' + ltemp];		Wait();
		SPDR = nums[':' + ntemp];		Wait();
		SPDR = nums[spd[0] + ntemp];	Wait();		/* Hours							*/
		SPDR = nums[spd[1] + ntemp];	Wait();		/* Hours							*/
		SPDR = nums[spd[3] + ntemp];	Wait();		/* Minutes						*/

		SPDR = ltrs['@' + ltemp];		Wait();
		SPDR = ltrs['H' + ltemp];		Wait();
		SPDR = nums[':' + ntemp];		Wait();
		SPDR = nums[hdg[0] + ntemp];	Wait();		/* Hours							*/
		SPDR = nums[hdg[1] + ntemp];	Wait();		/* Hours							*/
		SPDR = nums[hdg[3] + ntemp];	Wait();		/* Minutes						*/

		SPSR = 1;											/* Switch to small font		*/
		SPDR = ltrs['@' + ltemp];		Wait();
		SPDR = ltrs['A' + ltemp];		Wait();
		SPDR = nums[':' + ntemp];		Wait();
		SPSR = 0;											/* Switch to large font		*/
		SPDR = nums[time[0] + ntemp];	Wait();		/* Hours							*/
		SPDR = nums[time[1] + ntemp];	Wait();		/* Hours							*/
		SPDR = nums[time[2] + ntemp];	Wait();		/* Minutes						*/
		SPDR = nums[time[3] + ntemp];	Wait();		/* Minutes						*/
		SPDR = nums[time[4] + ntemp];	Wait();		/* Minutes						*/
		SPDR = nums[time[5] + ntemp];	Wait();		/* Minutes						*/
	DDRD  = 0x00;							/* Data direction register for port D		*/
	}

	if ((line > 230) && (line < 238))
	{
		ltemp = (line - 231) * 27 - 64;
		ntemp = (line - 231) * 14 - 45;

		SPSR = 1;											/* Switch to small font		*/
	DDRD  = 0x80;							/* Data direction register for port D		*/
		SPDR = nums[time[0] + ntemp];	Wait();		/* Hours							*/
		SPDR = nums[time[1] + ntemp];	Wait();		/* Hours							*/
		SPDR = nums[':' + ntemp];		Wait();		/* ':'							*/
		SPDR = nums[time[2] + ntemp];	Wait();		/* Minutes						*/
		SPDR = nums[time[3] + ntemp];	Wait();		/* Minutes						*/
		SPDR = nums[':' + ntemp];		Wait();		/* ':'							*/
		SPDR = nums[time[4] + ntemp];	Wait();		/* Seconds						*/
		SPDR = nums[time[5] + ntemp];	Wait();		/* Seconds						*/
		SPDR = ltrs['@' + ltemp];		Wait();		/* ' '							*/

		SPSR = 0;											/* Switch to large font		*/
		SPDR = ltrs[lat[0] + ltemp];	Wait();		/* Degrees						*/
		SPDR = nums[lat[1] + ntemp];	Wait();		/* Degrees						*/
		SPDR = nums[lat[2] + ntemp];	Wait();		/* Degrees						*/
		SPSR = 1;											/* Switch to small font		*/
		SPDR = nums['/' + ntemp];		Wait();		/* Print degrees symbol		*/
		SPSR = 0;											/* Switch to large font		*/
		SPDR = nums[lat[3] + ntemp];	Wait();		/* Minutes						*/
		SPDR = nums[lat[4] + ntemp];	Wait();		/* Minutes						*/
		SPSR = 1;											/* Switch to small font		*/
		SPDR = nums[lat[5] + ntemp];	Wait();		/* '.'							*/
		SPSR = 0;											/* Switch to large font		*/
		SPDR = nums[lat[6] + ntemp];	Wait();		/* Decimal Minutes			*/
		SPDR = nums[lat[7] + ntemp];	Wait();		/* Decimal Minutes			*/
	DDRD  = 0x00;							/* Data direction register for port D		*/
	}

	if ((line > 240) && (line < 248))
	{
		ltemp = (line - 241) * 27 - 64;
		ntemp = (line - 241) * 14 - 45;

		SPSR = 1;											/* Switch to small font		*/
	DDRD  = 0x80;							/* Data direction register for port D		*/
		SPDR = nums[date[0] + ntemp];	Wait();		/* Month							*/
		SPDR = nums[date[1] + ntemp];	Wait();		/* Month							*/
		SPDR = nums['-' + ntemp];		Wait();		/* '-'							*/
		SPDR = nums[date[2] + ntemp];	Wait();		/* Day							*/
		SPDR = nums[date[3] + ntemp];	Wait();		/* Day							*/
		SPDR = nums['-' + ntemp];		Wait();		/* '-'							*/
		SPDR = nums[date[4] + ntemp];	Wait();		/* Year							*/
		SPDR = nums[date[5] + ntemp];	Wait();		/* Year							*/
		SPDR = ltrs['@' + ltemp];		Wait();		/* ' '							*/

		SPSR = 0;											/* Switch to large font		*/
		SPDR = nums[lon[0] + ntemp];	Wait();		/* Degrees						*/
		SPDR = nums[lon[1] + ntemp];	Wait();		/* Degrees						*/
		SPDR = nums[lon[2] + ntemp];	Wait();		/* Degrees						*/
		SPSR = 1;											/* Switch to small font		*/
		SPDR = nums['/' + ntemp];		Wait();		/* Print degrees symbol		*/
		SPSR = 0;											/* Switch to large font		*/
		SPDR = nums[lon[3] + ntemp];	Wait();		/* Minutes						*/
		SPDR = nums[lon[4] + ntemp];	Wait();		/* Minutes						*/
		SPSR = 1;											/* Switch to small font		*/
		SPDR = nums[lon[5] + ntemp];	Wait();		/* '.'							*/
		SPSR = 0;											/* Switch to large font		*/
		SPDR = nums[lon[6] + ntemp];	Wait();		/* Decimal Minutes			*/
		SPDR = nums[lon[7] + ntemp];	Wait();		/* Decimal Minutes			*/
	DDRD  = 0x00;							/* Data direction register for port D		*/
	}

}	/* SIGNAL(SIG_OVERFLOW0) */


/*$FUNCTION$*******************************************************************/
SIGNAL(SIG_INTERRUPT0)
/*******************************************************************************
* ABSTRACT:	Reset line timer at the left side of the screen.
*
* INPUT:		None
* OUTPUT:	None
*/
{
	line++;
	TCNT0 = 135;							/* Set timeout period for the first line	*/
	if (line > 220) TCNT0 = 172;		/* Set timeout period for fourth line		*/
	if (line > 230) TCNT0 = 175;		/* Set left edge of time/lat line			*/
	if (line > 240) TCNT0 = 185;		/* Set left edge of date/lon line			*/
	TIFR |= 1<<TOV0;						/* Clear any potential interrupt				*/
	TIMSK |= 1<<TOIE0;					/* Enable the timer0 interrupt				*/

}	/* SIGNAL(SIG_COMPARATOR) */


/*$FUNCTION$*******************************************************************/
SIGNAL(SIG_INTERRUPT1)
/*******************************************************************************
* ABSTRACT:	Reset line at the top of the screen.
*
* INPUT:		None
* OUTPUT:	None
*/
{
	line = 0;

}	/* SIGNAL(SIG_COMPARATOR) */
